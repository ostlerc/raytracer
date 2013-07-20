#ifndef SpecularMaterial_h
#define SpecularMaterial_h

#include "Material.h"
#include "Color.h"
#include "Animation.h"

class SpecularMaterial : public Material {
    public:
        SpecularMaterial(const Color& color, Animation<float> Kd, Animation<float> Ka, Animation<float> Ks, Animation<float> Kr, Animation<float> exp);
        virtual ~SpecularMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

        virtual void preprocess(int maxTime);

    private:
        SpecularMaterial(const SpecularMaterial&);
        SpecularMaterial& operator=(const SpecularMaterial&);

        Color color;
        Animation<float> Kd; //diffuse
        Animation<float> Ka; //ambient
        Animation<float> Ks; //specular
        Animation<float> Kr; //reflection
        Animation<float> exp; //specular exponent
};

#endif //SpecularMaterial_h
