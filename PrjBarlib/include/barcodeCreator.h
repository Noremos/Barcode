#pragma once

#include "presets.h"

#include "hole.h"
#include "barstrucs.h"
#include "barclasses.h"

#include <unordered_map>
#include <limits>

#define COMPP Component<T>*
#define HOLEP Hole<T>*

#include "include_py.h"
#include "include_cv.h"

namespace bc {

	// # - позиции; * - текущий поинт
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

		inline static float safeDiffSqr(const float& a, const float& b)
		{
			return a > b ? ((a - b) * (a - b)) : ((b - a) * (b - a));
		}

		template<class TK, uint N>
		inline static float val_distance(const barVector<TK, N>& fisrt, const barVector<TK, N>& second)
		{
			float dist = 0;
			for (uint i = 0; i < N; ++i)
			{
				dist += safeDiffSqr(static_cast<float>(second[i]), static_cast<float>(fisrt[i]));
			}
			return sqrtf(dist);
		}

		template<class TK>
		inline static float val_distance(const TK& a, const TK& b)
		{
			return a > b ? (a - b) : (b - a);
		}
	};

	template<class T>
	using Include = Component<T>*;

	template<class T>
	class EXPORT BarcodeCreator
	{
		typedef bc::DatagridProvider<T> bcBarImg;

		bool originalImg = true;
		std::vector<COMPP> components;
	private:
#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif

		BarConstructor<T> settings;
		bool skipAddPointsToParent = false;

		Include<T>* included = nullptr;
		DatagridProvider<T>* workingImg = nullptr;
		void setWorkingImg(bcBarImg* newWI)
		{
			if (!saveImg)
			{
				if (workingImg != nullptr && needDelImg)
					delete workingImg;

				workingImg = newWI;
			}

			if (!settings.stepPorog.isCached || !settings.maxLen.isCached)
			{
				T maxVal, minVal;
				workingImg->maxAndMin(minVal, maxVal);

				if (!settings.stepPorog.isCached)
					settings.stepPorog.set(maxVal - minVal);

				if (!settings.maxLen.isCached)
					settings.maxLen.set(maxVal - minVal);
			}
		}

		bool needDelImg = false;
		T curbright;
		poidex curpoindex;
		uint curIndexInSortedArr;
		point curpix;
		int wid;
		int hei;
		T sourceMax;
		T sourceMin;
		// int lastB;
		friend class Component<T>;
		friend class Hole<T>;
		//		friend struct BarRoot<T>;
		friend class Baritem<T>;

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

		constexpr bool GETDIFF(T a, T b) const
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

		inline void setInclude(poidex ind, COMPP comp)
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


		void init(bc::DatagridProvider<T>* src, ProcType& type, ComponentType& comp);

		void processComp(Barcontainer<T>* item = nullptr);
		void processHole(Barcontainer<T>* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);

		void processTypeF(barstruct& str, bc::DatagridProvider<T>* img, Barcontainer<T>* item = nullptr);

		void processFULL(barstruct& str, bc::DatagridProvider<T>* img, bc::Barcontainer<T>* item);
		void addItemToCont(Barcontainer<T>* item);

		void computeNdBarcode(Baritem<T>* lines, int n);

	public:
		BarcodeCreator()
		{
		}
		BarcodeCreator(const BarcodeCreator&)
		{

		}

		bc::Barcontainer<T>* createBarcode(bc::DatagridProvider<T>* img, const BarConstructor<T>& structure);
		/*{
			this->settings = structure;
			settings.checkCorrect();

			Barcontainer<T>* cont = new Barcontainer<T>();

			for (const auto& it : settings.structure)
			{
				processFULL(it, img, cont);
			}
			return cont;
		}*/

		bc::Barcontainer<T>* searchHoles(float* img, int wid, int hei, float nullVal = -9999);


		virtual ~BarcodeCreator()
		{
			//			clearIncluded();
#ifdef USE_OPENCV
			colors.clear();
#endif // USE_OPENCV
		}

#ifdef _PYD

		bc::Barcontainer<T>* createBarcode(bn::ndarray& img, bc::BarConstructor<T>& structure);
#endif // _PYD

		///////////GEOMETRY
	private:
		void processCompByRadius(Barcontainer<T>* item, float maxLen = std::numeric_limits<float>::max());
		void processImageByD(Barcontainer<T>* item);



		std::unique_ptr<indexCov> geometrySortedArr;
		bool saveImg = false;
		};

	template<class T>
	static Barcontainer<T>* createBarcode(bc::BarType type, bc::DatagridProvider<T>* img, BarConstructor<T>& _struct)
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
	}
	}
