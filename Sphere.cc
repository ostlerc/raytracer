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

Sphere::~Sphere()
{
}

void Sphere::preprocess(double maxTime)
{
    center.preprocess(maxTime);
    radius.preprocess(maxTime);
}

void Sphere::getBounds(BoundingBox& bbox, const RenderContext& context) const
{
    double time = context.time();
    double rtime = radius(time);
    Vector diag(rtime, rtime, rtime);
    bbox.extend(center(time)+diag);
    bbox.extend(center(time)-diag);
}

void Sphere::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    double time = context.time();
    double rtime = radius(time);
    Vector O(ray.origin()-center(time));
    const Vector& V(ray.direction());
    double b = Dot(O, V);
    double c = Dot(O, O)-rtime*rtime;
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
    static std::map<double, double> inv_radius;
    if(inv_radius.find( time ) != inv_radius.end())
        inv_radius[time] = 1./radius(time);
    normal = (hitpos-center(time))*inv_radius[time];
}
