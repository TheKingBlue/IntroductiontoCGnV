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
    double a = ray.D.dot(ray.D);
    double b = ray.D.dot(2*(ray.O - position));
    double c = (ray.O - position).dot(ray.O - position) - pow(r, 2);

    double t0, t1;

    bool tState = Sphere::quadratic(a, b, c, t0, t1);

    if (tState == false) return nullopt;

    double t ;
    // Ray travels behind the origin towards the object
    if (t0 <= 0 && t1 <= 0)
        return nullopt;
    // One negative t solution means we are in the sphere
    else if (t0 <= 0 || t1 <= 0)
        t = t0 > 0 ? t0 : t1;
    else
        t = t0 < t1 ? t0 : t1;

    // 2.2.1: Normal calculation
    if (r == 0) return Hit(t, Vector(0,0,0), shared_from_this());

    Point p = ray.O + t * ray.D;
    Vector N = (p-position) / r;

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
    double d = pow(b, 2) - 4 * a * c;
    
    // One solution
    if (d == 0) {
        double t = -b / (2*a);
        t0 = t;
        t1 = t;
        return true;
    }

    // Two solutions
    if (d > 0) {
        t0 = (-b - sqrt(d)) / (2*a);
        t1 = (-b + sqrt(d)) / (2*a);
        return true;
    }

    // No solution
    return false;
}
