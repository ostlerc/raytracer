#ifndef CheckeredMaterial_h
#define CheckeredMaterial_h

#include "Material.h"
#include "Color.h"

class CheckeredMaterial : public Material {
    public:
        CheckeredMaterial(Material* m1, Material* m2);
        virtual ~CheckeredMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

        virtual void preprocess(int maxTime);

    private:
        CheckeredMaterial(const CheckeredMaterial&);
        CheckeredMaterial& operator=(const CheckeredMaterial&);

        Material *m1;
        Material *m2;
};

#endif
