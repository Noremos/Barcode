#pragma once
#include <vector>
#include "barstrucs.h"

namespace bc
{
	template<class T>
	class BarcodeCreator;

	template<class T>
	class Component
	{
		Component<T>* cachedMaxParent = nullptr;
	protected:
		size_t totalCount = 0;
		BarcodeCreator<T>* factory;
		bool lived = true;

		int cashedSize = 0;
		T lastVal = 0;

		void init(BarcodeCreator<T>* factory);
	public:

		Component(point pix, BarcodeCreator<T>* factory);
		Component(BarcodeCreator<T>* factory, bool create = false);

		Component<T>* parent = nullptr;
		//pmap<T>* coords;

		//pmap subCoords;
		T start = 0, end = 0;
		barcounter* bar3d;
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
				//totalCount += cachedMaxParent->coords->size();
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

		size_t getTotalSize()
		{
			return totalCount;
		}
	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
