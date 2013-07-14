#include "SpecularMaterial.h"
#include "HitRecord.h"
#include "Light.h"
#include "Point.h"
#include "Primitive.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Scene.h"
#include "Vector.h"
#include "Math.h"
using namespace std;

    SpecularMaterial::SpecularMaterial(const Color& color, float Kd, float Ka, float Ks)
:color(color), Kd(Kd), Ka(Ka), Ks(Ks)
{
}

SpecularMaterial::~SpecularMaterial()
{
}

void SpecularMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color&, int) const
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

            double s = pow( Clamp(Dot(normal, H1), 0.0, 1.0), 50.0); //Specular fraction
            if(!shadowhit.getPrimitive())
                // No shadows...
                light += light_color*((Kd*cosphi) + (Ks*s));
        }


    }
    result = light*color;
}
