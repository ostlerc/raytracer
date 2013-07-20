#ifndef RefractionMaterial_h
#define RefractionMaterial_h

#include "Material.h"
#include "Color.h"
#include "Animation.h"

class RefractionMaterial : public Material {
    public:
        RefractionMaterial(Animation<float> eta, Animation<int> exp);
        virtual ~RefractionMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

        virtual void preprocess(int maxTime);

    private:
        RefractionMaterial(const RefractionMaterial&);
        RefractionMaterial& operator=(const RefractionMaterial&);

        Animation<float> eta; //this is the ratio kifrom / kito. ( ie 1. / 1.33 )
        Animation<int> exp; //specular exponent
};

#endif //RefractionMaterial_h
