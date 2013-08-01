
#ifndef BoundingBox_h
#define BoundingBox_h

#include "Point.h"
#include "Ray.h"
#include "Box.h"

class BoundingBox {
 public:
  BoundingBox();
  ~BoundingBox();

  void reset();
  void extend(const Point&);
  void extend(const BoundingBox&);

  Point getMin() const {
    return min;
  }
  Point getMax() const {
    return max;
  }

  bool intersects(const Ray& ray, double& tmin, double& tmax) const;

  double distance(BoundingBox& bb);
  double distance(double p1, double p2);

  bool operator <(const BoundingBox& other) const;

 private:
  Point min, max;
};

#endif
