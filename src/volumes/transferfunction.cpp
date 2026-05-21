#include "transferfunction.h"

using namespace std;

/**
 * Samples the transfer function at the given density.
 * @return A Sample object containing the resulting color and opacity.
 */
Sample TransferFunction::operator()(double density) const {
    TFPoint first = points.front();
    if (density < first.density)
        return {first.color, first.opacity};

    for (unsigned i = 0; i < points.size() - 1; ++i) {
        // Find the two entries to interpolate between
        if (density > points[i + 1].density)
            continue;
        // Now, points[i] < density <= points[i + 1]
        double ratio = (density - points[i].density) / (points[i + 1].density - points[i].density);
        return {
            points[i].color * (1 - ratio) + points[i + 1].color * ratio,
            points[i].opacity * (1 - ratio) + points[i + 1].opacity * ratio
        };
    }

    TFPoint last = points.back();
    return {last.color, last.opacity};
}
