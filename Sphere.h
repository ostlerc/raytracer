#ifndef Sphere_h
#define Sphere_h

#include "Primitive.h"
#include "Animation.h"
#include "Point.h"
#include <map>
class Ray;

class Sphere : public Primitive {
 public:
  Sphere(Material* material,
          Animation<Point>& center,
          Animation<double> radius);
  virtual ~Sphere() {};

  virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
  virtual void normal(Vector& normal, const RenderContext& context,
                      const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
  virtual void preprocess(int maxTime);
 protected:
  Animation<Point> center;
  Animation<double> radius;
  Animation<double> inv_radius;
};

#endif
