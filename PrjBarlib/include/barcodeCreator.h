#pragma once
#include "hole.h"
#include "barclasses.h"
namespace bc {

    class EXPORT barcodeCreator
    {
    public:

        //std::vector<int> b0;
        //std::vector<int> b1;
        int b[256];
        //int bLen = 256;
        cv::Mat barMap;
        bool useCorrectSE = true;
        bool createBin = false;
        std::vector<Component*> components;
        //    std::vector<cv::Mat> .;
    private:
        std::vector<cv::Vec3b> colors;
        //cv::Mat image;
        Component** included;
        uchar curbright;
        int wid;
        int hei;
        int lastB;
        int lll3 = 0;
        int lll4 = 0;
        bool createMap = false;
        friend class Component;
        friend class Hole;
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

        Hole* tryAttach(Hole* h1, Hole* h2, point p);
        Component* attach(Component* first, Component* second);
        //Образовала новый?
        bool checkCloserB0(point midP);

        bool checkCloserB1(point& p);

        inline bool tryAdd(bc::Hole* h, const point& p);

        inline void add(bc::Hole* h, point& p);
        static point* sort(cv::Mat* arr);

        size_t totalSize = 0;

        void clearIncluded();
        void clear();
        Barcode* createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd);
        BarcodeTwo* createTwoSlbarcode(const cv::Mat& src);

        void draw(std::string name = "test");
        void VISULA_DEBUG(int y, int i, point& pix);
        void VISULA_DEBUG_COMP(int y, int i, point& pix);
        void init(const cv::Mat& src, cv::Mat& img);
        point* sortedArr;
        void mapp(bc::Hole* h, const point& p);
        void drawColorMap(int delay = 0);
        void processHole0to255(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);
        void processHole255to0(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);

        void processComp0to255(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);
        void processComp255to0(cv::Mat& img, int* retBty, Barcontainer* item = nullptr);
        void addItemToCont(bc::Barcontainer* item);
        void processTypeF(const barstruct& str, cv::Mat& img, Barcontainer* item = nullptr);
        void processFULL(const barstruct& str, cv::Mat& img, bc::Barcontainer* item);
    public:

        barcodeCreator();
        //********************B1************************************************//

        //соединяет дыры и  изменяет holeCollaps
        //**************************************************************************//
        /// \brief createTwoSlbarcode
        /// \param src
        /// \param createRGBbar
        /// \return barcodeTwo with barcodeRBG if(bool par is true and src.channels()==3) else barcode
        BarcodeTwo* createTwoSlbarcode(const cv::Mat& src, bool createRGBbar);
        Barbase* createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd, bool createRGBbar);
        Barcode* getSLbarcode(int* points);
        bc::Barcontainer* createBarcode(cv::Mat img, const std::vector<barstruct>& structure);
        //************************
        virtual ~barcodeCreator();
        std::vector<std::vector<cv::Point>> segmetbar(cv::Mat src, bool reverse, uchar terminateBright);
        std::vector<std::vector<cv::Point> > segmetbarFull(cv::Mat src, uchar terminateBright);
        void addToBin0();
        void addToBin255();
    };
}


