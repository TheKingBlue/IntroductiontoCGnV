#include "densityfield.h"

#include "../datraw/datraw.h" // IWYU pragma: keep

#include <algorithm>
#include <cmath>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

/**
 * Reads density field data from the given file.
 */
DensityField::DensityField(std::string const &path, double ka, double kd)
:   ka(ka), kd(kd)
{
    using reader = datraw::raw_reader<char>;
    reader r;
    try {
        r = reader::open(path);
    } catch (std::runtime_error const &ex) {
        std::stringstream msg;
        msg << "Failed to read DAT file at " << fs::path(path) << ".\n"
            << "Please check that this full path is correct: "
            << fs::weakly_canonical(fs::current_path() / path) << "\n";
        throw std::runtime_error(msg.str());
    }

    width  = r.info().resolution()[0];
    height = r.info().resolution()[1];
    depth  = r.info().resolution()[2];
    size_t dataLength = width * height * depth;
    data.resize(dataLength);

    r.read_current(data.data(), dataLength);
}

/**
 * Indexes the 1D data array at the given coordinates. Since the data is
 * stored as std::uint8_t to save space, we use UINT8_MAX to scale to [0, 1].
 */
double DensityField::index(unsigned x, unsigned y, unsigned z) const {
    return static_cast<double>(data[z * width * height + y * width + x]) / UINT8_MAX;
}

/**
 * Find the density of the field by rounding to the nearest neighbor data point.
 */
double DensityField::nearestNeighbor(double xPercent, double yPercent, double zPercent) const {
    // 3.3: Sampling the actual data
    unsigned int x = clamp(round(xPercent * width), 0.0, width-1.0);
    unsigned int y = clamp(round(yPercent * height), 0.0, height-1.0);
    unsigned int z = clamp(round(zPercent * depth), 0.0, depth-1.0);
    
    return index(x, y, z);;
}

/**
 * Find the density of the field by interpolating between the two nearest
 * neighbors on each axis.
 */
double DensityField::trilinear(double xPercent, double yPercent, double zPercent) const {
    // 3.3: Sampling the actual data
    double x = xPercent * width;
    double y = yPercent * height;
    double z = zPercent * depth;

    unsigned int xLow = clamp(floor(x), 0.0, width-1.0);
    unsigned int yLow = clamp(floor(y), 0.0, height-1.0);
    unsigned int zLow = clamp(floor(z), 0.0, depth-1.0);
    unsigned int xHigh = clamp(ceil(x), 0.0, width-1.0);
    unsigned int yHigh = clamp(ceil(y), 0.0, height-1.0);
    unsigned int zHigh = clamp(ceil(z), 0.0, depth-1.0);

    double xD = (x - xLow);
    double yD = (y - yLow);
    double zD = (z - zLow);

    double c000 = index(xLow,yLow,zLow);
    double c100 = index(xHigh,yLow,zLow);
    double c001 = index(xLow,yLow,zHigh);
    double c101 = index(xHigh,yLow,zHigh);
    double c010 = index(xLow,yHigh,zLow);
    double c110 = index(xHigh,yHigh,zLow);
    double c011 = index(xLow,yHigh,zHigh);
    double c111 = index(xHigh,yHigh,zHigh);

    double c00 = c000 * (1-xD) + c100 * xD;
    double c01 = c001 * (1-xD) + c101 * xD;
    double c10 = c010 * (1-xD) + c110 * xD;
    double c11 = c011 * (1-xD) + c111 * xD;

    double c0 = c00 * (1-yD) + c10 * yD;
    double c1 = c01 * (1-yD) + c11 * yD;

    double c = c0 * (1-zD) + c1 * zD;

    return c;
}

/**
 * Finds the density of the field according to the given interpolation method.
 * The percentage parameters are relative to the boundary on each axis.
 */
double DensityField::densityAt(double xPercent, double yPercent, double zPercent, bool doTrilinear) const {
    if (!doTrilinear)
        return nearestNeighbor(xPercent, yPercent, zPercent);
    return trilinear(xPercent, yPercent, zPercent);
}
