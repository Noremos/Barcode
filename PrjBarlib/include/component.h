#pragma once
#include "barline.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
namespace bc
{
    class barcodeCreator;
    class EXPORT  Component
    {
    protected:
        barcodeCreator* factory;
    public:
        pmap* coords;
        uchar start = 0, end = 0;
        //0 - nan
        size_t num = 0;
        //    pmap coords;

        //    cv::Mat binmap;

        Component(int x, int y, barcodeCreator* factory);
        Component(barcodeCreator* factory);
        bool isContain(int x, int y);
        bool isContain(point p);
        virtual void add(const point& p);
        virtual void kill();
        //    void setB(const point &p);
        virtual	~Component();
    };

    //typedef std::unordered_map<point, Component*, pointHash> cmap;
    typedef Component** cmap;
    typedef std::pair<point, Component*> cpair;
}
