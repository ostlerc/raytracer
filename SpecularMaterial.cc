#include "Background.h"
#include "HitRecord.h"
#include "Light.h"
#include "Math.h"
#include "Point.h"
#include "Primitive.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Scene.h"
#include "SpecularMaterial.h"
#include "Vector.h"

#include <cfloat>
using namespace std;

SpecularMaterial::SpecularMaterial(const Color& color, Animation<Color> highlight, Animation<float> Kd, Animation<float> Ka, Animation<float> Kr, Animation<int> exp)
:color(color), highlight(highlight), Kd(Kd), Ka(Ka), Kr(Kr), exp(exp)
{
}

SpecularMaterial::~SpecularMaterial()
{
}

void SpecularMaterial::preprocess(int maxTime)
{
    highlight.preprocess(maxTime);
    Kd.preprocess(maxTime);
    Ka.preprocess(maxTime);
    Kr.preprocess(maxTime);
    exp.preprocess(maxTime);
}

void SpecularMaterial::shade(Color& result, const RenderContext& context,
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

    const Object* world = scene->getObject();

    Color light = scene->getAmbient()*Ka(time);
    Color specular(0,0,0);

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

                light += light_color*(Kd(time)*cosphi);
                double s = pow( Dot(normal, h), exp(time)); //Specular fraction
                specular += light_color*s;
            }
        }
    }

    result = specular*highlight(time) + light*color;

    if(depth < context.getScene()->getMaxRayDepth() && Kr(time) > 0.)
    {
        double reflet = 2. * Dot(ray.direction(), normal);
        Vector rDir = ray.direction() - reflet * normal;
        rDir.normalize();
        Ray reflection(hitpos, rDir);
        Color rcolor;
        scene->traceRay(rcolor, context, reflection, atten, depth+1);

        result += rcolor * Kr(time);
    }
}
