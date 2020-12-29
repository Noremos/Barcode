#pragma once
#include "Hole.h"
#include "barcode.h"
namespace bc {

class EXPORT barcodeCreator
{
public:

    //std::vector<int> b0;
    //std::vector<int> b1;
    int b0[256];
    int b1[256];

    int bLen = 256;
private:
    //cv::Mat image;
    int wid;
    int hei;
    int lastB0;
    int lastB1;
    int lll3 = 0;
    int lll4 = 0;
    Component** included;//координаты и сссылка на ребро, которому принадлежит координата

    //попытаться доавть в дыру незадействованные точки
    Hole* createProcessHole(point p);
    //***************************************************

    int GETPOFF(point p) const {
        return wid * p.y + p.x;
    }

    constexpr int GETOFF(int x, int y) const {
        return wid * y + x;
    }

    //#define GETPOFF(P) (this->wid*P.y+P.x)
    //#define GETOFF(X, Y) (this->wid*y+x)

    bool isContain(int x, int y) const
    {
        if (x < 0 || y < 0 || x >= wid || y >= hei)
        {
            return false;
        }
        return included[wid * y + x] != nullptr;
    }

    bool isContain(point p, bool valid) const
    {
        if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
        {
            return false;
        }
        return (included[wid * p.y + p.x] != nullptr && ((Hole*)included[wid * p.y + p.x])->isValid == valid);
    }

    bool isContain(point p)
    {
        if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
        {
            return false;
        }
        return included[wid * p.y + p.x] != nullptr;
    }
    Component* getComp(point p) const
    {
        return  included[wid * p.y + p.x];
    }

    void setInclude(int x, int y, Component* comp)
    {
        included[wid * y + x] = comp;
    }

    void setInclude(point p, Component* comp)
    {
        included[wid * p.y + p.x] = comp;
    }

    Component* getComp(int x, int y) const
    {
        return  included[wid * y + x];
    }
    Hole* getHole(point p) const
    {
        return  (Hole*)included[wid * p.y + p.x];
    }
    Hole* getHole(int x, int y) const
    {
        return  (Hole*)included[wid * y + x];
    }

    Hole* tryAttach(Hole* h1, Hole* h2, point p);
    Component* attach(Component* first, Component* second);
    //Образовала новый?
    bool checkCloserB0(const int x, const int y);

    bool checkCloserB1(const int x, const int y);

    inline bool tryAdd(bc::Hole* h, point& p);

    inline void add(bc::Hole* h, point& p);

    static point* sort(cv::Mat* arr, size_t* totalSize);
    int coords_len = 0;

    void clearIncluded(int len);
    void clear();
    barcode *createSLbarcode(const cv::Mat &src, uchar foneStart, uchar foneEnd);
    barcodeTwo *createTwoSlbarcode(cv::Mat src);
    void checkB1(bc::Hole* h, bool outDo);

    void draw(std::string name = "test");
public:

    barcodeCreator();
    //********************B1************************************************//

    //соединяет дыры и  изменяет holeCollaps
    //**************************************************************************//
    /// \brief createTwoSlbarcode
    /// \param src
    /// \param createRGBbar
    /// \return barcodeTwo with barcodeRBG if(bool par is true and src.channels()==3) else barcode
    barcodeTwo *createTwoSlbarcode(const cv::Mat &src,bool createRGBbar);
    barbase *createSLbarcode(const cv::Mat &src, uchar foneStart, uchar foneEnd,bool createRGBbar);
    barcode *getSLbarcode(int* points);
    //************************
    virtual ~barcodeCreator();
};
}


