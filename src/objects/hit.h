#ifndef HIT_H_
#define HIT_H_

#include "../triple.h"

#include <memory>
#include <utility>

// Forward declare Object and ObjectPtr to avoid cyclic dependency
class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Hit {
public:
    double t;          // distance of hit
    Vector N;          // normal at hit
    ObjectPtr object;  // object that was hit

    Hit(double time, Vector const &normal, ObjectPtr object)
        : t(time), N(normal), object(std::move(object))
    {}
};

#endif
