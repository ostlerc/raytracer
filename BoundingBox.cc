
#include "BoundingBox.h"
#include <float.h>

BoundingBox::BoundingBox()
{
  min = Point(DBL_MAX, DBL_MAX, DBL_MAX);
  max = Point(-DBL_MAX, -DBL_MAX, -DBL_MAX);
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::extend(const Point& p)
{
  min = Min(p, min);
  max = Max(p, max);
}

void BoundingBox::extend(const BoundingBox &bb)
{
    min = Min(bb.min, min);
    max = Max(bb.max, max);
}

bool BoundingBox::intersects(const Ray& ray, double& tmin, double& tmax) const
{
    // Optimized Smits’ method
    // http://people.csail.mit.edu/amy/papers/box-jgt.pdf
    int sign[3];
    PointPair bounds(min, max);

    sign[0] = (ray.inv_direction().x() < 0.);
    sign[1] = (ray.inv_direction().y() < 0.);
    sign[2] = (ray.inv_direction().z() < 0.);

    float tymin, tymax, tzmin, tzmax;
    tmin = (bounds[sign[0]].x() - ray.origin().x()) * ray.inv_direction().x();
    tmax = (bounds[1-sign[0]].x() - ray.origin().x()) * ray.inv_direction().x();
    tymin = (bounds[sign[1]].y() - ray.origin().y()) * ray.inv_direction().y();
    tymax = (bounds[1-sign[1]].y() - ray.origin().y()) * ray.inv_direction().y();
    if ( (tmin > tymax) || (tymin > tmax) )
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;
    tzmin = (bounds[sign[2]].z() - ray.origin().z()) * ray.inv_direction().z();
    tzmax = (bounds[1-sign[2]].z() - ray.origin().z()) * ray.inv_direction().z();
    if ( (tmin > tzmax) || (tzmin > tmax) )
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

double BoundingBox::distance(BoundingBox& bb)
{
    double minx = DBL_MAX, miny = DBL_MAX, minz = DBL_MAX;

    minx = Min(
            Min(
                distance(min.x(), bb.max.x()),
                distance(max.x(), bb.min.x())),
            Min(
                distance(min.x(), bb.min.x()),
                distance(max.x(), bb.max.x())));

    miny = Min(
            Min(
                distance(min.y(), bb.max.y()),
                distance(max.y(), bb.min.y())),
            Min(
                distance(min.y(), bb.min.y()),
                distance(max.y(), bb.max.y())));

    minz = Min(
            Min(
                distance(min.z(), bb.max.z()),
                distance(max.z(), bb.min.z())),
            Min(
                distance(min.z(), bb.min.z()),
                distance(max.z(), bb.max.z())));

    Vector min(minx, miny, minz);

    return min.length();
}

double BoundingBox::distance(double p1, double p2)
{
    return Abs(p1 - p2);
}

bool BoundingBox::operator <(const BoundingBox& other) const
{
    //sort on x, good enough for top down sorting
    return min.x() < other.min.x();
}
