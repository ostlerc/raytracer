#include "Disc.h"
#include "HitRecord.h"
#include "Math.h"
#include "Ray.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

Disc::Disc(Material* material, const Vector& normal, double r, const Point& q)
: Primitive(material), n(normal), r(r), q(q)
{
    n.normalize();
    d = Dot(n, q);
}

Disc::~Disc()
{
}

void Disc::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    double denom = Dot(n, ray.direction());
    if(Abs(denom) > 1.e-6){ //hit plane
        double t = (d-Dot(n, ray.origin()))/denom;
        Point hitpos = ray.origin()+ray.direction()*t;
        if((hitpos - q).length() < r)
            hit.hit(t, this, matl);
    }
}

void Disc::normal(Vector& normal, const RenderContext&,
        const Point&, const Ray&, const HitRecord&) const
{
    normal = n;
}

void Disc::getBounds(BoundingBox& bbox, const RenderContext&) const
{
    //TODO: 
}

