
#ifndef Ray_h
#define Ray_h

#include "Point.h"
#include "Vector.h"

class Ray {
 public:
  Ray() {
  }
  Ray(const Point& origin, const Vector& direction)
    : org(origin), dir(direction), inv_dir(dir.inverse()) {
  }

  ~Ray() {
  }
  Ray(const Ray& copy)
    : org(copy.org), dir(copy.dir), inv_dir(copy.inv_dir) {
  }
  Ray& operator=(const Ray& copy) {
    org = copy.org;
    dir = copy.dir;
    inv_dir = copy.inv_dir;
    return *this;
  }

  const Point& origin() const {
    return org;
  }
  const Vector& direction() const {
    return dir;
  }
  const Vector& inv_direction() const {
      return inv_dir;
  }
 private:
   Point org;
   Vector dir;
   Vector inv_dir;
};

#endif
