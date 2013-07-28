
#ifndef Group_h
#define Group_h

#include "Object.h"
#include "BoundingBox.h"
#include <vector>

class Group : public Object {
 public:
  Group();
  virtual ~Group();

  void addObject(Object* object);
  virtual void preprocess(int);
  virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;

 protected:
  std::vector<Object*> objects;
  std::vector<Object*> infiniteObjects;

  //This holds the recursive bvh for all objects this item holds
  std::vector<std::vector<Object*>> groups;

 private:
  void _populateBVH(int maxTime);
  std::vector<Object*> _pairObjects(std::vector<Object*> objectsRemaining, RenderContext&);
  std::vector<BoundingBox> _bounds;

 private:
  Group(const Group&);
  Group& operator=(const Group&);
  BoundingBox bb1, bb2;
};

#endif

