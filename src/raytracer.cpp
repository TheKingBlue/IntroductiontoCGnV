#include "raytracer.h"

#include "json/json_wrapper.h"
#include "image.h"
#include "light.h"
#include "triple.h"
#include "objects/material.h"
#include "objects/shapes/sphere.h"
#include "volumes/volume.h"
#include "volumes/densityfield.h"

#include <exception>
#include <fstream>
#include <iostream>

using namespace std;  // no std:: required
using json = nlohmann::json;

VolumePtr Raytracer::parseVolume(json const &node) {
    Point aa = parseTriple(node.at("min_bounds"));
    Point bb = parseTriple(node.at("max_bounds"));
    double ka = node.at("ka");
    double kd = node.at("kd");
    DensityField data(node.at("volume_data_path").get<string>(), ka, kd);
    TransferFunction tf = parseTransferFunction(node.at("transfer_function"));

    return make_shared<Volume>(aa, bb, data, tf);
}

ObjectPtr Raytracer::parseObject(json const &node) {
    ObjectPtr object;

    if (node.at("type") == "sphere") {
        Point pos = parseTriple(node.at("position"));
        double radius = node.at("radius");
        object = make_shared<Sphere>(pos, radius);
    }

    object->material = parseMaterial(node.at("material"));
    return object;
}

Light Raytracer::parseLight(json const &node) {
    Point pos = parseTriple(node.at("position"));
    Color col = parseTriple(node.at("color"));
    return Light(pos, col);
}

Material Raytracer::parseMaterial(json const &node) {
    Color color = parseTriple(node.at("color"));
    double ka = node.at("ka");
    double kd = node.at("kd");
    return Material(color, ka, kd);
}

Triple Raytracer::parseTriple(json const &node) {
    if (!node.is_array()) throw runtime_error("Triple JSON node is not an array");
    if (node.size() != 3) throw runtime_error("Triple JSON array size != 3");
    if (!node[0].is_number()) throw runtime_error("Triple JSON array is not numerical");
    return Triple(node[0], node[1], node[2]);
}

TransferFunction Raytracer::parseTransferFunction(json const &node) {
    if (!node.is_array()) throw runtime_error("Transfer function JSON node is not an array");

    TransferFunction tf;
    for (nlohmann::json const &entry: node) {
        Color color = parseTriple(entry.at("color"));
        tf.points.emplace_back(entry.at("density"), color, entry.at("opacity"));
    }
    return tf;
}

bool Raytracer::readScene(string const &ifname) try {
    // Read and parse input JSON file
    ifstream infile(ifname);
    if (!infile)
        throw runtime_error("Could not open input file for reading.");

    json jsonscene;
    infile >> jsonscene;

    // =============================================================================
    // -- Read your scene data in this section -------------------------------------
    // =============================================================================

    scene.setImageWidth(jsonscene.at("ImageWidth"));
    scene.setImageHeight(jsonscene.at("ImageHeight"));
    scene.setEye(parseTriple(jsonscene.at("Eye")));
    scene.setLookAt(parseTriple(jsonscene.at("LookAt")));
    scene.setFieldOfView(jsonscene.at("FieldOfView"));

    if (jsonscene.count("VolumeTStep"))
        scene.setTStepFactor(jsonscene.at("VolumeTStep"));

    if (jsonscene.count("VolumeTrilinear"))
        scene.setVolumeTrilinear(jsonscene.at("VolumeTrilinear"));

    if (jsonscene.count("SuperSamplingFactor"))
        scene.setSuperSample(jsonscene["SuperSamplingFactor"]);

    if (jsonscene.count("Shadows"))
        scene.setRenderShadows(jsonscene.at("Shadows"));

    for (auto const &lightNode : jsonscene.at("Lights"))
        scene.addLight(parseLight(lightNode));

    unsigned objCount = 0;
    for (auto const &objectNode : jsonscene.at("Objects")) {
        scene.addObject(parseObject(objectNode));
        ++objCount;
    }
    cout << "Parsed " << objCount << " object(s).\n";

    unsigned volumeCount = 0;
    for (auto const &volumeNode : jsonscene.at("Volumes")) {
        scene.addVolume(parseVolume(volumeNode));
        ++volumeCount;
    }
    cout << "Parsed " << volumeCount << " volume(s).\n";

    // =============================================================================
    // -- End of scene data reading ------------------------------------------------
    // =============================================================================

    return true;
} catch (exception const &ex) {
    cerr << ex.what() << '\n';
    return false;
}

void Raytracer::renderToFile(string const &ofname) const {
    unsigned width = scene.getImageWidth();
    unsigned height = scene.getImageHeight();
    Image img(width, height);
    cout << "Tracing...\n";
    scene.render(img);
    cout << "Writing image to " << ofname << "...\n";
    img.write_png(ofname);
    cout << "Done.\n";
}
