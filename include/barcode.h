#ifndef BARCODE_H
#define BARCODE_H
#include <vector>
#include <opencv2/opencv.hpp>
#include "point.h"

#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif _STL
#  define EXPORT __declspec(dllimport)
#else
# define EXPORT
#endif

using uchar = unsigned char;
namespace bc {

struct EXPORT bline
{
    //    cv::Mat binmat;
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
    pmap* matr = nullptr;
	float start;
	float len;
    //    bline(uchar _start, uchar _len) :binmat(0,0,CV_8UC1), start(_start), len(_len) {}
    //    bline(uchar _start, uchar _len, cv::Mat _mat) :  start(_start), len(_len)
    //    {
    //        binmat = _mat;
    //    }
    bline() : start(0), len(0), matr(nullptr) {}
	bline(float _start, float _len) : matr(nullptr), start(_start), len(_len) {}
	bline(float _start, float _len, pmap *_mat) : start(_start), len(_len) {
		matr = _mat;
	}
    ~bline()
    {
        if(matr!=nullptr)
            delete matr;
    }
    bline *clone()
    {
        auto temp = new bline(start, len, nullptr);
        if (matr != nullptr)
        {
            temp->matr = new pmap();
            temp->matr->insert(temp->matr->begin(), matr->begin(),matr->end());
        }
        return temp;
    }
	cv::Rect getRect()
	{
		pmap &points = (*matr);
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
enum class EXPORT CompireFunction { CommonToSum, CommonToLen, FromMethod, Complex, Temp1, Temp2 };

class EXPORT Barbase
{
public:
    virtual void removePorog(uchar const porog) = 0;
    virtual void preprocessBar(int const &porog, bool normalize) = 0;
    virtual float compireCTML(const Barbase *Y) const = 0;
    virtual float compireCTS(Barbase const *Y) const = 0;
    virtual Barbase* clone() const= 0;
    virtual int sum() const=0;
    virtual void relen()=0;
//    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
    virtual ~Barbase();
    static float compireBarcodes(const Barbase *X, const Barbase *Y,const CompireFunction &type);
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
