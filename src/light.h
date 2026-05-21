#ifndef LIGHT_H_
#define LIGHT_H_

#include "triple.h"

class Light {
public:
    Point const position;
    Color const color;

    Light(Point const &pos, Color const &c)
        : position(pos), color(c)
    {}
};

#endif
