#ifndef Triangle_h
#define Triangle_h

#include "Primitive.h"
#include "Point.h"
class Ray;

class Triangle : public Primitive {
    public:
        Triangle(Material* material, const Point& p0, const Point& p1, const Point& p2, const Vector& n);
        virtual ~Triangle();

        virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
        virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
        virtual void normal(Vector& normal, const RenderContext& context,
                const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
    protected:
        Point p0, p1, p2;
        Vector n;
};

#endif //Triangle_h
