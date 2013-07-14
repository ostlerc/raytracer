
#ifndef RenderContext_h
#define RenderContext_h

#define MAX_DEPTH (5)

class Scene;

class RenderContext {
 public:
  RenderContext(const Scene* scene, int time)
    : scene(scene)
    , rayDepth(0)
    , _time(time) {}

  ~RenderContext() {}

  const Scene* getScene() const { return scene; }

  bool next() {
      return rayDepth++ < MAX_DEPTH;
  }

  void setTime(double time) { _time = time; }
  double time() const { return _time; }

 private:
  const Scene* scene;
  int rayDepth;
  double _time;
};

#endif
