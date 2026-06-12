#include "scene.h"

#include "image.h"
#include "ray.h"
#include "objects/object.h"
#include "volumes/volume.h"

#include <algorithm>
#include <optional>

using namespace std;

// --- Public functions --------------------------------------------------------

void Scene::render(Image &img) const {
    unsigned w = img.width;
    unsigned h = img.height;

#pragma omp parallel for
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            img(x, y) = shadePixel(x, h - 1 - y, w, h).clamp();
        }
    }
}

// --- Private functions -------------------------------------------------------

Color Scene::shadePixel(unsigned px, unsigned py, unsigned w, unsigned h) const {
    Color color(0.0, 0.0, 0.0);

    // Set up camera
    Vector forward = (lookAt - eye).normalized();
    Vector right = forward.cross(Vector(0.0, 1.0, 0.0)).normalized();
    if (std::fabs(forward.y) == 1) // Fix right if looking straight up/down
        right = Vector(0.0, 0.0, 1.0);
    Vector up = right.cross(forward);
    double halfH = std::tan(fieldOfView * M_PI / 360.0);
    double halfW = halfH * static_cast<double>(w) / h;

    for (unsigned sx = 0; sx < supersamplingFactor; ++sx) {
        for (unsigned sy = 0; sy < supersamplingFactor; ++sy) {
            double dx = (sx + 0.5) / supersamplingFactor;
            double dy = (sy + 0.5) / supersamplingFactor;

            double u = ((px + dx) / w - 0.5) * 2.0 * halfW;
            double v = ((py + dy) / h - 0.5) * 2.0 * halfH;
            Vector dir = (forward + u * right + v * up).normalized();
            Ray ray(eye, dir);

            color += trace(ray);
        }
    }

    return color / (supersamplingFactor * supersamplingFactor);
}

/**
 * Traces a ray through the scene by testing for intersection with objects and
 * later volumes.
 *
 * @param ray The ray to determine the resulting color of.
 * @return The color of the ray, in the range [0, 1].
 */
Color Scene::trace(Ray const &ray) const {
    optional<Hit> objectHit = intersectObjects(ray);
    vector<Segment> volumeHit = intersectVolumes(ray);
    Color color(0.0, 0.0, 0.0);
    double opacity = 0.0;
   
    // If both an abject and a volume were hit, only return the closest color
    if (objectHit && volumeHit.size() > 0){
        // Object in front of the volume
        if (objectHit.value().t < volumeHit[0].t1)
            return shadeHit(objectHit.value(), ray).clamp();

        // Object behind the volume
        if (objectHit.value().t > volumeHit[0].t1) {
            for (long unsigned int i = 0; i < volumeHit.size(); i++) {
                if (objectHit.value().t < volumeHit[i].t1) {
                    color = shadeHit(objectHit.value(), ray);
                    break;
                } 
                pair segment = shadeSegment(volumeHit[i], ray);
                color += (1 - opacity) * segment.first;
                opacity += (1 - opacity) * segment.second;

                // Early ray termination
                if (opacity > 0.99) break;
            }
            color += shadeHit(objectHit.value(), ray);
            return color.clamp();
        }
    }

    // Only object(s) hit
    else if (objectHit)
        return shadeHit(objectHit.value(), ray).clamp();

    // Only volume(s) hit
    else if (volumeHit.size() > 0) {
            for (long unsigned int i = 0; i < volumeHit.size(); i++) {
                pair segment = shadeSegment(volumeHit[i], ray);
                color += (1 - opacity) * segment.first;
                opacity += (1 - opacity) * segment.second;

                // Early ray termination
                if (opacity > 0.99) break;
            }
            return color.clamp();
    }

    // Hit nothing
    return Color(0,0,0);
}

/**
 * Determines the color of the object at the hit point using ambient and
 * diffuse shading.
 *
 * Hints: (see triple.h)
 *  - Triple.dot(Vector) dot product
 *  - Vector + Vector    vector sum
 *  - Vector - Vector    vector difference
 *  - Point - Point      yields vector
 *  - Vector.normalize() normalizes vector, returns length
 *  - double * Color     scales each color component (r,g,b)
 *  - Color * Color      ditto
 *
 * @param min_hit The hit closest to the camera. Contains a pointer to the
 * object that was hit.
 */
Color Scene::shadeHit(Hit const &min_hit, Ray const &ray) const {
    Material const &material = min_hit.object->material; // the hit object's material
    Point hit = ray.at(min_hit.t);      // the hit point
    Vector N = min_hit.N;               // the normal at the hit point
    Vector V = -ray.D;                  // the view vector

    // 2.1: Ambient component
    Color ia = Color(1,1,1) * material.ka;

    // 2.2.1: Normal calculation
    [[maybe_unused]] Color normalMap = (N+1)/2;
    if (N.dot(V) < 0) N *= -1;

    // 2.2.2: Diffuse component
    // 2.3: Multiple lights
    Color id = Color(0,0,0);
    Vector L = Vector(0,0,0);
    for (long unsigned int i = 0; i < lights.size(); i++) {
        Light light = lights[i];
        Vector L = (light.position - hit).normalized();

        // 2.4: Shadows
        if (renderShadows) {
            // Avoid shadow acne
            hit += epsilon*N;

            Ray ray = Ray(hit, L);
            optional<Hit> shadowHit = intersectObjects(ray);
            
            if(!(shadowHit && shadowHit->t < min_hit.t))
                id += light.color * material.kd * max(0.0,N.dot(L));
        }

        // Without considering shadows
        else
            id += light.color * material.kd * max(0.0,N.dot(L));
    }

    // 3.6: Sphere and volume integration

    // Combine components
    Color i = ia;
    if (N.dot(L) >= 0) i += id;

    Color color = material.color * i;
    
    return color;
}

