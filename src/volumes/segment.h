#ifndef SEGMENT_H_
#define SEGMENT_H_

#include <cmath>
#include <memory>
#include <utility>

// Forward declare Volume to avoid cyclic dependency
class Volume;
typedef std::shared_ptr<Volume> VolumePtr;

class Segment {
public:
    double t1;         // distance to segment start
    double t2;         // distance to segment end
    VolumePtr volume;  // volume that was intersected

    Segment(double const t1, double const t2, VolumePtr volume)
        : t1(t1), t2(t2), volume(std::move(volume))
    {}

    /**
     * Cap the segment so that it cuts off at the given t.
     */
    void cap(double t) {
        t1 = std::min(t1, t);
        t2 = std::min(t2, t);
    }

    double length() const {
        return t2 - t1;
    }

    // To be able to sort a vector of Segments.
    // Assumes volumes do not overlap.
    bool operator<(Segment const &rhs) const {
        return t2 < rhs.t1;
    }
};

#endif
