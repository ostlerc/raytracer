#include "Triangle.h"
#include "BoundingBox.h"
#include "HitRecord.h"
#include "RenderContext.h"
#include "Point.h"
#include "Ray.h"
#include "Vector.h"
#include <math.h>
#include <iostream>

Triangle::Triangle(Material* material, Animation<Point>& p0, Animation<Point>& p1,
        Animation<Point>& p2, Animation<Vector>& normal)
    : Primitive(material)
    , p0(p0)
    , p1(p1)
    , p2(p2)
    , n(normal)
{
}

Triangle::~Triangle()
{
}

void Triangle::preprocess(int maxTime)
{
    p0.preprocess(maxTime);
    p1.preprocess(maxTime);
    p2.preprocess(maxTime);
    if(n.isEmpty()) //calculate a normal since none was given
    {
        for(int i = 0; i <= maxTime; i++)
        {
            Vector _n(Cross(p1(i)-p0(i),p2(i)-p0(i)));
            _n.normalize();
            n.addFrame(i, _n);
        }
    }
    n.preprocess(maxTime);
}

void Triangle::getBounds(BoundingBox& bbox, const RenderContext& context) const
{
    bbox.extend(p0(context.time()));
    bbox.extend(p1(context.time()));
    bbox.extend(p2(context.time()));
}

void Triangle::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    double time = context.time();
    const Point& _p0 = p0(time);
    const Point& _p1 = p1(time);
    const Point& _p2 = p2(time);
    const Vector& _n = n(time);
    //check plane intersection first first
    double denom = Dot(_n, ray.direction());
    if(Abs(denom) <= 1.e-6)
        return;

    Vector e1, e2, h, s, q;
    double a, f, u, v;
    e1 = _p1 - _p0;
    e2 = _p2 - _p0;

    h = Cross(ray.direction(), e2);
    a = Dot(e1, h);

    if(a > -0.00001 && a < 0.00001)
        return;

    f = 1.0/a;
    s = ray.origin() - _p0;
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

void Triangle::normal(Vector& normal, const RenderContext& context, const Point& hitpos,
        const Ray& ray, const HitRecord& hit) const
{
    normal = n(context.time());
}
