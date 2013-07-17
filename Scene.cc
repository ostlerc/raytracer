#include "Scene.h"
#include "Background.h"
#include "Camera.h"
#include "HitRecord.h"
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "Ray.h"
#include "RenderContext.h"
#include <float.h>
#include <iostream>
#include <stdlib.h>
#include <future>
#include <omp.h>
using namespace std;

Scene::Scene()
{
  object = 0;
  background = 0;
  camera = 0;
  ambient = Color(0, 0, 0);
  minAttenuation = 0;
}

Scene::~Scene()
{
  delete object;
  delete background;
  delete camera;
  delete image;
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    delete light;
  }
}

void Scene::preprocess(int maxTime)
{
  background->preprocess();
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    light->preprocess();
  }
  double aspect_ratio = image->aspect_ratio();
  camera->preprocess(aspect_ratio, maxTime);
  object->preprocess(maxTime);
}

void Scene::render(int time)
{
  if(!object || !background || !camera || lights.empty()){
    cerr << "Incomplete scene, cannot render!\n";
    exit(1);
  }
  int xres = image->getXresolution();
  int yres = image->getYresolution();
  double dx = 2./xres;
  double xmin = -1. + dx/2.;
  double dy = 2./yres;
  double ymin = -1. + dy/2.;
  Color atten(1,1,1);

#pragma omp parallel for //comment out if your compiler doesn't support OpenMP
  for(int i=0;i<yres;i++){
    //cerr << "y=" << i << '\n';
    const double y = ymin + i*dy;
    for(int j=0;j<xres;j++){
      const double x = xmin + j*dx;

      Ray ray;
      RenderContext context(this, time);
      camera->makeRay(ray, context, x, y);
      HitRecord hit(DBL_MAX);
      object->intersect(hit, context, ray);
      Color result;
      if(hit.getPrimitive()){
          // Ray hit something...
          const Material* matl = hit.getMaterial();
          matl->shade(result, context, ray, hit, atten, 0);
      } else {
          background->getBackgroundColor(result, context, ray);
      }
      image->set(j, i, result);
    }
  }
}

double Scene::traceRay(Color& result, const RenderContext& context, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    object->intersect(hit, context, ray);
    if(hit.getPrimitive()){
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}

double Scene::traceRay(Color& result, const RenderContext& context, const Object* obj, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    obj->intersect(hit, context, ray);
    if(hit.getPrimitive()){
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}
