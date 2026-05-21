#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "../triple.h"

class Material {
public:
    Color color;  // base color
    double ka;    // ambient coefficient
    double kd;    // diffuse coefficient

    Material() = default;
    Material(Color const &color, double ka, double kd)
        : color(color), ka(ka), kd(kd)
    {}
};

#endif
