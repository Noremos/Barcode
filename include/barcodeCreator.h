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

		bool reverse = false;
		int b[256];

		BarConstructor<T> settings;

		Include<T>* included;
		const DatagridProvider<T>* workingImg = nullptr;
		void setWorkingImg(const bcBarImg* newWI)
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;

			workingImg = newWI;

			if (settings.stepPorog.isCached || settings.stepPorog.isCached)
			{
				T maxVal = workingImg->max();

				if (!settings.stepPorog.isCached)
					settings.stepPorog.set(maxVal);

				if (!settings.maxLen.isCached)
					settings.maxLen.set(maxVal);
			}

		}

		bool needDelImg = false;
		T curbright;
		point curpix;
		int wid;
		int hei;
		int lastB;
		friend class Component<T>;
		friend class Hole<T>;
		//		friend struct BarRoot<T>;
		friend class Baritem<T>;

		size_t totalSize = 0;
		point* sortedArr;

		//***************************************************

		int GETPOFF(const point& p) const;

		constexpr int GETOFF(int x, int y) const {
			return wid * y + x;
		}

		constexpr T GETDIFF(T a, T b) const {

			return a > b ? a - b : b - a;
		}

		point getPoint(size_t i) const
		{
			return point(static_cast<int>(i % (size_t)wid), static_cast<int>(i / (size_t)wid));
		}

		//#define GETPOFF(P) (this->wid*P.y+P.x)
		//#define GETOFF(X, Y) (this->wid*y+x)

		bool isContain(int x, int y) const;
		bool isContain(const point& p, bool valid) const;
		bool isContain(const point& p) const;

		void setInclude(int x, int y, COMPP comp);
		void setInclude(const point& p, COMPP comp);

		//inline void setInclude(int x, int y, COMPP comp)
		//{
		//	setInclude(x, y, comp);
		//}

		//inline void setInclude(const point& p, COMPP comp)
		//{
		//	setInclude(p, comp);
		//}

		COMPP getComp(int x, int y);
		COMPP getComp(const point& p);
		COMPP getPorogComp(const point& p);

		Include<T>* getInclude(size_t pos);

		HOLEP getHole(int x, int y);
		HOLEP getHole(const point& p);

		COMPP attach(COMPP first, COMPP second);
		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB0();
		bool checkCloserB1();

		point* sort();

		void clearIncluded();

		void draw(std::string name = "test");
		void VISULA_DEBUG(int y, int i);
		void VISULA_DEBUG_COMP(int y, int i);


		void init(const bc::DatagridProvider<T>* src, const  ProcType& type);

		void processComp(int* retBty, Barcontainer<T>* item = nullptr);
		void processHole(int* retBty, Barcontainer<T>* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);

		bc::Baritem<T>* getBarcode();

		void processTypeF(const barstruct& str, const bc::DatagridProvider<T>* img, Barcontainer<T>* item = nullptr);

		void processFULL(const barstruct& str, const bc::DatagridProvider<T>* img, bc::Barcontainer<T>* item);
		void addItemToCont(Barcontainer<T>* item);

		void reverseCom();

		void computeBettyBarcode(Baritem<T>* lines);
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

		[[deprecated]]
		bc::Barcontainer<T>* createBarcode(bcBarImg* img, const std::vector<barstruct>& structure);

		[[deprecated]]
		bc::Barcontainer<T>* createBarcode(bcBarImg* img, const barstruct* structure, int size);

		[[deprecated]]
		bc::Barcontainer<T>* createSLbarcode(const bcBarImg* src, T foneStart, T foneEnd, Barcontainer<T>* cont = nullptr);

		[[deprecated]]
		bc::Barcontainer<T>* createSLbarcode(const bcBarImg* src, T foneStart, T foneEnd, bool createRGBbar);


		bc::Barcontainer<T>* searchHoles(float* img, int wid, int hei);

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
