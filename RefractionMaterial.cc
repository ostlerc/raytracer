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

#include <iostream>
#include <cfloat>

using namespace std;

RefractionMaterial::RefractionMaterial(const Color& color, float Kd, float Ka, float Ks, float Kr, float exp, float Krefr, float refr_index)
:color(color), Kd(Kd), Ka(Ka), Ks(Ks), Kr(Kr), exp(exp), Krefr(Krefr), refr_index(refr_index)
{
}

RefractionMaterial::~RefractionMaterial()
{
}

void RefractionMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
    const Scene* scene = context.getScene();
    const vector<Light*>& lights = scene->getLights();
    Point hitpos = ray.origin()+ray.direction()*hit.minT();
    Vector normal;
    hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
    double costheta = Dot(normal, ray.direction());
    if(costheta > 0)
        normal = -normal;

    const Object* world = scene->getObject();

    Color light = scene->getAmbient()*Ka;
    Color rColor;
    Color refr_color;

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

            double s = pow( Clamp(Dot(normal, H1), 0.0, 1.0), exp); //Specular fraction

            if(!shadowhit.getPrimitive()) // No shadows...
                light += light_color*((Ks*s));
        }
    }

    if(depth < context.getScene()->getMaxRayDepth())
    {
        float ndoti, two_ndoti, ndoti2, a,b,b2,D2;
        Vector T;
        ndoti = Dot(normal,ray.direction());
        ndoti2 = ndoti*ndoti;                    // 1 mul
        if (ndoti>=0.0) { b=1.; b2=refr_index;} else {b=1.;b2=1./refr_index;}
        D2 = 1.0f - b2*(1.0f - ndoti2);

        if (D2>=0.0f) {
            if (ndoti >= 0.0f)
                a = b * ndoti - sqrtf(D2); // 2 mul, 3 add, 1 sqrt
            else
                a = b * ndoti + sqrtf(D2);
            T = a*normal - b*ray.direction();     // 6 mul, 3 add
        } else {
            // total internal reflection
            // this usually doesn't happen, so I don't count it.
            two_ndoti = ndoti + ndoti;         // +1 add
            T = two_ndoti * normal - ray.direction();
        }
        T.normalize();
        Ray refraction(hitpos, T);
        //double t =
        scene->traceRay(refr_color, context, refraction, atten, depth+1);

        result = refr_color*Krefr;
    }
    else
        result = light*color;
    /*if(depth < context.getScene()->getMaxRayDepth())
    {
        // calculate refraction
        if (Krefr > 0.)
        {
            double n = 1.0 / refr_index;
            if(costheta > 0) //we are leaving the object, so inverse the refraction index
            {
                n = 1. / n;
            }
            const double cosI = Dot(normal, ray.direction());
            const double sinT2 = n * n * (1.0 - cosI * cosI);

            if (sinT2 > 1.0)
            {
                //total internal reflection
                //cerr << "invalid refraction" << endl;

                if(Kr > 0.) //reflection
                {
                    double reflet = 2. * Dot(ray.direction(), normal);
                    Vector rDir = ray.direction() - reflet * normal;
                    rDir.normalize();
                    Ray reflection(hitpos, rDir);
                    //double t =
                    scene->traceRay(rColor, context, reflection, atten, depth+1);
                }
            }
            else
            {
                Vector refract =  n * ray.direction() - (n + sqrt(1.0 - sinT2)) * normal;
                refract.normalize();
                Ray refractionRay(hitpos, refract);
                scene->traceRay(refr_color, context, refractionRay, atten, depth+1);
            }
        }

        result = light*color*0.1 + rColor*Kr + refr_color*Krefr;
    }
    else*/
        //result = light*color;
}
