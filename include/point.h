#pragma once

#include "presets.h"
INCLUDE_CV

#include <unordered_map>


namespace bc {


struct EXPORT point
{
    int x = -1;
    int y = -1;
    point();
    point(int x, int y);
    void init(int x, int y);
    void init(int p[2]);

#ifdef USE_OPENCV
    inline cv::Point bc::point::cvPoint()
    {
        return cv::Point(x, y);
    }
#endif // USE_OPENCV


    point operator+(int* xy)
    {
        return point(x + xy[0], y + xy[1]);
    }
    point operator+(char* xy)
    {
        return point(x + xy[0], y + xy[1]);
    }
    point operator*(int c)
    {
        return point(x * c, y * c);
    }
    point operator+(point p)
    {
        return point(x + p.x, y + p.y);
    }
    int operator[](int i)
    {
        return i == 0 ? x : y;
    }
    bool operator==(const point& p) const
    {
        return x == p.x && y == p.y;
    }
    bool operator!=(const point& p) const
    {
        return x != p.x || y != p.y;
    }
};
class EXPORT pointHash
{
public:
    size_t operator()(const point& p) const
    {
        return ((size_t)p.x << 32) + (size_t)p.y;
    }
};

}
//typedef std::unordered_map<bc::point, uchar, bc::pointHash> pmap;
template<class T>
using ppair = std::pair<bc::point, T>;

template<class T>
using pmap = std::vector<ppair<T>>;

typedef std::vector<uint32_t> barcounter;
