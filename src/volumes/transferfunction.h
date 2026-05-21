#ifndef TRANSFERFUNCTION_H_
#define TRANSFERFUNCTION_H_

#include "../triple.h"

#include <vector>

struct Sample {
    Color color{1.0, 1.0, 1.0};
    double opacity = 1.0;
};

struct TFPoint {
    double density;
    Color color;
    double opacity;

    TFPoint(double density, Color const &color, double opacity) : density(density), opacity(opacity) {
        this->color = opacity * color; // Store pre-multiplied colors
    }
};

class TransferFunction {
public:
    std::vector<TFPoint> points;

    Sample operator()(double density) const;
};

#endif
