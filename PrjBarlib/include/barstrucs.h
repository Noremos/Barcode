#ifndef BARCODE_H
#define BARCODE_H
#include <vector>
#include <opencv2/opencv.hpp>
#include "barline.h"

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
        void setFromMat(cv::Mat& mat)
        {
            matr->clear();
            mat.forEach<uchar>([m = matr](uchar& pixel, const int* pos) -> void {
                m->insert(ppair(bc::point(pos[0], pos[1]), pixel)); });
        }
        pmap* matr = nullptr;
        uchar start;
        uchar len;
        //    bline(uchar _start, uchar _len) :binmat(0,0,CV_8UC1), start(_start), len(_len) {}
        //    bline(uchar _start, uchar _len, cv::Mat _mat) :  start(_start), len(_len)
        //    {
        //        binmat = _mat;
        //    }
        bline() : start(0), len(0), matr(nullptr) {}
        bline(uchar _start, uchar _len) : matr(nullptr), start(_start), len(_len) {}
        bline(uchar _start, uchar _len, pmap* _mat) : start(_start), len(_len) { matr = _mat; }
        ~bline()
        {
            if (matr != nullptr)
                delete matr;
        }
    };
    enum class CompireFunction { CommonToSum, CommonToLen, FromMethod, Complex, Temp1, Temp2 };

    class EXPORT Barbase
    {
    public:
        virtual void removePorog(uchar const porog) = 0;
        virtual void preprocessBar(int const& porog, bool normalize) = 0;
        virtual float compireCTML(const Barbase* Y) const = 0;
        virtual float compireCTS(Barbase const* Y) const = 0;
        virtual Barbase* clone() const = 0;
        virtual int sum() const = 0;
        virtual void relen() = 0;
        //    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
        virtual ~Barbase();
        static float compireBarcodes(const Barbase* X, const Barbase* Y, const CompireFunction& type);
    };

    class EXPORT Barcode : public Barbase
    {
    public:
        std::vector<bc::bline> bar;

        Barcode();
        Barcode(const Barcode& obj);

        void add(uchar st, uchar len);
        int sum() const;
        void relen();

        Barcode* clone() const;
        uchar maxLen() const;
        size_t count();

        void removePorog(uchar const porog);
        void preprocessBar(int const& porog, bool normalize);

        float compireCTML(const Barbase* bc) const;
        float compireCTS(Barbase const* bc) const;
        //    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
        ~Barcode();
    };

    //typedef barcontainer<1> barcode;
    //typedef barcontainer<2> barcodeTwo;
    //typedef barcontainer<3> barcodeRGB;
    //typedef barcontainer<6> barcodeTwoRGB;

    class EXPORT BarcodeRGB : public Barbase
    {
    public:
        Barbase* barR = nullptr;
        Barbase* barG = nullptr;
        Barbase* barB = nullptr;
        BarcodeRGB* clone() const;
        BarcodeRGB();
        BarcodeRGB(Barbase* r, Barbase* g, Barbase* b, bool allocateNewpointers);
        BarcodeRGB(const BarcodeRGB& obj);
        void init(Barbase* r, Barbase* g, Barbase* b, bool allocateNewpointers);
        void preprocessBar(int const& porog, bool normalize);
        float compireCTML(const Barbase* bc) const;
        float compireCTS(Barbase const* bc) const;

        // barbase interface
        void removePorog(const uchar porog);
        ~BarcodeRGB();

        // barbase interface
        int sum() const;
        void relen();
    };

    class EXPORT BarcodeTwo :public Barbase
    {
    public:
        BarcodeTwo();
        BarcodeTwo(Barbase* one, Barbase* two, bool allocateNewpointers);
        BarcodeTwo(const BarcodeTwo& obj);
        void init(Barbase* one, Barbase* two, bool allocateNewpointers);
        Barbase* one = nullptr;
        Barbase* two = nullptr;
        BarcodeTwo* clone() const;
        int sum() const;
        void preprocessBar(int const& porog, bool normalize);
        float compireCTML(const Barbase* bc) const;
        float compireCTS(Barbase const* bc) const;
        static float fullCompire_MaxLen(BarcodeTwo const& b1, BarcodeTwo const& b2, float proc = 0.5f);
        static float fullCompire_Sum(BarcodeTwo const& b1, BarcodeTwo const& b2, float proc = 0.5f);

        // barbase interface
        void removePorog(const uchar porog);
        ~BarcodeTwo();

        // barbase interface

        void relen();
    };
}
#endif // BARCODE_H
