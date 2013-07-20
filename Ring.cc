#include "Ring.h"
#include "HitRecord.h"
#include "Math.h"
#include "Ray.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

Ring::Ring(Material* material, const Vector& normal, double r, double r2, const Point& q)
: Primitive(material), n(normal), r(r), r2(r2), q(q)
{
    n.normalize();
    d = Dot(n, q);
}

Ring::~Ring()
{
}

void Ring::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    double denom = Dot(n, ray.direction());
    if(Abs(denom) > 1.e-6){ //hit plane
        double t = (d-Dot(n, ray.origin()))/denom;
        Point hitpos = ray.origin()+ray.direction()*t;
        double l = (hitpos - q).length();
        if(l < r2 && l > r)
        {
            hit.hit(t, this, matl);
        }
    }
}

void Ring::normal(Vector& normal, const RenderContext&,
        const Point&, const Ray&, const HitRecord&) const
{
    normal = n;
}

void Ring::getBounds(BoundingBox& bbox, const RenderContext&) const
{
    //TODO:
}

