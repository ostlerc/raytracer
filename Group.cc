#include "Group.h"
#include "BoundingBox.h"
#include "RenderContext.h"

#include <iostream>
using namespace std;

Group::Group(bool bottomUp) : _bottomUp(bottomUp)
{
}

Group::~Group()
{
}

void Group::addObject(Object* object)
{
    if(object->isInfinite())
        infiniteObjects.push_back(object);
    else
        objects.push_back(object);
}

void Group::preprocess(int maxTime)
{
    for (Object *o : objects)
        o->preprocess(maxTime);

    for(Object *o : infiniteObjects)
        o->preprocess(maxTime);

    if(objects.size() > 2) //we want to calc a BVH here
    {
        groups.resize(maxTime+1);
        _populateBVH(maxTime);
        objects.clear();
    }

    _bounds.resize(maxTime+1);

    RenderContext context(0, 0);

    for(int time = 0; time <= maxTime; ++time)
    {
        context.setTime(time);
        BoundingBox bbox;
        for(Object *o : objects)
            o->getBounds(bbox, context);

        if(groups.size())
        {
            for(auto o : groups[time])
                o->getBounds(bbox, context);
        }

        _bounds[time] = bbox;
    }
}

void Group::_populateBVH(int maxTime)
{
    if(_bottomUp)
        _bottomUpBVH(maxTime);
    else
        _topDownBVH(maxTime);
}

void Group::_topDownBVH(int maxTime)
{
    RenderContext context(0, 0);

    for(int time = 0; time <= maxTime; time++)
    {
        context.setTime(time);

        _sortObjects(objects, context);
        std::vector<Object*> _groups = _divideObjects(objects, context, maxTime);
        if(_groups.size() > 2 || _groups.size() < 0)
        {
            cerr << "this is an error!!!" << endl;
            exit(1);
        }

        //at this point _groups.count() can only be 1 or 2
        groups[time] = _groups;
    }
}

void Group::_sortObjects(std::vector<Object*>& objs, RenderContext& context)
{
    std::vector<BoundingBoxSorter> boxes;
    for(auto o : objs)
    {
        BoundingBox bb;
        o->getBounds(bb, context);
        boxes.push_back(BoundingBoxSorter(bb, o));
    }

    sort(boxes.begin(), boxes.end());
    objs.clear();

    for(auto b : boxes)
        objs.push_back(b.o);
}

std::vector<Object*> Group::_divideObjects(std::vector<Object*> objectsRemaining, RenderContext& context, int maxTime)
{
    std::vector<Object*> _groups;

    //Top down bvh creation:
    //sort elements and put half in one group and another in a different group

    Group *grp = new Group(objectsRemaining.size() < 5000 || _bottomUp);
    unsigned i;

    {
        for(i = 0; i < objectsRemaining.size() / 2; ++i)
        {
            if(!objectsRemaining[i])
            {
                cerr << "Null object found! at " << i << endl;
                exit(1);
            }

            grp->addObject(objectsRemaining[i]);
        }

        grp->preprocess(maxTime);
        _groups.push_back(grp);
    }

    grp = new Group(objectsRemaining.size() < 5000 || _bottomUp);

    {
        for( ; i < objectsRemaining.size(); ++i)
        {
            if(!objectsRemaining[i])
            {
                cerr << "Null object found! at " << i << endl;
                exit(1);
            }
            grp->addObject(objectsRemaining[i]);
        }

        grp->preprocess(maxTime);
        _groups.push_back(grp);
    }

    return _groups;
}

void Group::_bottomUpBVH(int maxTime)
{
    RenderContext context(0, 0);
    for(int time = 0; time <= maxTime; time++)
    {
        context.setTime(time);

        std::vector<Object*> _groups = _pairObjects(objects, context);

        for(Object *o : _groups)
            o->preprocess(maxTime);

        //once all objects have pairs, pair up the pairs until only two pairs are left
        while(_groups.size() > 2)
        {
            _groups = _pairObjects(_groups, context);

            for(Object *o : _groups)
                o->preprocess(maxTime);
        }

        groups[time] = _groups;
    }
}

std::vector<Object*> Group::_pairObjects(std::vector<Object*> objectsRemaining, RenderContext& context)
{
    std::vector<Object*> _groups;

    //Bottom up bvh creation:
    //find two objects that are the closest, pair them together.
    while(objectsRemaining.size())
    {
        Object* at = objectsRemaining[0];
        Group *pairs = new Group(_bottomUp);

        if(objectsRemaining.size() > 1)
        {
            Object* closestObject = objectsRemaining[1]; //closest to first object in list

            BoundingBox atBox, closestBox;
            at->getBounds(atBox, context);
            closestObject->getBounds(closestBox, context);
            int indexOfClosest = 1;

            if(objectsRemaining.size() > 2) //3 or more, so find closest
            {
                double closestDistance = atBox.distance(closestBox);

                Object*const* begin = &objectsRemaining[2];
                Object*const* end = &objectsRemaining[0]+objectsRemaining.size();
                for (int index = 2; begin != end; ++index)
                {
                    Object* _o = (*begin++);
                    BoundingBox objBox;
                    _o->getBounds(objBox, context);
                    double d = atBox.distance(objBox);
                    if(d < closestDistance)
                    {
                        indexOfClosest = index;
                        closestBox = objBox;
                        closestObject = _o;
                        closestDistance = d;
                    }
                }
            }

            pairs->addObject(closestObject);
            objectsRemaining.erase(objectsRemaining.begin() + indexOfClosest);
        }

        pairs->addObject(at);
        _groups.push_back(pairs);
        objectsRemaining.erase(objectsRemaining.begin());
    }

    return _groups;
}

void Group::getBounds(BoundingBox& bbox, const RenderContext& context) const
{
    bbox.extend(_bounds[context.time()]);
}

void Group::intersect(HitRecord& hit, const RenderContext& context, const Ray& ray) const
{
    if(infiniteObjects.size())
    {
        Object*const* begin = &infiniteObjects[0];
        Object*const* end = &infiniteObjects[0]+infiniteObjects.size();
        while (begin != end)
            (*begin++)->intersect(hit, context, ray);
    }

    //comment out these three lines to disable the bvh optimization
    double tmin, tmax;
    if(!_bounds[context.time()].intersects(ray, tmin, tmax))
        return;

    if(objects.size())
    {
        Object*const* begin = &objects[0];
        Object*const* end = &objects[0]+objects.size();
        while (begin != end)
            (*begin++)->intersect(hit, context, ray);
    }

    if(groups.size())
    {
        Object*const* begin = &groups[context.time()][0];
        Object*const* end = &groups[context.time()][0]+groups[context.time()].size();
        while (begin != end)
            (*begin++)->intersect(hit, context, ray);
    }
}
