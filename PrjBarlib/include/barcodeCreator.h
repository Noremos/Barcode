#pragma once

#include "presets.h"

#include "hole.h"
#include "barstrucs.h"
#include "barclasses.h"

#include <functional>
#include <memory>

#define COMPP Component*
#define HOLEP Hole*

#include "include_py.h"
#include "include_cv.h"

namespace bc {

	// # - �������; * - ������� �����
	// 00#
	// 0*#
	// 0##
	enum nextPoz : char
	{
		//topRight = 0,
		middleRight = 1,
		bottomRight = 2,

		//topCenter,
		//middleCenter,
		bottomCenter,

		//topLeft,
		//middleLeft,
		bottomLeft
	};
	struct indexCov
	{
		poidex offset = 0;
		float dist = 0;
		nextPoz poz;
		indexCov(uint _offset = 0, float _dist = 0, nextPoz _vert = middleRight) : offset(_offset), dist(_dist), poz(_vert)
		{}

		bc::point getNextPoint(const bc::point& p) const
		{
			switch (poz)
			{
			case middleRight:
				return bc::point(p.x + 1, p.y);
			case bottomRight:
				return bc::point(p.x + 1, p.y + 1);
			case bottomCenter:
				return bc::point(p.x, p.y + 1);
			case bottomLeft:
				return bc::point(p.x - 1, p.y + 1);
			}
		}
	};


	using Include = Component*;


	class EXPORT BarcodeCreator
	{
		typedef bc::DatagridProvider bcBarImg;

		bool allowEvOAttach = false;
		bool originalImg = true;
		std::vector<COMPP> components;

#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif

		BarConstructor settings;
		bool skipAddPointsToParent = false;

		Include* included = nullptr;

	protected:
		const DatagridProvider* workingImg = nullptr;
	private:
		void setWorkingImg(const bcBarImg* newWI)
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;
			workingImg = newWI;

			if (!settings.stepPorog.isCached || !settings.maxLen.isCached)
			{
				Barscalar maxVal, minVal;
				workingImg->maxAndMin(minVal, maxVal);

				if (!settings.stepPorog.isCached)
					settings.stepPorog.set(maxVal - minVal);

				if (!settings.maxLen.isCached)
					settings.maxLen.set(maxVal - minVal);
			}
		}

		BarType type;

		bool needDelImg = false;
		Barscalar curbright;
		poidex curpoindex;
		//uint compIndex = 0;
		point curpix;
		int wid;
		int hei;
		//Barscalar sourceMax;
		//Barscalar sourceMin;
		// int lastB;
		friend class Component;
		friend class Hole;
		//		friend struct BarRoot;
		friend class Baritem;

	protected:
		uint curIndexInSortedArr;
		size_t processCount = 0;
		size_t totalSize = 0;
		std::unique_ptr<poidex[]> sortedArr = nullptr;
		// bc::BarImg drawimg;

		//***************************************************
		constexpr bool IS_OUT_OF_REG(const int x, const int y) const
		{
			return x < 0 || y < 0 || x >= wid || y >= hei;
		}

		poidex GETPOFF(const point& p) const
		{
			return wid * p.y + p.x;
		}

		poidex GETOFF(uint x, uint y) const {
			return wid * y + x;
		}

		bool GETDIFF(const Barscalar& a, const Barscalar& b) const
		{
			return !this->settings.stepPorog.isCached || (a > b ? a - b : b - a) <= this->settings.getMaxStepPorog();
		}

		point getPoint(poidex i) const
		{
			return point(i % wid, i / wid);
		}

	private:
		//#define GETPOFF(P) (this->wid*P.y+P.x)
		//#define GETOFF(X, Y) (this->wid*y+x)

		bool isContain(poidex ind) const
		{
			return included[ind] != nullptr;
		}

		inline void setInclude(const poidex ind, COMPP comp)
		{
			included[ind] = comp;
		}

		inline COMPP getComp(poidex ind)
		{
			auto itr = included[ind];
			return itr ? itr->getMaxparent() : nullptr;
		}

		inline COMPP getRealComp(poidex ind)
		{
			auto itr = included[ind];
			return itr ? itr : nullptr;
		}


		COMPP getPorogComp(const point& p, poidex index);

		COMPP getInclude(size_t pos);

		// ONLY FOR HOLE
		bool isContain(const point& p) const
		{
			if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
				return false;

			return included[wid * p.y + p.x] != nullptr;
		}

		HOLEP getHole(uint x, uint y);
		HOLEP getHole(const point& p);

		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB1();


