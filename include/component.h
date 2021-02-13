#pragma once
#include "point.h"
#include <vector>
namespace bc
{
	template<class T>
	class barcodeCreator;

	template<class T>
	class EXPORT Component
	{
		Component<T>* cachedMaxParent = nullptr;
		size_t totalCount = 0;
	protected:
		barcodeCreator<T>* factory;
		bool lived = true;
	public:

		Component(point pix, barcodeCreator<T>* factory);
		Component(barcodeCreator<T>* factory, bool create = false);

		Component<T>* parent = nullptr;
		pmap<T>* coords;
		//pmap subCoords;
		T start = 0, end = 0;
		//0 - nan
		size_t num = 0;
		//    pmap coords;

		bool isAlive()
		{
			return lived;
		}

		//    cv::Mat binmap;
		Component<T>* getMaxParrent()
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

		bool isContain(int x, int y);
		bool isContain(point p);
		virtual void add(const point& p);
		virtual void kill();
		virtual void setParrent(Component<T>* parnt);
		//    void setB(const point &p);
		virtual ~Component();
	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
