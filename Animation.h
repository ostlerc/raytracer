#ifndef Animation_h
#define Animation_h

#include <vector>
#include <algorithm>
#include <iostream>

template<typename ValueType>
class Animation
{
public:
    Animation():map()
    {
    }
    Animation(ValueType val)
        :map()
    {
        map.push_back(entry(0,val));
    }
    ValueType operator()(double t) const
    {
        return values[t];
    }
    void addFrame(double t, ValueType v)
    {
        map.push_back(entry(t,v));
    }

    void preprocess(double maxTime)
    {
        if(maxTime <= 0) exit(1);

        for(int i = 0; i <= maxTime; i++)
        {
            auto x = std::find(map.begin(), map.end(), (double)i);
            if(x != map.end())
                values.push_back((*x).value);
            else
                values.push_back(interpolatedValue(i));
        }
    }

    ValueType interpolatedValue(double t)
    {
        auto a = justLess(t);
        if(a.key == t) return a.value;
        auto b = justMore(t);
        if(b.key == t) return b.value;
        if((b.key - a.key)==0) return a.value;
        auto weight = (t - a.key)/(b.key - a.key);
        return a.value * (1-weight) + b.value * weight;
    }

    bool isEmpty() { return map.empty(); }

private:
    struct entry
    {
        double key;
        ValueType value;
        entry(double key, ValueType value):key(key),value(value){}
        bool operator<(entry e) const
        {
            return key < e.key;
        }
        bool operator==(double _key) const
        {
            return key == _key;
        }
    };
    entry justLess(double t) const
    {
        std::vector<entry> less;
        std::copy_if(
             map.begin()
            ,map.end()
            ,std::back_inserter(less)
            ,[=](entry e){return e.key < t;});
        std::sort(less.begin(),less.end());
        if(!less.empty()) return less.back();
        std::sort(map.begin(),map.end());
        return map.front();
    }
    entry justMore(double t) const
    {
        std::vector<entry> more;
        std::copy_if(
                map.begin()
            ,map.end()
            ,std::back_inserter(more)
            ,[=](entry e){return e.key > t;});
        std::sort(more.begin(),more.end());
        if(!more.empty()) return more.front();
        std::sort(map.begin(),map.end());
        return map.back();
    }
    mutable std::vector<entry> map;
    std::vector<ValueType> values;
};

#endif //Animation_h
