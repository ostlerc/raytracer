
#include "Primitive.h"
#include "HitRecord.h"
#include "Material.h"
#include "Ray.h"
#include "Vector.h"

#include <iostream>

Primitive::Primitive(Material* matl)
  : matl(matl)
{
}

Primitive::~Primitive()
{
}

void Primitive::preprocess(int maxTime)
{
  matl->preprocess(maxTime);
}

void Primitive::computeUVW(Vector& uvw, const RenderContext& context,
                           const Ray& ray, const HitRecord& hit) const
{
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  uvw = Vector(hitpos);
}
