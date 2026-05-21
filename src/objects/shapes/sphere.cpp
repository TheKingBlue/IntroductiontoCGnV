#include "sphere.h"

#include <cmath>

using namespace std;

Sphere::Sphere(Point const &pos, double radius, Vector const &axis, double angle)
:   position(pos),
    r(radius),
    axis(axis.normalized()),
    angle(angle / 180.0 * PI)
{}

/**
 * Determines the intersection point, if any, of the sphere and a ray. The hit
 * point is guaranteed to be in the positive direction of the ray
 * (t-value >= 0).
 * @return The hit point, wrapped in a std::optional. Could be std::nullopt in
 * case the ray does not intersect the sphere.
 */
optional<Hit> Sphere::intersect(Ray const &ray) {
    // 2.1: Sphere intersection
    // Placeholder for actual intersection calculation.
    Vector originToPosition = (position - ray.O).normalized();
    if (originToPosition.dot(ray.D) < 0.999) return nullopt;

    double t = 1000;

    // 2.2.1: Normal calculation
    Vector N(0, 0, 0);

    return Hit(t, N, shared_from_this());
}

/**
 * Computes the quadratic formula. Values are returned as follows:
 *
 * - No solution: returns false, t0 and t1 unchanged
 * - One solution: returns true, t0 and t1 both contain the solution
 * - Two solutions: returns true, t0 and t1 contain the solutions
 */
bool Sphere::quadratic(double a, double b, double c, double &t0, double &t1) {
    // 2.1: Sphere intersection
    return false;
}
