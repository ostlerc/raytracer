#include "Triangle.h"
#include "BoundingBox.h"
#include "HitRecord.h"
#include "Point.h"
#include "Ray.h"
#include "Vector.h"
#include <math.h>
#include <iostream>

Triangle::Triangle(Material* material, const Point& p0, const Point& p1,
        const Point& p2, const Vector& normal)
    : Primitive(material)
    , p0(p0)
    , p1(p1)
    , p2(p2)
    , n(normal)
{
    if(n.isNull()) //calculate a normal since none was given
        n = Cross(p1-p0,p2-p0);

    n.normalize();
}

Triangle::~Triangle()
{
}

void Triangle::getBounds(BoundingBox& bbox, const RenderContext&) const
{
    bbox.extend(p0);
    bbox.extend(p1);
    bbox.extend(p2);
}

void Triangle::intersect(HitRecord& hit, const RenderContext&, const Ray& ray) const
{
    //check plane intersection first first
    double denom = Dot(n, ray.direction());
    if(Abs(denom) <= 1.e-6)
        return;

    Vector e1, e2, h, s, q;
    double a, f, u, v;
    e1 = p1 - p0;
    e2 = p2 - p0;

    h = Cross(ray.direction(), e2);
    a = Dot(e1, h);

    if(a > -0.00001 && a < 0.00001)
        return;

    f = 1.0/a;
    s = ray.origin() - p0;
    u = f * Dot(s,h);

    if (u < 0.0 || u > 1.0)
        return;

    q = Cross(s, e1);
    v = f * Dot(ray.direction(), q);

    if (v < 0.0 || u + v > 1.0)
        return;

    // At this stage we can compute t to find out where
    // the intersection point is on the line
    double t = f * Dot(e2, q);

    if (t > 0.00001) // ray intersection
        hit.hit(t, this, matl);

    // This means that there is a line intersection
    // but not a ray intersection
}

void Triangle::normal(Vector& normal, const RenderContext&, const Point& hitpos,
        const Ray& ray, const HitRecord& hit) const
{
    normal = n;
}
