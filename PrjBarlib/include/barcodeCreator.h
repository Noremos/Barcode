#pragma once

#include "presets.h"

#include "hole.h"
#include "barstrucs.h"
#include "barclasses.h"

#include <unordered_map>

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
		topRight = 0,
		middleRight = 1,
		downRight = 2,
		downCur = 3
	};
	struct indexCov
	{
		poidex offset = 0;
		float dist = 0;
		nextPoz poz;
		indexCov(uint _offset = 0, float _dist = 0, nextPoz _vert = topRight) : offset(_offset), dist(_dist), poz(_vert)
		{}

		bc::point getNextPoint(const bc::point& p) const
		{
			switch (poz)
			{
			case topRight:
				return bc::point(p.x + 1, p.y - 1);
			case middleRight:
				return bc::point(p.x + 1, p.y);
			case downRight:
				return bc::point(p.x + 1, p.y + 1);
			case downCur:
				return bc::point(p.x, p.y + 1);
			}
		}
	};

	
	using Include = Component*;

	
	class EXPORT BarcodeCreator
	{
		typedef bc::DatagridProvider bcBarImg;

		bool originalImg = true;
		std::vector<COMPP> components;
	private:
#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif

		BarConstructor settings;
		bool skipAddPointsToParent = false;

		Include* included = nullptr;
		const DatagridProvider* workingImg = nullptr;
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
		uint curIndexInSortedArr;
		point curpix;
		int wid;
		int hei;
		Barscalar sourceMax;
		Barscalar sourceMin;
		// int lastB;
		friend class Component;
		friend class Hole;
		//		friend struct BarRoot;
		friend class Baritem;

		size_t processCount = 0;
		size_t totalSize = 0;
		poidex* sortedArr = nullptr;

		//***************************************************
		constexpr bool IS_OUT_OF_REG(int x, int y)
		{
			return x < 0 || y < 0 || x >= wid || y >= hei;
		}
		int GETPOFF(const point& p) const
		{
			return wid * p.y + p.x;
		}

		int GETOFF(uint x, uint y) const {
			return wid * y + x;
		}

		constexpr bool GETDIFF(const Barscalar &a, const Barscalar& b) const
		{
			return (a > b ? a - b : b - a) <= this->settings.getMaxStepPorog();
		}

		point getPoint(uint i) const
		{
			return point(i % wid, i / wid);
		}

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

		COMPP attach(COMPP first, COMPP second);
		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB0();
		bool checkCloserB1();


		void sortPixels(bc::ProcType type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISUAL_DEBUG();
		void VISUAL_DEBUG_COMP();


		void init(const bc::DatagridProvider* src, ProcType& type, ComponentType& comp);

		void processComp(Barcontainer* item = nullptr);
		void processHole(Barcontainer* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer* item = nullptr);

		void processTypeF(barstruct& str, const bc::DatagridProvider* img, Barcontainer* item = nullptr);

		void processFULL(barstruct& str, const bc::DatagridProvider* img, bc::Barcontainer* item);
		void addItemToCont(Barcontainer* item);

		void computeNdBarcode(Baritem* lines, int n);

	public:
		BarcodeCreator()
		{
		}
		BarcodeCreator(const BarcodeCreator&)
		{

		}

		bc::Barcontainer* createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure);
		/*{
			this->settings = structure;
			settings.checkCorrect();

			Barcontainer* cont = new Barcontainer();

			for (const auto& it : settings.structure)
			{
				processFULL(it, img, cont);
			}
			return cont;
		}*/

		bc::Barcontainer* searchHoles(float* img, int wid, int hei, float nullVal = -9999);


		virtual ~BarcodeCreator()
		{
			//			clearIncluded();
#ifdef USE_OPENCV
			colors.clear();
#endif // USE_OPENCV
		}

#ifdef _PYD

		bc::Barcontainer* createBarcode(bn::ndarray& img, bc::BarConstructor& structure);
#endif // _PYD

		///////////GEOMETRY
	private:
		void processCompByRadius(Barcontainer* item);
		


		std::unique_ptr<indexCov> geometrySortedArr;
	};

	/*
	static Barcontainer* createBarcode(bc::BarType type, const bc::DatagridProvider* img, BarConstructor& _struct)
	{
		switch (type)
		{
		case bc::BarType::bc_byte:
		{
			BarcodeCreator<ushort> t;
			return t.createBarcode(img, _struct);
		}
		case bc::BarType::bc_float:
		{
			BarcodeCreator<float> t;
			return t.createBarcode(img, _struct);
		}
		case bc::BarType::bc_int:
		{
			BarcodeCreator<int> t;
			return t.createBarcode(img, _struct);
		}
		case bc::BarType::bc_short:
		{
			BarcodeCreator<short> t;
			return t.createBarcode(img, _struct);
		}
		case bc::BarType::bc_ushort:
		{
			BarcodeCreator<ushort> t;
			return t.createBarcode(img, _struct);
		}
		default:
			return nullptr;
		}
	}*/
}
