#ifndef PolarSphere_h
#define PolarSphere_h

#include "Animation.h"
#include "Point.h"
#include "Sphere.h"
class Ray;

class PolarSphere : public Sphere {
 public:

  PolarSphere(Material* material, Animation<Point>& center, Animation<double> radius, Animation<Vector>& pole, Animation<Vector>& meridian);

  virtual ~PolarSphere() {};

  virtual void computeUVW(Vector& uvw, const RenderContext& context,
          const Ray& ray, const HitRecord& hit) const;

  virtual void preprocess(int maxTime);

 protected:
  Animation<Vector> X;
  Animation<Vector> Y;
  Animation<Vector> Z;
};

#endif
