#pragma once
#include <vector>
#include "barstrucs.h"

namespace bc
{
	template<class T>
	class BarcodeCreator;

	template<class T>
	struct barline;

	template<class T>
	class Component
	{
		Component<T>* cachedMaxParent = nullptr;
	protected:
		size_t totalCount = 0/*, ownSize = 0*/;
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
		barcounter<T>* bar3d = nullptr;
		//0 - nan
		size_t num = 0;
		//    pmap coords;
		barline<T>* resline;

		bool isAlive()
		{
			return lived;
		}

		T len()
		{
			return end - start;
		}

		Component<T>* getNonZeroParent()
		{
			if (parent == nullptr)
				return nullptr;

			Component<T>* ccomp = parent->parent;
			while (ccomp)
			{
				if (ccomp->parent->len() != 0)
					return ccomp;

				ccomp = ccomp->parent;
			}
			return nullptr;
		}

		//    cv::Mat binmap;
		Component<T>* getMaxParrent()
		{
			if (parent == nullptr)
				return this;

			if (cachedMaxParent == nullptr)
			{
				cachedMaxParent = parent;
			}
			while (cachedMaxParent->parent)
			{
				cachedMaxParent = cachedMaxParent->parent;
				//totalCount += cachedMaxParent->coords->size();
			}
			return cachedMaxParent;
		}
		Component<T> *getMaxAliveParrent()
		{
			auto *par = getMaxParrent();
			return par->isAlive() ? par : nullptr;
		}

		Component<T>* getMaxAliveParrent()
		{
			auto* par = getMaxParrent();
			return par->isAlive() ? par : nullptr;
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
