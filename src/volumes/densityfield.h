#ifndef DENSITYFIELD_H_
#define DENSITYFIELD_H_

#include <cstdint>
#include <string>
#include <vector>

class DensityField {
    double index(unsigned x, unsigned y, unsigned z) const;
    double nearestNeighbor(double xPercent, double yPercent, double zPercent) const;
    double trilinear(double xPercent, double yPercent, double zPercent) const;

public:
    double ka;  // ambient coefficient
    double kd;  // diffuse coefficient

    std::vector<std::uint8_t> data;
    unsigned width, height, depth;

    DensityField(std::string const &path, double ka, double kd);

    double densityAt(double xPercent, double yPercent, double zPercent, bool doTrilinear) const;
};

#endif
