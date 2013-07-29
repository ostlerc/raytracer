#ifndef PerlinMaterial_h
#define PerlinMaterial_h

#include "Material.h"
#include "Color.h"

class PerlinMaterial : public Material {
    public:
        PerlinMaterial(Material* m1, Material* m2);
        virtual ~PerlinMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

        virtual void preprocess(int maxTime);

    private:
        PerlinMaterial(const PerlinMaterial&);
        PerlinMaterial& operator=(const PerlinMaterial&);

        Material *m1;
        Material *m2;
};

#endif
