#ifndef VOLUME_H_
#define VOLUME_H_

#include "../ray.h"
#include "densityfield.h"
#include "segment.h"
#include "transferfunction.h"

#include <optional>

class Volume;
typedef std::shared_ptr<Volume> VolumePtr;

class Volume: public std::enable_shared_from_this<Volume> {
public:
    // Bounding box (inclusive)
    Point const aa, bb;
    int width, height, depth;
    DensityField data;
    double minVoxelSize;
    TransferFunction transferFunction;

    Volume(Point const &aa, Point const &bb, DensityField data, TransferFunction tf);

    std::optional<Segment> intersect(Ray const &ray);
    Sample sample(Point const &point, bool doTrilinear) const;
    Vector gradient(Point const &point, bool doTrilinear) const;
};

#endif
