#ifndef RenderContext_h
#define RenderContext_h

#define MAX_DEPTH (5)

class Scene;

class RenderContext {
 public:
  RenderContext(const Scene* scene, int time)
    : scene(scene)
    , _time(time) {}

  ~RenderContext() {}

  const Scene* getScene() const { return scene; }

  void setTime(double time) { _time = time; }
  double time() const { return _time; }

 private:
  const Scene* scene;
  double _time;
};

#endif
