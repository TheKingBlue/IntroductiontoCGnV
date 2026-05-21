#ifndef OBJECT_H_
#define OBJECT_H_

#include "../ray.h"
#include "hit.h"
#include "material.h"

#include <optional>

class Object;
typedef std::shared_ptr<Object> ObjectPtr;

class Object: public std::enable_shared_from_this<Object> {
public:
    Material material;

    // Required because we have an abstract method
    virtual ~Object() = default;

    // Abstract method, must be implemented in derived class
    virtual std::optional<Hit> intersect(Ray const &ray) = 0;
};

#endif
