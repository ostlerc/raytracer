#ifndef RefractionMaterial_h
#define RefractionMaterial_h

#include "Material.h"
#include "Color.h"

class RefractionMaterial : public Material {
    public:
        RefractionMaterial(const Color& color, float Kd, float Ka, float Ks, float Kr, float exp, float Krefr, float refr_index);
        virtual ~RefractionMaterial();

        virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                const HitRecord& hit, const Color& atten, int depth) const;

    private:
        RefractionMaterial(const RefractionMaterial&);
        RefractionMaterial& operator=(const RefractionMaterial&);

        Color color;
        float Kd; //diffuse
        float Ka; //ambient
        float Ks; //specular
        float Kr; //reflection
        float exp;
        float Krefr;
        float refr_index; //refraction index
};

#endif //RefractionMaterial_h
