#pragma once
#include <vector>
#include "barstrucs.h"


#define POINTS_ARE_AVALIBLE
namespace bc
{
	template<class T>
	class BarcodeCreator;

	template<class T>
	struct barline;

	template<class T>
	class Component
	{
	public:
#ifdef POINTS_ARE_AVALIBLE
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
#endif // !POINAS_ARE_AVALIBLE
	protected:
		BarcodeCreator<T>* factory;
		Component<T>* cachedMaxParent = nullptr;

	public:
		Component<T>* parent = nullptr;
		barline<T>* resline = nullptr;

	protected:
		int cashedSize = 0;
		T lastVal = 0;

	private:
		bool lived = true;
		//0 - nan

		void init(BarcodeCreator<T>* factory);
	public:

		Component(point pix, BarcodeCreator<T>* factory);
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


		bool isContain(int x, int y);
		bool isContain(point p);
		virtual void add(const point& p);
		virtual void kill();
		virtual void setParrent(Component<T>* parnt);

		virtual ~Component();

	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
