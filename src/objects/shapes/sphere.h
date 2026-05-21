#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere: public Object {
    double const PI = 3.14159265358979323846;

    bool quadratic(double a, double b, double c, double &t0, double &t1);

public:
    Point const position;
    double const r;
    Vector const axis;
    double const angle;

    Sphere(Point const &pos, double radius,
           Vector const &axis = Vector(0.0, 1.0, 0.0), double angle = 0.0);

    std::optional<Hit> intersect(Ray const &ray) override;

};

#endif