/**
 * Determines the color and opacity of the volume segment using ambient and
 * diffuse shading.
 */
pair<Color, double> Scene::shadeSegment(Segment const &segment, Ray const &ray) const {
    Color color(0.0, 0.0, 0.0);
    double opacity = 0.0;
    VolumePtr const &volume = segment.volume;
    double tStep = tStepFactor * volume->minVoxelSize;
    DensityField const &volumeData = volume->data;
    Vector V = -ray.D;

    // 3.2: Compositing
    double t;
    double numSteps = segment.length() / tStep;
    for (int i = 0; i < numSteps; i++) {
        t = segment.t1 + (tStep*i);
        Point tPos = ray.at(t);
        Sample sample = volume->sample(tPos, volumeTrilinear);

        // 3.4: Diffuse shading
        if (sample.opacity > 1e-6) {
            Color gradient = volume->gradient(tPos, volumeTrilinear);
            Color ia = Color(1,1,1) * volumeData.ka;
            Color id = Color(0, 0, 0);

            if (gradient.length() > epsilon) {
                // Normal calculation
                Vector N = gradient.normalized();
                if (N.dot(V) < 0) N *= -1;

                for (long unsigned int j = 0; j < lights.size(); j++) {
                    Light light = lights[j];
                    Vector L = (light.position - tPos).normalized();

                    id += light.color * volumeData.kd * max(0.0,N.dot(L));
                }
            }
            color += (1 - opacity) * (ia + id) * sample.color; 
        }
        opacity += (1 - opacity) * sample.opacity;

        // Early ray termination
        if (opacity > 0.99) break;
    }
    // 3.5: Shadows

    return {color, opacity};
}

/**
 * Like Scene::trace, but determines only the accumulated opacity of the shadow
 * ray, which is directed towards the light.
 */
double Scene::traceShadowOcclusion(Ray const &shadowRay, double distanceToLight) const {
    // 3.5: Shadows
    return 0.4;
}

/**
 * Like Scene::shadeSegment, but determines only the opacity of the shadow ray
 * along the volume segment.
 * @param shadowSegment The segment of the volume blocking the shadow ray.
 * Contains a pointer to the volume.
 */
double Scene::shadeSegmentOpacity(Segment const &shadowSegment, Ray const &shadowRay) const {
    double opacity = 0.0;
    [[maybe_unused]] double tStep = tStepFactor * shadowSegment.volume->minVoxelSize;

    // 3.5: Shadows
    opacity = 0.4;

    return opacity;
}

/**
 * Returns all of the ray's intersections with volumes in the scene. All
 * segments have positive t-values. The list is sorted based on distance.
 */
vector<Segment> Scene::intersectVolumes(Ray const &ray) const {
    vector<Segment> intersections;

    for (VolumePtr const &volume : volumes) {
        if (optional<Segment> segment = volume->intersect(ray))
            intersections.emplace_back(segment.value());
    }

    sort(intersections.begin(), intersections.end());
    return intersections;
};

/**
 * If the ray intersects an object in the scene, returns the intersection point
 * with the first hit object. If no intersection exists, returns std::nullopt.
 */
optional<Hit> Scene::intersectObjects(Ray const &ray) const {
    optional<Hit> minHit;
    for (ObjectPtr const &object : objects) {
        optional<Hit> hit = object->intersect(ray);
        if (!hit) continue;
        if (!minHit || hit->t < minHit->t)
            minHit = hit;
    }
    return minHit;
}

// --- Misc functions ----------------------------------------------------------

// Defaults
Scene::Scene()
:
    imageWidth(400),
    imageHeight(400),
    fieldOfView(45),
    tStepFactor(0.5),
    volumeTrilinear(false),
    renderShadows(false),
    supersamplingFactor(1)
{}

unsigned Scene::getImageWidth() const {
    return imageWidth;
}

unsigned Scene::getImageHeight() const {
    return imageHeight;
}

void Scene::setImageWidth(unsigned width) {
    imageWidth = width;
}

void Scene::setImageHeight(unsigned height) {
    imageHeight = height;
}

void Scene::setEye(Triple const &position) {
    eye = position;
}

void Scene::setLookAt(Triple const &position) {
    lookAt = position;
}

void Scene::setFieldOfView(unsigned fov) {
    fieldOfView = fov;
}

void Scene::setTStepFactor(double factor) {
    tStepFactor = factor;
}

void Scene::setVolumeTrilinear(bool doTrilinear) {
    volumeTrilinear = doTrilinear;
}

void Scene::setRenderShadows(bool doShadows) {
    renderShadows = doShadows;
}

void Scene::setSuperSample(unsigned factor) {
    supersamplingFactor = factor;
}

void Scene::addObject(ObjectPtr const &object) {
    objects.push_back(object);
}

void Scene::addVolume(VolumePtr const &volume) {
    volumes.push_back(volume);
}

void Scene::addLight(Light const &light) {
    lights.push_back(light);
}
