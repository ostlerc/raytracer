#ifndef SpecularMaterial_h
#define SpecularMaterial_h

#include "Material.h"
#include "Color.h"

class SpecularMaterial : public Material {
    public:
        SpecularMaterial(const Color& color, float Kd, float Ka, float Ks, float Kr);
        virtual ~SpecularMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

    private:
        SpecularMaterial(const SpecularMaterial&);
        SpecularMaterial& operator=(const SpecularMaterial&);

        Color color;
        float Kd; //diffuse
        float Ka; //ambient
        float Ks; //specular
        float Kr; //reflection
        float exp; //specular exponent
};

#endif //SpecularMaterial_h
