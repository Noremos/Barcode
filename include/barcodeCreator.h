#pragma once
#include "hole.h"
#include "barcode.h"
#include "barcontainer.h"
#include "../barconstructor.h"




namespace bc {

	class EXPORT barcodeCreator
	{
	public:
		std::vector<Component*> components;
		bc::BarNode *graphRoot;
	private:
		bool reverse = false;
		bool createGraph = true;
		int b[256];
		bool visualize = false, createBin = false, useBetty = true, getCoords = true;
		std::vector<cv::Vec3b> colors;
		Component **included;
		uchar* matrix;
		float curbright;
		point curpix;
		int wid;
		int hei;
		int lastB;
		uchar settStep = 1;
		friend class Component;
		friend class Hole;

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

		void setInclude(int x, int y, Component* comp);
		void setInclude(const point& p, Component* comp);

		Component* getComp(int x, int y);
		Component* getComp(const point& p);

		Hole* getHole(int x, int y);
		Hole* getHole(const point& p);

		Component* attach(Component* first, Component* second);
		Hole* tryAttach(Hole* h1, Hole* h2, point p);

		bool checkCloserB0();
		bool checkCloserB1();

		static point* sort(const cv::Mat &arr,const ProcType& type);

		void clearIncluded();

		void draw(std::string name = "test");
		void VISULA_DEBUG(int y, int i);
		void VISULA_DEBUG_COMP(int y, int i);
		void init(const cv::Mat& src, const ProcType &type, cv::Mat& img);

        void processHole(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);
		void processHole255to0(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);

		void processComp(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);
		void ProcessFullPrepair(cv::Mat& img, int *retBty, Barcontainer* item = nullptr);
		void ProcessPrepComp(cv::Mat& img, int *retBty, int step, Barcontainer* item = nullptr);
		void processComp255to0(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);
		void addItemToCont(bc::Barcontainer* item);
		bc::Baritem* getBarcode();
		void processTypeF(const barstruct& str, cv::Mat& img, Barcontainer* item = nullptr);
		void processFULL(const barstruct& str, cv::Mat& img, bc::Barcontainer* item);
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

		bc::Barcontainer* createBarcode(cv::Mat img, const std::vector<barstruct>& structure);

		bc::Barcontainer* createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd, Barcontainer* cont = nullptr);

		bc::Barcontainer* createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd, bool createRGBbar);
#ifdef _PYD
		bc::Barcontainer* createBarcode(bn::ndarray& img, bp::list& structure);
#endif // _PYD

		bc::Barcontainer* searchHoles(float *img, int wid, int hei);

		virtual ~barcodeCreator();

		void Prepair(cv::Mat, int step);
		uchar GetStep() const;
		void SetStep(const uchar value);
	};
}
