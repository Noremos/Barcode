#pragma once
#include "point.h"
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

namespace bc
{ 
class EXPORT  Component
{
public:
    pmap coords;
    Component(int len = 10);
    Component(int x, int y);
    bool isContain(int x, int y);
    bool isContain(point p);
    virtual void add(point p);

    virtual	~Component();
};

//typedef std::unordered_map<point, Component*, pointHash> cmap;
typedef Component** cmap;
typedef std::pair<point, Component*> cpair;
}
