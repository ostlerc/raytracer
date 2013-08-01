#ifndef Group_h
#define Group_h

#include "Object.h"
#include "BoundingBox.h"
#include <vector>

class Group : public Object {
    public:
        Group(bool);
        virtual ~Group();

        void addObject(Object* object);
        virtual void preprocess(int);
        virtual void getBounds(BoundingBox& bbox, const RenderContext&) const;
        virtual void intersect(HitRecord& hit, const RenderContext&, const Ray& ray) const;

    protected:
        std::vector<Object*> objects;
        std::vector<Object*> infiniteObjects;

        //This holds the recursive bvh for all objects this item holds
        std::vector<std::vector<Object*>> groups;

    private:
        void _populateBVH(int maxTime);

        //top down methods
        void _topDownBVH(int maxTime);
        void _sortObjects(std::vector<Object*>& objs, RenderContext&);
        std::vector<Object*> _divideObjects(std::vector<Object*>, RenderContext&, int maxTime);

        //bottom up methods
        void _bottomUpBVH(int maxTime);
        std::vector<Object*> _pairObjects(std::vector<Object*> objectsRemaining, RenderContext&);

    private:
        Group(const Group&);
        Group& operator=(const Group&);
        std::vector<BoundingBox> _bounds;
        bool _bottomUp;
};

class BoundingBoxSorter {
    public:
        BoundingBoxSorter(BoundingBox _bb, Object *_o) : bb(_bb), o(_o) {}
        bool operator <(const BoundingBoxSorter & other) const
        {
            return bb < other.bb;
        }

        BoundingBox bb;
        Object *o;
};

#endif

