#include "Background.h"
#include "Background.h"
#include "HitRecord.h"
#include "Light.h"
#include "Math.h"
#include "Point.h"
#include "Primitive.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Scene.h"
#include "RefractionMaterial.h"
#include "Vector.h"

using namespace std;

RefractionMaterial::RefractionMaterial(Animation<float> eta, Animation<int> exponent, Animation<Color> atten)
:eta(eta), exponent(exponent), atten(atten)
{
}

RefractionMaterial::~RefractionMaterial()
{
}

void RefractionMaterial::preprocess(int maxTime)
{
    eta.preprocess(maxTime);
    exponent.preprocess(maxTime);
    atten.preprocess(maxTime);

    if(co.empty())
    {
        for(int t = 0; t <= maxTime; ++t)
            co.push_back( Color(log(atten(t).r()), log(atten(t).g()), log(atten(t).b())) );
    }
}

void RefractionMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
    double time = context.time();
    const Scene* scene = context.getScene();
    const Object* world = scene->getObject();
    const vector<Light*>& lights = scene->getLights();
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    Vector normal, unflipped_normal;
    hit.getPrimitive()->normal(unflipped_normal, context, hitpos, ray, hit);
    double costheta = Dot(unflipped_normal, ray.direction());

    bool inside = false;

    if(costheta > 0) //this is if we are exiting the object or not
    {
        normal = -unflipped_normal;
        inside = true;
    }
    else
    {
        costheta = -costheta;
        normal = unflipped_normal;
    }

    Color c(0.0, 0.0, 0.0);

    Light*const* begin = &lights[0];
    Light*const* end = &lights[0]+lights.size();
    while(begin != end){
        Color light_color;
        Vector light_direction;

        double dist = (*begin++)->getLight(light_color, light_direction, context, hitpos);
        double cosphi = Dot(unflipped_normal, light_direction);
        if(cosphi > 0){
            // Cast shadow rays...
            HitRecord shadowhit(dist);
            Ray shadowray(hitpos, light_direction);
            world->intersect(shadowhit, context, shadowray);
            if(!shadowhit.getPrimitive()) // No shadows...
            {
                Vector h = inside
                  ? light_direction + ray.direction()
                  : light_direction - ray.direction(); //halfway vector

                h.normalize();
                double s = pow( Dot(unflipped_normal, h), exponent(time)); //Specular fraction
                c += light_color*s;
            }
        }
    }

    result = c;

    if(depth < scene->getMaxRayDepth())
    {
        if(eta.isEmpty())
            cerr << "uh oh" << endl;
        double _eta = inside ? 1. / eta(time) : eta(time);

        Vector rDir = ray.direction() + 2.*costheta * normal;
        rDir.normalize();
        Ray reflectionRay(hitpos, rDir);
        Color reflect_color;

        double costheta2sqrd = 1.0 + (costheta*costheta - 1.0)/(_eta*_eta);

        if (costheta2sqrd < 0.) //total internal reflection
        {
            double t = scene->traceRay(reflect_color, context, reflectionRay, atten, depth+1);
            //result += reflect_color;
            result += reflect_color*beersAttenuation(t, context.time());
        }
        else
        {
            double costheta2 = sqrt(costheta2sqrd);
            double Ro = (_eta - 1.)/(_eta + 1.);
            Ro *= Ro;

            double fresnel_reflect= Ro + (1. - Ro)*pow(1.-Min(costheta, costheta2),5);
            double fresnel_refract = 1. - fresnel_reflect;

            double eta_inverse = 1.0/_eta;
            Vector refract_dir = ray.direction()*eta_inverse + (costheta*eta_inverse - costheta2)*normal;
            //refract_dir.normalize(); //should already be normalized
            Ray refractionRay(hitpos, refract_dir);

            Color refraction_color;
            double t  = scene->traceRay(reflect_color, context, reflectionRay, atten*fresnel_reflect, depth+1);
            double _t = scene->traceRay(refraction_color, context, refractionRay, atten*fresnel_refract, depth+1);


            if(inside)
            {
                // reflect back inside
                result += reflect_color*beersAttenuation(t, context.time())*fresnel_reflect;
                // transmit to the outside
                result += refraction_color*fresnel_refract;
            }
            else
            {
                // reflect back outside
                result += reflect_color*fresnel_reflect;
                // transmit goes inside
                result += refraction_color*beersAttenuation(_t, context.time())*fresnel_refract;
            }
            //result += reflect_color*fresnel_reflect + refraction_color*fresnel_refract;
        }
    }
}

Color RefractionMaterial::beersAttenuation(double t, int time) const
{
    return Color(
            exp( t*co[time].r() ),
            exp( t*co[time].g() ),
            exp( t*co[time].b() ) );
}
