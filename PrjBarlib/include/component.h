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
	public:
		size_t num = 0;

	protected:
		size_t totalCount = 0/*, ownSize = 0*/;
		BarcodeCreator<T>* factory;
		Component<T>* cachedMaxParent = nullptr;
		Component<T>* cachedNonZeroParent = nullptr;

	public:
		Component<T>* parent = nullptr;
		barcounter<T>* bar3d = nullptr;
		barline<T>* resline = nullptr;

	protected:
		int cashedSize = 0;
		T lastVal = 0;

	public:
		T start = 0, end = 0;

	private:
		bool lived = true;
		//0 - nan

		void init(BarcodeCreator<T>* factory);
	public:

		Component(point pix, BarcodeCreator<T>* factory);
		Component(BarcodeCreator<T>* factory, bool create = false);

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

			if (cachedNonZeroParent == nullptr)
			{
				cachedNonZeroParent = parent;
			}

			while (cachedNonZeroParent)
			{
				if (cachedNonZeroParent->len() != 0)
					return cachedNonZeroParent;

				cachedNonZeroParent = cachedNonZeroParent->parent;
			}
			return cachedNonZeroParent;
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

		bool isContain(int x, int y);
		bool isContain(point p);
		virtual void add(const point& p);
		virtual void kill();
		virtual void setParrent(Component<T>* parnt);

		virtual ~Component();

		size_t getTotalSize()
		{
			return totalCount;
		}
	};

	//typedef std::unordered_map<point, Component*, pointHash> cmap;
}
