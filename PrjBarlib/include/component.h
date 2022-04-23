#pragma once
#include <vector>
#include "barstrucs.h"


//#define POINTS_ARE_AVAILABLE
namespace bc
{
	typedef uint poidex;

	template<class T>
	class BarcodeCreator;

	template<class T>
	struct barline;

	template<class T>
	class Component
	{
	public:
#ifdef POINTS_ARE_AVAILABLE
		size_t getTotalSize()
		{
			return resline->matr.size();
		}
#else
		size_t startIndex = 0;
		size_t getTotalSize()
		{
			return totalCount;
		}
	private:
		size_t totalCount = 0/*, ownSize = 0*/;
#endif // !POINTS_ARE_AVAILABLE
	protected:
		BarcodeCreator<T>* factory;
		Component<T>* cachedMaxParent = nullptr;

	public:
		Component<T>* parent = nullptr;
		barline<T>* resline = nullptr;

	protected:
		int cashedSize = 0;
		T lastVal = 0;
		point startPoint;
		bool lived = true;

		float sums = 0;

	private:
		//0 - nan

		void init(BarcodeCreator<T>* factory);
	public:

		Component(poidex pix, BarcodeCreator<T>* factory);
		Component(BarcodeCreator<T>* factory, bool create = false);

		T getStart()
		{
			return resline->start;
		}

		T getLast()
		{
			return lastVal;
		}

		bool isAlive()
		{
			return lived;
		}

		//T len()
		//{
		//	//return round(100000 * (end - start)) / 100000;
		//	return  abs(resline->len());
		//	//return end - start;
		//}
		//    cv::Mat binmap;
		Component<T>* getMaxparent()
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


		bool isContain(poidex index);
		virtual bool add(poidex index);
		virtual bool add(poidex index, const point p);
		virtual void kill();
		virtual void setParent(Component<T>* parnt);


		bool canBeConnected(const bc::point& p, bool incrSum = false)
		{
			if (!factory->settings.maxLen.isCached)
				return true;
			T val = factory->workingImg->get(p.x, p.y);
			T diff;
			if (val > resline->start)
			{
				diff = val - resline->start;
			}
			else
			{
				diff = resline->start - val;
			}
			return diff <= factory->settings.maxLen.val;

			//Component<T>* comp = getMaxparent();
			//if ((float)comp->totalCount / factory->workingImg->length() >= .1f)
			//{
			//	float st = (float)comp->getStart();
			//	//float avg = ((float)comp->sums + val) / (comp->totalCount + 1);
			//	float avg = ((float)lastVal - st) / 2;
			//	float dff = abs((float)st - avg);
			//	if (abs(val - avg) > dff)
			//	{
			//		return false;
			//	}
			//}
			//if (incrSum)
			//	comp->sums += val;

			return true;
		}

		virtual ~Component();

	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
