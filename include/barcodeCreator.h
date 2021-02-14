#pragma once
#include "hole.h"
#include "barcode.h"
#include "barcontainer.h"
#include "barconstructor.h"


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
		std::vector<COMPP*> components;
		bc::BarNode<T>* graphRoot;
	private:
#ifdef USE_OPENCV
		std::vector<cv::Vec3b> colors;
#endif

		bool reverse = false;
		int b[256];
		
		BarConstructor<T> settings;

		Include<T>* included;
		BarImg<T>* workingImg = nullptr;
		T curbright;
		point curpix;
		int wid;
		int hei;
		int lastB;
		friend class Component<T>;
		friend class Hole<T>;

		size_t totalSize = 0;
		point* sortedArr;

		//***************************************************

		int GETPOFF(const point& p) const;

		constexpr int GETOFF(int x, int y) const {
			return wid * y + x;
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

		void setInclude(int x, int y, COMPP comp, T bright = curbright);
		void setInclude(const point& p, COMPP comp, T bright = curbright);

		COMPP getComp(int x, int y);
		COMPP getComp(const point& p);

		HOLEP getHole(int x, int y);
		HOLEP getHole(const point& p);

		COMPP attach(COMPP first, COMPP second);
		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB0();
		bool checkCloserB1();

		static point* sort(const ProcType& type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISULA_DEBUG(int y, int i);
		void VISULA_DEBUG_COMP(int y, int i);
		void init(const bcBarImg& src, const ProcType& type);

		void processComp(int* retBty, Barcontainer<T>* item = nullptr);
		void processHole(int* retBty, Barcontainer<T>* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);

		void ProcessFullPrepair(int* retBty, Barcontainer<T>* item = nullptr);
		void ProcessPrepComp(int* retBty, Barcontainer<T>* item = nullptr);
		//void processComp255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);
		void addItemToCont(bc::Barcontainer<T>* item);
		bc::Baritem<T>* getBarcode();
		void processTypeF(const barstruct& str, bcBarImg& img, Barcontainer<T>* item = nullptr);
		void processFULL(const barstruct& str, const BarImg<T>& img, bc::Barcontainer<T>* item);


		void Prepair();
	public:
		BarcodeCreator();

		[[deprecated]]
		bc::Barcontainer<T>* createBarcode(bcBarImg& img, BarConstructor<T> structure);

		[[deprecated]]
		bc::Barcontainer<T>* createBarcode(bcBarImg& img, const std::vector<barstruct>& structure);
		
		bc::Barcontainer<T>* createBarcode(bcBarImg& img, const barstruct* structure, int size);

		[[deprecated]]
		bc::Barcontainer<T>* createSLbarcode(const bcBarImg& src, T foneStart, T foneEnd, Barcontainer<T>* cont = nullptr);

		[[deprecated]]
		bc::Barcontainer<T>* createSLbarcode(const bcBarImg& src, T foneStart, T foneEnd, bool createRGBbar);
#ifdef _PYD
		bc::Barcontainer<T>* createBarcode(bn::ndarray& img, bp::list& structure);
#endif // _PYD

		bc::Barcontainer<T>* searchHoles(float* img, int wid, int hei);

		virtual ~BarcodeCreator();

		void computeBettyBarcode(Baritem<T>* lines);
		void computeNdBarcode(Baritem<T>* lines, int n);

	};
}
