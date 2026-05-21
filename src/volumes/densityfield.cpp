#include "densityfield.h"

#include "../datraw/datraw.h"

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
    return index(0, 0, 0);
}

/**
 * Find the density of the field by interpolating between the two nearest
 * neighbors on each axis.
 */
double DensityField::trilinear(double xPercent, double yPercent, double zPercent) const {
    // 3.3: Sampling the actual data
    return index(0, 0, 0);
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