		int sortOrtoPixels(bc::ProcType type, int rtoe = 0, int off = 0, int offDop = 0);
		void sortPixels(bc::ProcType type, const bc::DatagridProvider* mask, int maskId);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISUAL_DEBUG();
		void VISUAL_DEBUG_COMP();

	public:
		void init(const bc::DatagridProvider* src, ProcType& type, const barstruct& struc);

	private:
		void processComp(Barcontainer* item = nullptr);
		void processHole(Barcontainer* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer* item = nullptr);

		void processTypeF(barstruct& str, const bc::DatagridProvider* img, Barcontainer* item = nullptr);

		void processFULL(barstruct& str, const bc::DatagridProvider* img, bc::Barcontainer* item);
		void addItemToCont(Barcontainer* item);

		void computeNdBarcode(Baritem* lines, int n);

	public:

//#ifdef ENABLE_ENERGY
		std::unique_ptr<int[]> energy;
		float maxe = 0;
//#endif

		bc::Barcontainer* createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure);
		//bc::Barcontainer* searchHoles(float* img, int wid, int hei, float nullVal = -9999);


		BarcodeCreator()
		{ }

		BarcodeCreator(const BarcodeCreator&)
		{ }

		virtual ~BarcodeCreator()
		{
			//			clearIncluded();
#ifdef USE_OPENCV
			colors.clear();
#endif // USE_OPENCV
		}

#ifdef _PYD

		bc::Barcontainer* createPysBarcode(bn::array& img, bc::BarConstructor& structure);
#endif // _PYD

		///////////GEOMETRY
	private:
		void processCompByRadius(Barcontainer* item);
		void processCompByStepRadius(Barcontainer* item);
		void processByValueRadius(Barcontainer* item);

		void processRadar(const indexCov& val, bool allowAttach);
		bool checkAvg(const point curpix) const;

	protected:
		std::unique_ptr<indexCov> geometrySortedArr;
	};

	using PloyPoints = std::vector<bc::point>;
	class CloudPointsBarcode
	{
		struct PointIndexCov
		{
			uint points[2];
			float dist = 0;
			PointIndexCov(uint ind0 = 0, uint ind1 = 0, float _dist = 0) : dist(_dist)
			{
				points[0] = ind0;
				points[1] = ind1;
			}
		};

	public:
		struct CloudPoint
		{
			CloudPoint(int x, int y, float z) : x(x), y(y), z(z)
			{}
			int x, y;
			float z;
			float distanse(const CloudPoint& R) const
			{
				float res = sp(x - R.x) + sp(y - R.y) + sp(z - R.z);
				return sqrtf(res);
			}

			Barscalar getScalar()
			{
				Barscalar a;
				//a.data.i3 = new int[3] { x, y, z };
				//a.type = BarType::INT32_3;
				a.data.f = z;
				a.type = BarType::FLOAT32_1;
				return a;
			}

		private:
			float sp(float v) const
			{
				return v * v;
			}
			float sp(int v) const
			{
				return (float)(v * v);
			}
		};

		struct CloudPoints
		{
			std::vector<CloudPoint> points;
		};

	public:
		CloudPointsBarcode()
		{}

		bc::Barcontainer* createBarcode(const CloudPoints* points);
		//bc::Barcontainer* searchHoles(float* img, int wid, int hei, float nullVal = -9999);
		bool useHolde = false;
		// static std::function<void(const point&, const point&, bool)> drawLine;
		// static std::function<void(PloyPoints&, bool)> drawPlygon;
		using ComponentsVector = std::vector<barline*>;

	private:
		bool isContain(poidex ind) const
		{
			return included.find(ind) != included.end();
		}

		inline void setInclude(const poidex ind, barline* comp)
		{
			included[ind] = comp;
		}

		inline barline* getComp(poidex ind) const
		{
			auto itr = included.find(ind);
			return itr != included.end() ? itr->second : nullptr;
		}

		void sortPixels();
		void sortTriangulate();

		void processTypeF(const CloudPoints* points, Barcontainer* item = nullptr);
		void processFULL(const CloudPoints* points, bc::Barcontainer* item);

		void process(Barcontainer *item);
		void processComp(const CloudPointsBarcode::PointIndexCov& val);
		void processHold();

		void addItemToCont(Barcontainer* item);
		void clearIncluded();


	private:
		friend class Baritem;

		const CloudPoints* cloud = nullptr;
		uint curIndexInSortedArr = 0;
		size_t totalSize = 0;
		barmap<poidex, barline*> included;
		ComponentsVector components;
		std::unique_ptr<PointIndexCov> sortedArr;

	};
}
