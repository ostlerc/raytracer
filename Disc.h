#ifndef Disc_h
#define Disc_h

#include "Vector.h"
#include "Primitive.h"

class HitRecord;
class Point;
class RenderContext;

class Disc : public Primitive {
    public:
        Disc(Material* material, const Vector& n, double r, const Point& q);
        virtual ~Disc();

        virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
        virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
        virtual void normal(Vector& normal, const RenderContext& context,
                const Point & hitpos, const Ray& ray, const HitRecord& hit) const;

    private:
        Disc(const Disc&);
        Disc& operator=(const Disc&);

        Vector n; //normal
        double r; //radius
        Point q; //center
        double d;
};

#endif //Disc_h
