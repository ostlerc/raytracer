
#include "PinholeCamera.h"
#include "RenderContext.h"
#include "Ray.h"
#include "Math.h"
#include <math.h>
#include <iostream>

PinholeCamera::PinholeCamera(const Animation<Point>& eye, const Animation<Point>& lookat, const Animation<Vector>& up,
                             double hfov)
  : eye(eye), lookat(lookat), up(up), hfov(hfov)
{
}

PinholeCamera::~PinholeCamera()
{
}

void PinholeCamera::preprocess(double aspect_ratio, int maxTime)
{
    eye.preprocess(maxTime);
    lookat.preprocess(maxTime);
    up.preprocess(maxTime);

    for(int i = 0; i <= maxTime; i++)
    {
        Vector l = lookat(i)-eye(i);
        l.normalize();
        lookdir.addFrame(i, l);

        Vector _u(Cross(l, up(i)));
        Vector _v(Cross(_u, l));

        double ulen = tan(hfov/2.*M_PI/180.);
        _u.normalize();
        _u *= ulen;
        double vlen = ulen/aspect_ratio;
        _v.normalize();
        _v *= vlen;

        u.addFrame(i, _u);
        v.addFrame(i, _v);
    }

    u.preprocess(maxTime);
    v.preprocess(maxTime);
    lookdir.preprocess(maxTime);
}

void PinholeCamera::makeRay(Ray& ray, const RenderContext& context, double x, double y) const
{
    double time = context.time();

    Vector direction = lookdir(time) + u(time)*x + v(time)*y;
    direction.normalize();
    ray = Ray(eye(time), direction);
}
