#ifndef Box_h
#define Box_h

#include "Primitive.h"
#include "Animation.h"
#include "Point.h"
#include <map>
class Ray;

class Box : public Primitive {
    public:
        Box(Material* material,
                Animation<Point>& min,
                Animation<Point>& max);
        virtual ~Box();

        virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
        virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const;
        virtual void normal(Vector& normal, const RenderContext& context,
                const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
        virtual void preprocess(int maxTime);
    protected:
        Animation<Point> min;
        Animation<Point> max;

    private:
        struct PointPair
        {
            PointPair() {}
            PointPair(const Point& a, const Point& b)
            {
                pair[0] = a;
                pair[1] = b;
            }

            const Point& operator[](int i) const
            {
                return pair[i];
            }

            /*Point operator[](int i) const
            {
                return pair[i];
            }*/

            private:
                Point pair[2];
        };

        std::vector<PointPair> _pairs;
        Vector n;
};

#endif
