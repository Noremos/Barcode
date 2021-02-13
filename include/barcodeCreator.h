#pragma once
#include "hole.h"
#include "barcode.h"
#include "barcontainer.h"
#include "barconstructor.h"


#define COMPP Component<T>*
#define HOLEP Hole<T>*

namespace bc {
	 
	template<class T>
	class EXPORT barcodeCreator
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
		bool createGraph = true;
		int b[256];
		bool visualize = false, createBin = false, useBetty = true, getCoords = true;

		bc::Component<T>** included;
		T curbright;
		point curpix;
		int wid;
		int hei;
		int lastB;
		uchar settStep = 1;
		friend class Component<T>;
		friend class Hole<T>;

		size_t totalSize = 0;
		point* sortedArr;

		//***************************************************

		int GETPOFF(const point& p) const;

		constexpr int GETOFF(int x, int y) const {
			return wid * y + x;
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

		COMPP getComp(int x, int y);
		COMPP getComp(const point& p);

		HOLEP getHole(int x, int y);
		HOLEP getHole(const point& p);

		COMPP attach(COMPP first, COMPP second);
		HOLEP tryAttach(HOLEP h1, HOLEP h2, point p);

		bool checkCloserB0();
		bool checkCloserB1();

		static point* sort(const bcBarImg& arr, const ProcType& type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISULA_DEBUG(int y, int i);
		void VISULA_DEBUG_COMP(int y, int i);
		void init(const bcBarImg& src, const ProcType& type, bcBarImg& img);

		void processComp(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);
		void processHole(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);
		//void processHole255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);

		void ProcessFullPrepair(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);
		void ProcessPrepComp(bcBarImg& img, int* retBty, int step, Barcontainer<T>* item = nullptr);
		//void processComp255to0(bcBarImg& img, int* retBty, Barcontainer<T>* item = nullptr);
		void addItemToCont(bc::Barcontainer<T>* item);
		bc::Baritem<T>* getBarcode();
		void processTypeF(const barstruct& str, bcBarImg& img, Barcontainer<T>* item = nullptr);
		void processFULL(const barstruct& str, bcBarImg& img, bc::Barcontainer<T>* item);
	public:
		barcodeCreator();

		inline void setVisualize(bool b)
		{
			visualize = b;
		}
		inline void setCreateBinaryMasks(bool b)
		{
			createBin = b;
		}
		inline void setReturnType(int type)
		{
			useBetty = (type == 0);
		}
		inline void setCreateGraph(bool val)
		{
			createGraph = val;
		}

		bc::Barcontainer<T>* createBarcode(bcBarImg& img, const std::vector<barstruct>& structure);

		bc::Barcontainer<T>* createSLbarcode(const bcBarImg& src, T foneStart, T foneEnd, Barcontainer<T>* cont = nullptr);

		bc::Barcontainer<T>* createSLbarcode(const bcBarImg& src, T foneStart, T foneEnd, bool createRGBbar);
#ifdef _PYD
		bc::Barcontainer<T>* createBarcode(bn::ndarray& img, bp::list& structure);
#endif // _PYD

		bc::Barcontainer<T>* searchHoles(float* img, int wid, int hei);

		virtual ~barcodeCreator();

		void Prepair(bcBarImg&, int step);
		uchar GetStep() const;
		void SetStep(const uchar value);
	};
}
