#ifndef MetalMaterial_h
#define MetalMaterial_h

#include "Material.h"
#include "Color.h"
#include "Animation.h"

class MetalMaterial : public Material {
    public:
        MetalMaterial(Animation<float> color, Animation<int> exp);
        virtual ~MetalMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

        virtual void preprocess(int maxTime);

    private:
        MetalMaterial(const MetalMaterial&);
        MetalMaterial& operator=(const MetalMaterial&);

        Animation<float> color;
        Animation<int> exp; //specular exponent
};

#endif //MetalMaterial_h
