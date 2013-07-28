#ifndef RefractionMaterial_h
#define RefractionMaterial_h

#include "Material.h"
#include "Color.h"
#include "Animation.h"

class RefractionMaterial : public Material {
    public:
        RefractionMaterial(Animation<float> eta, Animation<int> exponent, Animation<Color> atten);
        virtual ~RefractionMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

        virtual void preprocess(int maxTime);

    private:
        RefractionMaterial(const RefractionMaterial&);
        RefractionMaterial& operator=(const RefractionMaterial&);
        Color beersAttenuation(double t, int time) const;

        Animation<float> eta; //this is the ratio kifrom / kito. ( ie 1. / 1.33 )
        Animation<int> exponent; //specular exponent

        Animation<Color> atten; //attenuation of this object
        std::vector<Color> co; //coefficient (beers attenuation)
};

#endif //RefractionMaterial_h
