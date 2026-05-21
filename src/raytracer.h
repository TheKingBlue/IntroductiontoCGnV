#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "json/json_fwd.h"
#include "scene.h"
#include "volumes/transferfunction.h"

#include <string>

// Forward declarations
class Light;
class Material;

class Raytracer {
    Scene scene;

public:
    bool readScene(std::string const &ifname);
    void renderToFile(std::string const &ofname) const;

private:
    static VolumePtr parseVolume(nlohmann::json const &node);
    static ObjectPtr parseObject(nlohmann::json const &node);
    static Light parseLight(nlohmann::json const &node);
    static Material parseMaterial(nlohmann::json const &node);
    static Triple parseTriple(nlohmann::json const &node);
    static TransferFunction parseTransferFunction(nlohmann::json const &node);
};

#endif
