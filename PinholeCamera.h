
#ifndef PinholeCamera_h
#define PinholeCamera_h

#include "Camera.h"
#include "Point.h"
#include "Vector.h"
#include "Animation.h"

class PinholeCamera : public Camera {
 public:
  PinholeCamera(const Animation<Point>& eye, const Animation<Point>& lookat, const Animation<Vector>& up, double hfov);
  virtual ~PinholeCamera();

  virtual void preprocess(double aspect_ratio, int maxTime);
  virtual void makeRay(Ray& ray, const RenderContext& context, double x, double y) const;

 private:
  PinholeCamera(const PinholeCamera&);
  PinholeCamera& operator=(const PinholeCamera&);

  Animation<Point> eye;
  Animation<Point> lookat;
  Animation<Vector> up;
  double hfov;

  Animation<Vector> u;
  Animation<Vector> v;
  Animation<Vector> lookdir;
};

#endif

