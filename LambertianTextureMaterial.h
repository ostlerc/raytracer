#ifndef LambertianTextureMaterial_h
#define LambertianTextureMaterial_h

#include "Material.h"
#include "Color.h"
#include "Image.h"

class LambertianTextureMaterial : public Material {
 public:
  LambertianTextureMaterial(const std::string& file, float Kd, float Ka);
  virtual ~LambertianTextureMaterial();

  virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, const Color& atten, int depth) const;

 private:
  LambertianTextureMaterial(const LambertianTextureMaterial&);
  LambertianTextureMaterial& operator=(const LambertianTextureMaterial&);

  Image texture;
  float Kd; //diffuse
  float Ka; //ambient
};

#endif
