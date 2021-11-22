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
		bool lived = true;

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
		virtual void add(poidex index);
		virtual void add(poidex index, const point p);
		virtual void kill();
		virtual void setParent(Component<T>* parnt);

		virtual ~Component();

	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
