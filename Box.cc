#include "Box.h"
#include "BoundingBox.h"
#include "RenderContext.h"
#include "HitRecord.h"
#include "Point.h"
#include "Ray.h"
#include "Vector.h"

#include <iostream>
using namespace std;

Box::Box(Material* material, Animation<Point>& min, Animation<Point>& max)
: Primitive(material), min(min), max(max) {
}

Box::~Box() {
}

void Box::preprocess(double maxTime)
{
    min.preprocess(maxTime);
    max.preprocess(maxTime);

    _pairs.resize(maxTime+1);
    for(int i = 0; i <= maxTime; i++)
    {
        _pairs[i] = PointPair(min(i), max(i));
    }
}

void Box::getBounds(BoundingBox& bbox, const RenderContext& context) const
{
    bbox.extend(min(context.time()));
    bbox.extend(max(context.time()));
}

void Box::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    // Optimized Smits’ method
    // http://people.csail.mit.edu/amy/papers/box-jgt.pdf
    const PointPair& bounds = _pairs[context.time()];
    int sign[3];

    sign[0] = (ray.inv_direction().x() < 0.);
    sign[1] = (ray.inv_direction().y() < 0.);
    sign[2] = (ray.inv_direction().z() < 0.);

    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    tmin = (bounds[sign[0]].x() - ray.origin().x()) * ray.inv_direction().x();
    tmax = (bounds[1-sign[0]].x() - ray.origin().x()) * ray.inv_direction().x();
    tymin = (bounds[sign[1]].y() - ray.origin().y()) * ray.inv_direction().y();
    tymax = (bounds[1-sign[1]].y() - ray.origin().y()) * ray.inv_direction().y();
    if ( (tmin > tymax) || (tymin > tmax) )
        return;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    tzmin = (bounds[sign[2]].z() - ray.origin().z()) * ray.inv_direction().z();
    tzmax = (bounds[1-sign[2]].z() - ray.origin().z()) * ray.inv_direction().z();
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    hit.hit(tmin, this, matl);
}

void Box::normal(Vector& normal, const RenderContext& context, const Point& hitpos,
        const Ray& ray, const HitRecord& hit) const
{
    //by subtracting hitpos from our point we get a localized point. The
    //component with a value of zero is the side of the box that we hit

    double time = context.time();

    const Vector& localMin = (hitpos - min(time)).absoluteValue();
    const Vector& localMax = (hitpos - max(time)).absoluteValue();

    //x normals
    if(localMin.x() < EPSILON)
        normal = Vector(-1, 0, 0);
    else if(localMax.x() < EPSILON)
        normal = Vector( 1, 0, 0);

    //y normals
    else if(localMin.y() < EPSILON)
        normal = Vector(0, -1, 0);
    else if(localMax.y() < EPSILON)
        normal = Vector(0,  1, 0);

    //z normals
    else if(localMin.z() < EPSILON)
        normal = Vector(0, 0, -1);
    else if(localMax.z() < EPSILON)
        normal = Vector(0, 0,  1);
    else
        cerr << "broken" << endl;
}
