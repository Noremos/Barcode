#pragma once

#include "presets.h"

#include "hole.h"
#include "barstrucs.h"
#include "barclasses.h"

#include <unordered_map>

#define COMPP Component<T>*
#define HOLEP Hole<T>*

#include "include_py.h"
#include "include_cv.h"

namespace bc {

	template<class T>
	using Include = Component<T>*;

	template<class T>
	class EXPORT BarcodeCreator
	{
		typedef bc::DatagridProvider<T> bcBarImg;

		bool originalImg = true;
	public:
		std::vector<COMPP> components;
	private:
#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif

		BarConstructor<T> settings;
		bool skipAddPointsToParrent = false;

		Include<T>* included = nullptr;
		const DatagridProvider<T>* workingImg = nullptr;
		void setWorkingImg(const bcBarImg* newWI)
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;

			workingImg = newWI;

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
		uint wid;
		uint hei;
		T sourceMax;
		T sourceMin;
		// int lastB;
		friend class Component<T>;
		friend class Hole<T>;
		//		friend struct BarRoot<T>;
		friend class Baritem<T>;

		size_t totalSize = 0;
		poidex* sortedArr;

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
			return itr ? itr->getMaxParrent() : nullptr;
		}


		COMPP getPorogComp(const point& p, poidex index);

		Include<T>* getInclude(size_t pos);

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

		poidex* sortPixels(bc::ProcType type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISULA_DEBUG();
		void VISULA_DEBUG_COMP();


		void init(const bc::DatagridProvider<T>* src, const  ProcType& type);

		void processComp(Barcontainer<T>* item = nullptr);
		void processHole(Barcontainer<T>* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);

		void processTypeF(const barstruct& str, const bc::DatagridProvider<T>* img, Barcontainer<T>* item = nullptr);

		void processFULL(const barstruct& str, const bc::DatagridProvider<T>* img, bc::Barcontainer<T>* item);
		void addItemToCont(Barcontainer<T>* item);

		void computeNdBarcode(Baritem<T>* lines, int n);

	public:
		BarcodeCreator()
		{
		}

		bc::Barcontainer<T>* createBarcode(const bc::DatagridProvider<T>* img, const BarConstructor<T>& structure);
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
			clearIncluded();
#ifdef USE_OPENCV
			colors.clear();
#endif // USE_OPENCV
		}

#ifdef _PYD

		bc::Barcontainer<T>* createBarcode(bn::ndarray& img, bc::BarConstructor<T>& structure)
		{
			//auto shape = img.get_shape();

			/*	int type = cv_8uc1;
				if (img.get_nd() == 3 && img.shape[2] == 3)
					image = &barimg<bcvec3b>(shape[0], shape[1], img.get_data());
				else if float
					barimg image(shape[0], shape[1], img.get_data());*/

					//cv::imshow("test", image);
					//cv::waitkey(0);
			bc::BarNdarray<T> image(img);
			//try
			//{
			return createBarcode(&image, structure);
			//}
			//catch (const std::exception& ex)
			//{
				//printf("ERROR");
				//printf(ex.what());
			//}

			//bc::BarImg<T> image(img.shape(1), img.shape(0), img.get_nd(), (uchar*)img.get_data(), false, false);
			//return createBarcode(&image, structure);
		}
#endif // _PYD
	};

	template<class T>
	static Barcontainer<T>* createBarcode(bc::BarType type, const bc::DatagridProvider<T>* img, BarConstructor<T>& strct)
	{
		switch (type)
		{
		case bc::BarType::bc_byte:
		{
			BarcodeCreator<ushort> t;
			return t.createBarcode(img, strct);
		}
		case bc::BarType::bc_float:
		{
			BarcodeCreator<float> t;
			return t.createBarcode(img, strct);
		}
		case bc::BarType::bc_int:
		{
			BarcodeCreator<int> t;
			return t.createBarcode(img, strct);
		}
		case bc::BarType::bc_short:
		{
			BarcodeCreator<short> t;
			return t.createBarcode(img, strct);
		}
		case bc::BarType::bc_ushort:
		{
			BarcodeCreator<ushort> t;
			return t.createBarcode(img, strct);
		}
		default:
			return nullptr;
		}
	}
}
