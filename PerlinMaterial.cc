#include "PerlinMaterial.h"
#include "HitRecord.h"
#include "Light.h"
#include "Point.h"
#include "Primitive.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Scene.h"
#include "Vector.h"
#include "Math.h"
#include "PerlinNoise.h"

#include <iostream>

using namespace std;

PerlinMaterial::PerlinMaterial(Material* m1, Material* m2)
:m1(m1), m2(m2)
{
}

PerlinMaterial::~PerlinMaterial()
{
}

void PerlinMaterial::preprocess(int maxTime)
{
    m1->preprocess(maxTime);
    m2->preprocess(maxTime);
}

Color turbstripe(Point& p, double w, Color c0, Color c1)
{
    double k1 = .01, k2 = 4.;
    double t = (1. + (k1*p.x() + sin(perlinNoise(p*k2))) / w) / 2.;

    return c0*t + c1*(1-t);
}

void PerlinMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    //if( sin((hitpos.x() + hitpos.y() - hitpos.z())* 15) > 0 &&
    //sin((hitpos.x() + hitpos.y() + hitpos.z())* 4  ) > 0)
    Color c0,c1;

    m1->shade(c0, context, ray, hit, atten, depth);
    m2->shade(c1, context, ray, hit, atten, depth);

    result = turbstripe(hitpos, .3, c0, c1);
}
