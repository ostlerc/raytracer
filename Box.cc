#include "Box.h"
#include "BoundingBox.h"
#include "RenderContext.h"
#include "Material.h"
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

void Box::preprocess(int maxTime)
{
    min.preprocess(maxTime);
    max.preprocess(maxTime);

    _pairs.resize(maxTime+1);
    for(int i = 0; i <= maxTime; i++)
    {
        _pairs[i] = PointPair(min(i), max(i));
    }
    matl->preprocess(maxTime);
}

void Box::getBounds(BoundingBox& bbox, const RenderContext& context) const
{
    bbox.extend(min(context.time()));
    bbox.extend(max(context.time()));
}

void Box::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    double tmin, tmax;
    BoundingBox bbox;
    getBounds(bbox, context);

    if(bbox.intersects(ray, tmin, tmax))
        if(!hit.hit(tmin, this, matl))
            hit.hit(tmax, this, matl);
}

void Box::normal(Vector& normal, const RenderContext& context, const Point& hitpos,
        const Ray& ray, const HitRecord& hit) const
{
    //by subtracting hitpos from our point we get a localized point. The
    //component with a value of zero is the side of the box that we hit

    double time = context.time();

    const Vector& localMin = (hitpos - min(time)).absoluteValue();
    const Vector& localMax = (hitpos - max(time)).absoluteValue();

    const double e = .001;

    //x normals
    if(localMin.x() < e)
        normal = Vector(-1, 0, 0);
    else if(localMax.x() < e)
        normal = Vector( 1, 0, 0);

    //y normals
    else if(localMin.y() < e)
        normal = Vector(0, -1, 0);
    else if(localMax.y() < e)
        normal = Vector(0,  1, 0);

    //z normals
    else if(localMin.z() < e)
        normal = Vector(0, 0, -1);
    else if(localMax.z() < e)
        normal = Vector(0, 0,  1);
    else
        cerr << "broken" << localMax << localMin << endl;
}
