#include "BoundingBox.h"
#include "HitRecord.h"
#include "Material.h"
#include "Point.h"
#include "PolarSphere.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Vector.h"

#include <math.h>

const double PI = 3.141592653589793;
const double INV_PI = 0.318309886;

PolarSphere::PolarSphere(Material* material, Animation<Point>& center, Animation<double> radius, Animation<Vector>& pole, Animation<Vector>& meridian)
: Sphere(material, center, radius), X(meridian), Z(pole)
{
}

void PolarSphere::preprocess(int maxTime)
{
    X.preprocess(maxTime);
    Z.preprocess(maxTime);

    for(int t = 0; t <= maxTime; ++t)
        Y.addFrame(t, Cross(X(t), Z(t)));

    Y.preprocess(maxTime);

    Sphere::preprocess(maxTime);
}

void PolarSphere::computeUVW(Vector& uvw, const RenderContext& context,
        const Ray& ray, const HitRecord& hit) const
{
    double time = context.time();
    Point P = ray.origin()+ray.direction()*hit.minT() - Vector(center(time).x(),center(time).y(),center(time).z());
    double x = Dot(P,X(time));
    double z = Dot(P,Z(time));
    double y = Dot(P,Y(time));
    double phi = acos(z*inv_radius(time));
    double theta = atan2(y,x);
    double u = theta*0.5*INV_PI;
    if(u < 0)
        u += 1.0;
    double v = 1 - phi*INV_PI;
    uvw = Vector(u, v, radius(time));
}
