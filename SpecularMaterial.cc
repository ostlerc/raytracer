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

SpecularMaterial::SpecularMaterial(const Color& color, Animation<float> Kd, Animation<float> Ka, Animation<float> Ks, Animation<float> Kr, Animation<float> exp)
:color(color), Kd(Kd), Ka(Ka), Ks(Ks), Kr(Kr), exp(exp)
{
}

SpecularMaterial::~SpecularMaterial()
{
}

void SpecularMaterial::preprocess(int maxTime)
{
    Kd.preprocess(maxTime);
    Ka.preprocess(maxTime);
    Ks.preprocess(maxTime);
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
    Color rcolor;
    double i_Kr = 1. - Kr(time);

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

            Vector T = ray.origin() - hitpos; //Points from hitpos towards eye
            Vector H1 = T + light_direction; //halfway vector

            T.normalize();
            H1.normalize();

            double s = pow( Clamp(Dot(normal, H1), 0.0, 1.0), exp(time)); //Specular fraction

            if(!shadowhit.getPrimitive()) // No shadows...
                light += light_color*((Kd(time)*cosphi) + (Ks(time)*s));
        }
    }

    if(depth < context.getScene()->getMaxRayDepth() && Kr(time) > 0.)
    {
        double reflet = 2. * Dot(ray.direction(), normal);
        Vector rDir = ray.direction() - reflet * normal;
        rDir.normalize();
        Ray reflection(hitpos, rDir);
        HitRecord rhit(DBL_MAX);
        context.getScene()->getObject()->intersect(rhit, context, reflection);
        if(rhit.getPrimitive()){
            // Ray hit something...
            const Material* matl = rhit.getMaterial();
            matl->shade(rcolor, context, reflection, rhit, atten, depth+1);
        } else {
            context.getScene()->getBackground()->getBackgroundColor(rcolor, context, reflection);
        }

        result = light*color*i_Kr + rcolor*Kr(time);
    }
    else
        result = light*color;

}
