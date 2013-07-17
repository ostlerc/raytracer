#ifndef Triangle_h
#define Triangle_h

#include "Primitive.h"
#include "Point.h"
#include "Animation.h"

class Ray;

class Triangle : public Primitive {
    public:
        Triangle(Material* material,
                Animation<Point>& p0,
                Animation<Point>& p1,
                Animation<Point>& p2,
                Animation<Vector>& n);
        virtual ~Triangle();

        virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
        virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
        virtual void normal(Vector& normal, const RenderContext& context,
                const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
        virtual void preprocess(int maxTime);
    protected:
        Animation<Point> p0, p1, p2;
        Animation<Vector> n;
};

#endif //Triangle_h
