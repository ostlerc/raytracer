#include "CheckeredMaterial.h"
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

CheckeredMaterial::CheckeredMaterial(Material* m1, Material* m2)
:m1(m1), m2(m2)
{
}

CheckeredMaterial::~CheckeredMaterial()
{
}

void CheckeredMaterial::preprocess(int maxTime)
{
    m1->preprocess(maxTime);
    m2->preprocess(maxTime);
}

void CheckeredMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
    Vector uvw;
    hit.getPrimitive()->computeUVW(uvw, context, ray, hit);

    if( sin((uvw.x() + uvw.y() + uvw.z())*30) > 0 &&
        sin((uvw.x() + uvw.y() - uvw.z())*30) > 0)
        m1->shade(result, context, ray, hit, atten, depth);
    else
        m2->shade(result, context, ray, hit, atten, depth);
}
