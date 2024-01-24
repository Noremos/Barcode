#ifndef SKIP_M_INC
#pragma once
#include <vector>
#include "barline.h"
#endif

namespace bc
{

	class BarcodeCreator;


	class Component
	{
	public:
#ifdef POINTS_ARE_AVAILABLE
		size_t getTotalSize() const
		{
			return resline->matr.size();
		}
#else
		size_t startIndex = 0;
		size_t getTotalSize() const
		{
			return totalCount;
		}
	private:
		size_t totalCount = 0/*, ownSize = 0*/;
#endif // !POINTS_ARE_AVAILABLE
	protected:
		BarcodeCreator* factory;
		Component* cachedMaxParent = nullptr;

	public:
		Component* parent = nullptr;
		barline* resline = nullptr;

		float energy = 0;
	protected:
		int cashedSize = 0;
		Barscalar lastDistance = 0;
		bool same = true;
		//Barscalar avgSr = 0;
		//point startPoint;
		bool lived = true;

		//float sums = 0;

	private:
		//0 - nan

		//exp
		size_t x = 0, y = 0, xMin = 9999999, yMin = 9999999, xMax = 0, yMax = 0;

		void init(BarcodeCreator* factory, const Barscalar& val);
	public:


		Component(poidex pix, const Barscalar& val, const Barscalar& distance, bc::BarcodeCreator* factory);
		Component(BarcodeCreator* factory, Barscalar start);

		int getLastRowSize()
		{
			return cashedSize;
		}

		Barscalar getStart();

		//Barscalar getLast()
		//{
		//	return lastVal;
		//}

		bool isAlive()
		{
			return lived;
		}
		void markNotSame();
		virtual bool justCreated(const Barscalar& currentDistance);

		//Barscalar len()
		//{
		//	//return round(100000 * (end - start)) / 100000;
		//	return  abs(resline->len());
		//	//return end - start;
		//}
		//    cv::Mat binmap;
		Component* getMaxparent()
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
		virtual bool add(const poidex index, const point p, const Barscalar& value, const Barscalar& distance, bool forsed = false);
		void kill();
		virtual void kill(const Barscalar& endScalar);
		void addChild(Component* child, const Barscalar& lastValue, const Barscalar& distance);


		bool canBeConnected(const bc::point& p, bool incrSum = false);

		virtual ~Component();

		static void passSame(BarcodeCreator* factory);
		static void passConnections(BarcodeCreator* factory);
		static void process(BarcodeCreator* factory);
		void merge(bc::Component* dummy);

		struct AttachInfo
		{
			bc::Component* comp;
			Barscalar diff;
		};
		using AttachList = std::vector<AttachInfo>;
		static void attach(const barstruct& settings, bc::point p, bc::poidex index, Barscalar& bars, AttachList& attachList);
	};

	//typedef barmap<point, Component*, pointHash> cmap;

	class RadiusComponent : public Component
	{
	public:
		RadiusComponent(BarcodeCreator* factory, const Barscalar& startDistance) : Component(factory, startDistance)
		{ }

		void addInit(poidex pix1, bc::point p1, const Barscalar& val1, poidex pix2, bc::point p2, const Barscalar& val2)
		{
			add(pix1, p1, val1, lastDistance);
			add(pix2, p2, val2, lastDistance);
		}

		bool justCreated(const Barscalar& distance) override
		{
			return same || resline->matr.size() == 2;
		}
	};

	//class RadiusHole;

	//struct RCon
	//{
	//	poidex cons[8];
	//	char size = 0;

	//	std::vector<RadiusHole*> holes;

	//	RCon()
	//	{
	//		std::fill_n(cons, 8, -1);
	//	}
	//};
	//class RadiusRoot
	//{
	//public:
	//	std::vector<RCon> rebs;
	//	std::vector<RadiusHole*> field;
	//	std::vector<std::unique_ptr<RadiusHole>> holes;
	//	DatagridProvider* img;

	//	void findPath(const poidex p1, const poidex p2, bool first, std::vector<poidex>& out);
	//	void addConnection(const poidex p1, const poidex p2);

	//};

	//class RadiusHole
	//{
	//	barline* resline;
	//	int leftToCollapse;
	//public:

	//	RadiusHole(const std::vector<poidex>& path);
	//	void addConnection();
	//	inline bool isAlive() const
	//	{
	//		return leftToCollapse > 0;
	//	}
	//	void add(const barvalue& value);
	//	void tryAddChild();

	//};
}
