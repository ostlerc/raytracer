#include "Sphere.h"
#include "BoundingBox.h"
#include "RenderContext.h"
#include "HitRecord.h"
#include "Point.h"
#include "Ray.h"
#include "Vector.h"
#include <math.h>
#include <map>

Sphere::Sphere(Material* material, Animation<Point>& center, Animation<double> radius)
: Primitive(material), center(center), radius(radius)
{
}

void Sphere::preprocess(double maxTime)
{
    center.preprocess(maxTime);
    radius.preprocess(maxTime);

    for(int i = 0; i <= maxTime; i++)
        inv_radius[i] = 1./radius(i);
}

void Sphere::getBounds(BoundingBox& bbox, const RenderContext& context) const
{
    double time = context.time();
    double radiusT = radius(time);
    Point centerT = center(time);
    Vector diag(radiusT, radiusT, radiusT);
    bbox.extend(centerT+diag);
    bbox.extend(centerT-diag);
}

void Sphere::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    double time = context.time();
    double radiusT = radius(time);
    Vector O(ray.origin()-center(time));
    const Vector& V(ray.direction());
    double b = Dot(O, V);
    double c = Dot(O, O)-radiusT*radiusT;
    double disc = b*b-c;
    if(disc > 0){
        double sdisc = sqrt(disc);
        double root1 = (-b - sdisc);
        if(!hit.hit(root1, this, matl)){
            double root2 = (-b + sdisc);
            hit.hit(root2, this, matl);
        }
    }
}

void Sphere::normal(Vector& normal, const RenderContext& context, const Point& hitpos,
        const Ray& ray, const HitRecord& hit) const
{
    double time = context.time();
    normal = (hitpos-center(time))*inv_radius.at(time);
}
