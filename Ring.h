#ifndef Ring_h
#define Ring_h

#include "Vector.h"
#include "Primitive.h"

class HitRecord;
class Point;
class RenderContext;

class Ring : public Primitive {
    public:
        Ring(Material* material, const Vector& n, double r, double r2, const Point& q);
        virtual ~Ring();

        virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
        virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
        virtual void normal(Vector& normal, const RenderContext& context,
                const Point & hitpos, const Ray& ray, const HitRecord& hit) const;

    private:
        Ring(const Ring&);
        Ring& operator=(const Ring&);

        Vector n; //normal
        double r; //radius
        double r2; //radius 2
        Point q; //center
        double d;
};

#endif //Ring_h
