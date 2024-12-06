#ifndef SKIP_M_INC
#pragma once

#include "presets.h"

#include "hole.h"
#include "barstrucs.h"
#include "barclasses.h"

#include <functional>
#include <memory>


#include "../extra/include_py.h"
#include "../extra/include_cv.h"
#endif

#define COMPP Component*
#define HOLEP Hole*

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
	MEXP struct indexCov
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
			default:
				assert(false);
				return { 0,0 };
			}
		}

		std::vector<poidex> getOffsets(const bc::point& p, int wid) const
		{
			std::vector<poidex> out;
			switch (poz)
			{
			case middleRight:
				out.push_back(barvalue::getStatInd(p.x, p.y - 1, wid)); // top
				out.push_back(barvalue::getStatInd(p.x, p.y + 1, wid)); // bottom
				out.push_back(barvalue::getStatInd(p.x + 1, p.y - 1, wid)); // top from right
				out.push_back(barvalue::getStatInd(p.x + 1, p.y + 1, wid)); // bottom from right
				break;
			case bottomRight:
				out.push_back(barvalue::getStatInd(p.x + 1, p.y, wid)); // right
				out.push_back(barvalue::getStatInd(p.x, p.y + 1, wid)); // bottom
				break;
			case bottomCenter:
				out.push_back(barvalue::getStatInd(p.x - 1, p.y, wid)); // left
				out.push_back(barvalue::getStatInd(p.x + 1, p.y, wid)); // right
				out.push_back(barvalue::getStatInd(p.x - 1, p.y + 1, wid)); // left from down
				out.push_back(barvalue::getStatInd(p.x + 1, p.y + 1, wid)); // right from down
				break;
			case bottomLeft:
				out.push_back(barvalue::getStatInd(p.x - 1, p.y, wid)); // left
				out.push_back(barvalue::getStatInd(p.x, p.y + 1, wid)); // bottom
				break;
			}

			return out;
		}
	};


	using Include = Component*;


	MEXP class EXPORT BarcodeCreator
	{
		typedef bc::DatagridProvider bcBarImg;

		bool allowEvOAttach = false;
		bool originalImg = true;
		std::vector<COMPP> components;

#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif
	public:
		barstruct settings;
		bool skipAddPointsToParent = false;
		size_t sameStart = 0;

	private:
		Include* included = nullptr;
		struct RebInfo
		{
			std::vector<Component*> cons;
			void add(Component* comp)
			{
				for (auto* c : cons)
				{
					if (c == comp)
						return;
				}

				cons.push_back(comp);
			}

			void clearDeleted()
			{
				for (int i = cons.size() - 1; i >= 0; i--)
				{
					if (cons[i]->resline == nullptr)
						cons.erase(cons.begin() + i);
				}
			}

			bool exists(Component* comp)
			{
				for (auto* c : cons)
				{
					if (c == comp)
						return true;
				}
				return false;
			}

		};
		barmap<size_t, RebInfo> connections;

	protected:
		const DatagridProvider* workingImg = nullptr;
	private:
		void setWorkingImg(const bcBarImg* newWI)
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;
			workingImg = newWI;
		}

	protected:
		BarType type;

	private:
		bool needDelImg = false;
		Barscalar curbright;
		poidex curpoindex;
		//uint compIndex = 0;
		point curpix;
		int wid;
		int hei;
		Baritem* root;
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
			return true;
			return a.absDiff(b) <= this->settings.getMaxStepPorog();
		}
	public:
		point getPoint(poidex i) const
		{
			return point(i % wid, i / wid);
		}

		Barscalar getValue(poidex i) const
		{
			return workingImg->getLiner(i);
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

		HOLEP getHole(int x, int y);
		HOLEP getHole(const point& p);

		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB1();


		int sortOrtoPixels(bc::ProcType type, int rtoe = 0, int off = 0, int offDop = 0);
		void sortPixels(bc::ProcType type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISUAL_DEBUG();
		void VISUAL_DEBUG_COMP();

	public:
		void init(const bc::DatagridProvider* src, ProcType& type);

	private:
		void processComp(Barcontainer* item = nullptr);
		void processHole(Barcontainer* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer* item = nullptr);

		void processTypeF(const bc::DatagridProvider* img, Barcontainer* item = nullptr);

		void processFULL(const bc::DatagridProvider* img, bc::Barcontainer* item);
		void addItemToCont(Barcontainer* item);

		void computeNdBarcode(Baritem* lines, int n);

	public:

//#ifdef ENABLE_ENERGY
		std::unique_ptr<int[]> energy;
		float maxe = 0;
//#endif

		bc::Barcontainer* createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure);
		bc::Baritem* createBarcode(const bc::DatagridProvider* img, const barstruct& structure);
		//bc::Barcontainer* searchHoles(float* img, int wid, int hei, float nullVal = -9999);

		static std::unique_ptr<bc::Baritem> create(const bc::DatagridProvider& img, const barstruct& structure = {});

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

		static Barcontainer* pycreateMultiple(bn::array& img, bc::BarConstructor& structure);
		static Baritem* pycreate(bn::array& img, const barstruct& structure);
#endif // _PYD

		///////////GEOMETRY
	private:
		void processCompByRadius(Barcontainer* item);
		void processCompByStepRadius(Barcontainer* item);
		void processByValueRadius(Barcontainer* item);

		void processRadius(const indexCov& val, bool allowAttach);

		void processHoleByRadius(Barcontainer* item);
		void processHoleRadius(const indexCov& val);

		bool checkAvg(const point curpix) const;

	protected:
		std::unique_ptr<indexCov> geometrySortedArr;
	};

	MEXP using PloyPoints = std::vector<bc::point>;
}
