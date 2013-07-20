#include "Background.h"
#include "HitRecord.h"
#include "Light.h"
#include "Math.h"
#include "Point.h"
#include "Primitive.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Scene.h"
#include "MetalMaterial.h"
#include "Vector.h"

#include <cfloat>
using namespace std;

MetalMaterial::MetalMaterial(Animation<float> color, Animation<int> exp)
:color(color), exp(exp)
{
}

MetalMaterial::~MetalMaterial()
{
}

void MetalMaterial::preprocess(int maxTime)
{
    color.preprocess(maxTime);
    exp.preprocess(maxTime);
}

void MetalMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
    double time = context.time();
    const Scene* scene = context.getScene();
    const vector<Light*>& lights = scene->getLights();
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    Vector normal;
    hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
    double costheta = Dot(normal, ray.direction());
    if(costheta > 0)
        normal = -normal;
    else
        costheta = -costheta;

    const Object* world = scene->getObject();

    Color c(0.0, 0.0, 0.0);

    Light*const* begin = &lights[0];
    Light*const* end = &lights[0]+lights.size();
    while(begin != end){
        Color light_color;
        Vector light_direction;

        double dist = (*begin++)->getLight(light_color, light_direction, context, hitpos);
        double cosphi = Dot(normal, light_direction);
        if(cosphi > 0){
            // Cast shadow rays...
            HitRecord shadowhit(dist);
            Ray shadowray(hitpos, light_direction);
            world->intersect(shadowhit, context, shadowray);

            if(!shadowhit.getPrimitive()) // No shadows...
            {
                Vector h = light_direction - ray.direction(); //halfway vector
                h.normalize();
                double s = pow( Dot(normal, h), exp(time)); //Specular fraction
                c += light_color*s;
            }
        }
    }

    result = c*color(time);

    if(depth < context.getScene()->getMaxRayDepth())
    {
        Vector rDir = ray.direction() + (2.*costheta) * normal;
        rDir.normalize();
        Ray reflect_ray(hitpos, rDir);
        Color rColor;
        scene->traceRay(rColor, context, reflect_ray, atten, depth+1);

        double fresnel_reflect = color(time) + (1 - color(time))*pow(1-costheta,5);
        result += rColor*fresnel_reflect;
    }
}
