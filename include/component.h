#pragma once
#include "point.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
namespace bc
{
class barcodeCreator;
class EXPORT Component
{
    Component *cachedMaxParent = nullptr;
	size_t totalCount = 0;
protected:
    barcodeCreator *factory;
    bool lived = true;
public:
	bool isAlive()
    {
		return lived;
	}

	Component *parent = nullptr;
	pmap *coords;
	//pmap subCoords;
	float start=0, end=0;
    //0 - nan
    size_t num = 0;
    //    pmap coords;

    //    cv::Mat binmap;
    Component *getMaxParrent()
    {
		if (cachedMaxParent == nullptr || parent == nullptr)
        {
            if (parent == nullptr)
				return this;
            cachedMaxParent = parent;
        }
        while (cachedMaxParent->parent)
        {
            cachedMaxParent = cachedMaxParent->parent;
			totalCount += cachedMaxParent->coords->size();
        }
        return cachedMaxParent;
    }
	Component(point pix, barcodeCreator *factory);
	Component(barcodeCreator *factory, bool create = false);
    bool isContain(int x, int y);
    bool isContain(point p);
    virtual void add(const point &p);
    virtual void kill();
	virtual void setParrent(Component *parnt);
//    void setB(const point &p);
    virtual ~Component();
};

//typedef std::unordered_map<point, Component*, pointHash> cmap;
typedef Component** cmap;
typedef std::pair<point, Component*> cpair;
}
