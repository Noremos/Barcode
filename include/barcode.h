#ifndef BARCODE_H
#define BARCODE_H
#include <vector>
#include "point.h"
#include "barImg.h"

#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif _STL
#  define EXPORT __declspec(dllimport)
#else
# define EXPORT
#endif

using uchar = unsigned char;
namespace bc {

struct BarRect
{
    int _x, _y, _wid, _hei;
public:
    BarRect(int x, int y, int width, int height)
    {
        _x = x;
        _y = y;
        _wid = width;
        _hei = height;
    }
};

template<class T>
struct EXPORT bline
{

    bool dropChildes = true;
    //    cv::Mat binmat;
#ifdef USE_OPENCV


    cv::Mat getMat(const cv::Size& s)
    {
        cv::Mat m = cv::Mat::zeros(s.height, s.width, CV_8UC1);
        for (auto it = matr->begin(); it != matr->end(); ++it) {
            m.at<uchar>(it->first.y, it->first.x) = it->second;
        }
        return m;
    }
    void setFromMat(cv::Mat &mat)
    {
        matr->clear();
        mat.forEach<uchar>([m = matr](uchar &pixel, const int *pos) -> void {
            m->push_back(ppair(bc::point(pos[0], pos[1]), pixel)); });
    }
    cv::Rect getRect()
    {
        pmap& points = (*matr);
        int l, r, t, d;
        r = l = points[0].first.x;
        t = d = points[0].first.y;
        for (int j = 0; j < points.size(); ++j)
        {
            if (l > points[j].first.x)
                l = points[j].first.x;
            if (r < points[j].first.x)
                r = points[j].first.x;

            if (t > points[j].first.y)
                t = points[j].first.y;
            if (d < points[j].first.y)
                d = points[j].first.y;
        }
        return cv::Rect(l, t, r - l + 1, d - t + 1);
    }
#endif // USE_OPENCV
    
    bc::BarImg<T> getBarImg(const int wid, int hei)
    {
        BarImg<T> bc(wid, hei);
        for (auto it = matr->begin(); it != matr->end(); ++it) {
            bc.set(it->first.y, it->first.x, it->second);
        }
        return bc;
    }
    void setFromBarImg(const bc::BarImg<T>& mat)
    {
        matr->clear();
        size_t pos = 0;
        for (size_t i = 0; i < mat.getLiner(); i++)
            matr->push_back(ppair<T>(mat.getPointAt(i), mat.getLiner(i)));
    }

    BarRect getRect()
    {
        pmap<T>& points = (*matr);
        int l, r, t, d;
        r = l = points[0].first.x;
        t = d = points[0].first.y;
        for (int j = 0; j < points.size(); ++j)
        {
            if (l > points[j].first.x)
                l = points[j].first.x;
            if (r < points[j].first.x)
                r = points[j].first.x;

            if (t > points[j].first.y)
                t = points[j].first.y;
            if (d < points[j].first.y)
                d = points[j].first.y;
        }
        return BarRect(l, t, r - l + 1, d - t + 1);
    }
    void addCoord(const point& first, T bright)
    {
        matr->push_back(ppair<T>(first, bright));
    }
    pmap<T>* matr = nullptr;
    T start;
    T len;
    barcounter* bar3d = nullptr;
    //    bline(uchar _start, uchar _len) :binmat(0,0,CV_8UC1), start(_start), len(_len) {}
    //    bline(uchar _start, uchar _len, cv::Mat _mat) :  start(_start), len(_len)
    //    {
    //        binmat = _mat;
    //    }
    bline() : start(0), len(0), matr(nullptr) {}
	bline(T _start, T _len) : matr(nullptr), start(_start), len(_len) {}
	bline(T _start, T _len, pmap<T> *_mat) : start(_start), len(_len) {
		matr = _mat;
	}
    bline(T _start, T _len, pmap<T>* _mat, barcounter* _barc) : start(_start), len(_len) {
        matr = _mat;
        bar3d = _barc;
    }
    ~bline()
    {
        if(matr!=nullptr)
            delete matr;
        if (dropChildes)
        {
            for (int i = 0; i < childrens.size(); ++i)
            {
                delete childrens[i];
            }
        }
        if (parent)
        {
            parent->childrens[numInParet] = nullptr;
        }

    }
    bline *clone()
    {
        auto temp = new bline(start, len, nullptr);
        if (matr != nullptr)
        {
            temp->matr = new pmap<T>();
            temp->matr->insert(temp->matr->begin(), matr->begin(),matr->end());
        }

        if (bar3d != nullptr)
        {
            temp->bar3d = new barcounter();
            temp->bar3d->insert(temp->bar3d->begin(), bar3d->begin(), bar3d->end());
        }
        return temp;
    }

    //bc::Component *comp;
    bline<T>* parent = nullptr;
    std::vector<bline<T>*> childrens;
    size_t numInParet = 0;
    void setParrent(bline<T>* node)
    {
        numInParet = node->childrens.size();
        node->childrens.push_back(this);
        this->parent = node;
    }

    T end() const
    {
        return start + len;
    }

#ifdef _PYD
    bp::dict getPoints()
	{
        bp::dict pydict;

        if (matr != nullptr)
        {
            for (auto iter = matr->begin(); iter != matr->end(); ++iter)
                pydict[iter->first] = iter->second;
        }

        return pydict;
    }
#endif // _PYD

};

template<class T>
class EXPORT Barbase
{
public:
    virtual void removePorog(T const porog) = 0;
    virtual void preprocessBar(T const &porog, bool normalize) = 0;
    virtual float compireCTML(const Barbase<T> *Y) const = 0;
    virtual float compireCTS(Barbase<T> const *Y) const = 0;
    virtual Barbase<T>* clone() const= 0;
    virtual double sum() const=0;
    virtual void relen()=0;
//    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
    virtual ~Barbase();
    static float compireBarcodes(const Barbase<T> *X, const Barbase<T> *Y,const CompireFunction &type);
};

//class EXPORT Barcode : public Barbase
//{
//public:
//    std::vector<bc::bline> bar;
//
//    Barcode();
//    Barcode(const Barcode &obj);
//
//    void add(uchar st, uchar len);
//    int sum() const;
//    void relen();
//
//    Barcode *clone() const;
//    uchar maxLen() const;
//    size_t count();
//
//    void removePorog(uchar const porog);
//    void preprocessBar(int const &porog, bool normalize);
//
//    float compireCTML(const Barbase *bc) const;
//    float compireCTS(Barbase const *bc) const;
//    //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
//    ~Barcode();
//};

//typedef barcontainer<1> barcode;
//typedef barcontainer<2> barcodeTwo;
//typedef barcontainer<3> barcodeRGB;
//typedef barcontainer<6> barcodeTwoRGB;
}

#endif // BARCODE_H
