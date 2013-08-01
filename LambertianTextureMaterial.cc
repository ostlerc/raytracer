#include "LambertianTextureMaterial.h"
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

LambertianTextureMaterial::LambertianTextureMaterial(const std::string& file, float Kd, float Ka)
:texture(file), Kd(Kd), Ka(Ka)
{
}

LambertianTextureMaterial::~LambertianTextureMaterial()
{
}

void LambertianTextureMaterial::shade(Color& result, const RenderContext& context,
        const Ray& ray, const HitRecord& hit, const Color&, int) const
{
    Vector uvw;
    hit.getPrimitive()->computeUVW(uvw, context, ray, hit);
    Color color = texture.interpolate(uvw.x(), uvw.y());

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
            if(!shadowhit.getPrimitive())
                // No shadows...
                light += light_color*(Kd*cosphi);
        }
    }
    result = light*color;
}
