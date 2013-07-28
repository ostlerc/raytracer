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
#include <random>
#include <time.h>
using namespace std;

typedef std::mt19937 MyRNG;  // the Mersenne Twister with a popular choice of parameters
uint32_t seed_val;           // populate somehow

MyRNG rng;                   // e.g. keep one global instance (per thread)

Scene::Scene()
{
  object = 0;
  background = 0;
  camera = 0;
  ambient = Color(0, 0, 0);
  minAttenuation = 0;
  maxRayDepth = 0;
  sampleSize = 2;
  rng.seed(time(NULL));
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
    const int xres = image->getXresolution();
    const int yres = image->getYresolution();
    const double dx = 2./xres;
    const double xmin = -1. + dx/2.;
    const double dy = 2./yres;
    const double ymin = -1. + dy/2.;
    const int jitter_size = getSampleSize();
    const double jdx = 2./(xres*jitter_size);
    const double jdy = 2./(yres*jitter_size);
    //const double jxmin = -1 + jdx/2.;
    //const double jymin = -1 + jdy/2.;
    std::uniform_int_distribution<uint32_t> uint_dist10(0,100); // range [0,100]
    const double inv_sample_per_pixel = 1. / (jitter_size*jitter_size);

#pragma omp parallel for //comment out if your compiler doesn't support OpenMP
    for(int i=0;i<yres;i++) {
        //const double y = ymin + i*dy;
        for(int j=0;j<xres;j++) {
            //const double x = xmin + j*dx;
            Color colors[jitter_size][jitter_size];

            for(int ji=0; ji<jitter_size; ji++) {
                for(int jj=0; jj<jitter_size; jj++) {
                    const double random_y = uint_dist10(rng);
                    const double random_x = uint_dist10(rng);
                    const double jy = ymin + jdy*(ji-1) + i*dy + ( jdy * random_y/100.);
                    const double jx = xmin + jdx*(jj-1) + j*dx + ( jdx * random_x/100.);;

                    Ray ray;
                    Color atten(1,1,1);
                    RenderContext context(this, time);
                    camera->makeRay(ray, context, jx, jy);
                    HitRecord hit(DBL_MAX);
                    object->intersect(hit, context, ray);
                    if(hit.getPrimitive()){
                        // Ray hit something...
                        const Material* matl = hit.getMaterial();
                        matl->shade(colors[ji][jj], context, ray, hit, atten, 0);
                    } else {
                        background->getBackgroundColor(colors[ji][jj], context, ray);
                    }
                }
            }

            Color aggregate(0,0,0);
            for(int y = 0; y < jitter_size; y++)
                for(int x = 0; x < jitter_size; x++)
                    aggregate += colors[y][x];

            aggregate = aggregate * inv_sample_per_pixel;
            image->set(j, i, aggregate);
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
