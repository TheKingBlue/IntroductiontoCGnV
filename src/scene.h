#ifndef SCENE_H_
#define SCENE_H_

#include "light.h"
#include "triple.h"
#include "volumes/segment.h"

#include <optional>
#include <utility>
#include <vector>

#include "objects/object.h"

// Forward declarations
class Ray;
class Image;

class Scene {
    unsigned imageWidth;
    unsigned imageHeight;
    std::vector<ObjectPtr> objects;
    std::vector<VolumePtr> volumes;
    std::vector<Light> lights;
    Point eye;
    Point lookAt;
    unsigned fieldOfView;
    double tStepFactor;
    bool volumeTrilinear;
    bool renderShadows;
    unsigned supersamplingFactor;

    // Before casting a new ray from a hit point, move the hit point in the
    // direction of the normal with this offset to prevent finding an
    // intersection with the same object due to floating point inaccuracies.
    // This prevents shadow acne, among other problems.
    double const epsilon = 1E-3;

public:
    Scene();

    // render the scene to the given image
    void render(Image &img) const;

private:
    Color shadePixel(unsigned px, unsigned py, unsigned w, unsigned h) const;
    Color trace(Ray const &ray) const;

    Color shadeHit(Hit const &min_hit, Ray const &ray) const;
    std::pair<Color, double> shadeSegment(Segment const &segment, Ray const &ray) const;

    double traceShadowOcclusion(Ray const &shadowRay, double distanceToLight) const;
    double shadeSegmentOpacity(Segment const &shadowSegment, Ray const &shadowRay) const;

    std::vector<Segment> intersectVolumes(Ray const &ray) const;
    std::optional<Hit> intersectObjects(Ray const &ray) const;

public:
    unsigned getImageWidth() const;
    unsigned getImageHeight() const;
    void setImageWidth(unsigned imageWidth);
    void setImageHeight(unsigned imageHeight);

    void setEye(Triple const &position);
    void setLookAt(Triple const &position);
    void setFieldOfView(unsigned fov);
    void setTStepFactor(double factor);
    void setVolumeTrilinear(bool doTrilinear);
    void setRenderShadows(bool doShadows);
    void setSuperSample(unsigned factor);

    void addObject(ObjectPtr const &object);
    void addVolume(VolumePtr const &volume);
    void addLight(Light const &light);
};

#endif
