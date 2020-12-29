#pragma once
#include <unordered_map>
#include <opencv2/opencv.hpp>

#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif _STL
#  define EXPORT __declspec(dllimport)
#else
# define EXPORT
#endif

#ifdef _PYD
#include <boost/python.hpp>
#include "boost/python/numpy.hpp"
using namespace boost::python;
namespace bp = boost::python;
namespace bn = boost::python::numpy;
#endif // _PYD
namespace bc {


struct EXPORT point
{
    int x = -1;
    int y = -1;
    point();
    point(int x, int y);
    void init(int x, int y);
    void init(int p[2]);
    cv::Point cvPoint();


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
typedef std::pair<bc::point, float> ppair;
typedef std::vector<ppair> pmap;
