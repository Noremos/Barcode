#pragma once

#include "presets.h"

#include "hole.h"
#include "barstrucs.h"
#include "barclasses.h"

#include <unordered_map>

#define COMPP Component<T>*
#define HOLEP Hole<T>*

namespace bc {

	template<class T>
	struct Include
	{
	public:
		bc::Component<T>* comp;
		T bright;
		void setValues(bc::Component<T>* comp, T bright)
		{
			this->comp = comp;
			this->bright = bright;
		}
	};

	template<class T>
	class EXPORT BarcodeCreator
	{
		typedef bc::BarImg<T> bcBarImg;

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
		BarImg<T>* workingImg = nullptr;
		void setWorkingImg(BarImg<T>* newWI)
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;

			workingImg = newWI;
			if (!settings.stepPorog.isCached)
				settings.stepPorog.set(workingImg->max());
		}
		bool needDelImg = false;
		T curbright;
		point curpix;
		int wid;
		int hei;
		int lastB;
		friend class Component<T>;
		friend class Hole<T>;
		friend struct BarRoot<T>;
		friend class Baritem<T>;

		size_t totalSize = 0;
		point* sortedArr;

		//***************************************************

		int GETPOFF(const point& p) const;

		constexpr int GETOFF(int x, int y) const {
			return wid * y + x;
		}

		constexpr T GETDIFF(T a, T b) const {
			
			return a > b? a-b:b-a;
		}

		inline point getPoint(size_t i) const
		{
			return point(static_cast<int>(i % (size_t)wid), static_cast<int>(i / (size_t)wid));
		}

		//#define GETPOFF(P) (this->wid*P.y+P.x)
		//#define GETOFF(X, Y) (this->wid*y+x)

		bool isContain(int x, int y) const;
		bool isContain(const point& p, bool valid) const;
		bool isContain(const point& p) const;

		void setInclude(int x, int y, COMPP comp, T bright);
		void setInclude(const point& p, COMPP comp, T bright);

		inline void setInclude(int x, int y, COMPP comp)
		{
			setInclude(x, y, comp, curbright);
		}

		inline void setInclude(const point& p, COMPP comp)
		{
			setInclude(p, comp, curbright);
		}

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

		point* sort(const ProcType& type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISULA_DEBUG(int y, int i);
		void VISULA_DEBUG_COMP(int y, int i);
		void init(const bcBarImg& src, const ProcType& type);

		void processComp(int* retBty, Barcontainer<T>* item = nullptr);
		void processHole(int* retBty, Barcontainer<T>* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);

		bc::Baritem<T>* getBarcode();
		void processTypeF(const barstruct& str, const bcBarImg& img, Barcontainer<T>* item = nullptr);
		void processFULL(const barstruct& str, const BarImg<T>& img, bc::Barcontainer<T>* item);
		void addItemToCont(Barcontainer<T>* item);

		void reverseCom(std::unordered_map<COMPP, barline<T>*>& graph);

		void computeBettyBarcode(Baritem<T>* lines);
		void computeNdBarcode(Baritem<T>* lines, int n);

	public:
		BarcodeCreator()
		{
		}

		bc::Barcontainer<T>* createBarcode(bcBarImg& img, const BarConstructor<T> &structure);

		[[deprecated]]
		bc::Barcontainer<T>* createBarcode(bcBarImg& img, const std::vector<barstruct>& structure);
		
		[[deprecated]]
		bc::Barcontainer<T>* createBarcode(bcBarImg& img, const barstruct* structure, int size);

		[[deprecated]]
		bc::Barcontainer<T>* createSLbarcode(const bcBarImg& src, T foneStart, T foneEnd, Barcontainer<T>* cont = nullptr);

		[[deprecated]]
		bc::Barcontainer<T>* createSLbarcode(const bcBarImg& src, T foneStart, T foneEnd, bool createRGBbar);
#ifdef _PYD
		bc::Barcontainer<T>* createBarcode(bn::ndarray& img, bp::list& structure);
#endif // _PYD

		bc::Barcontainer<T>* searchHoles(float* img, int wid, int hei);

		virtual ~BarcodeCreator()
		{
			if (workingImg != nullptr && needDelImg)
				delete workingImg;

			clearIncluded();
#ifdef USE_OPENCV
			colors.clear();
#endif // USE_OPENCV
		}
	};
}
