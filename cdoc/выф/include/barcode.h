#ifndef BARCODE_H
#define BARCODE_H
#include <vector>
using uchar = unsigned char;
namespace bc {
struct bline
{
    uchar start;
    uchar len;
    bline(uchar _start, uchar _len) : start(_start), len(_len) {}
};
enum class CompireFunction { CommonToSum, CommonToLen, FromMethod, Complex, Temp1, Temp2 };

class barbase
{
public:
    virtual void removePorog(uchar const porog) = 0;
    virtual void preprocessBar(int const &porog, bool normalize) = 0;
    virtual float compireCTML(const barbase *Y) const = 0;
    virtual float compireCTS(barbase const *Y) const = 0;
    virtual barbase* clone() const= 0;
    virtual int sum() const=0;
    virtual void relen()=0;
//    virtual void fullCompite(barbase const *bc, CompireFunction fn, float poroc = 0.5f) = 0;
    virtual ~barbase();
    static float compireBarcode(const barbase *X, const barbase *Y,const CompireFunction &type);



};

class barcode:public barbase
{
public:
    barcode();
    barcode(const barcode &obj);
    std::vector<bc::bline> bar;
    void add(uchar st, uchar len);
    //    uchar type = 1;
    int sum() const;
    void relen();
    barcode *clone() const;

    uchar maxLen() const;


    void removePorog(uchar const porog);
    void preprocessBar(int const &porog, bool normalize);

    float compireCTML(const barbase *bc) const;
    float compireCTS(barbase const *bc) const;
//    void fullCompite(const barbase *bc, CompireFunction fn, float poroc = 0.5f);
    ~barcode();
};

class barcodeRGB : public barbase
{
public:
    barbase *barR=nullptr;
    barbase *barG=nullptr;
    barbase *barB=nullptr;
    barcodeRGB *clone() const;
    barcodeRGB();
    barcodeRGB(barbase *r, barbase *g, barbase *b,bool allocateNewpointers);
    barcodeRGB(const barcodeRGB &obj);
    void init(barbase *r, barbase *g, barbase *b,bool allocateNewpointers);
    void preprocessBar(int const &porog, bool normalize);
    float compireCTML(const barbase *bc) const;
    float compireCTS(barbase const *bc) const;

    // barbase interface
    void removePorog(const uchar porog);
    ~barcodeRGB();

    // barbase interface
    int sum() const;
    void relen();
};

class barcodeTwo:public barbase
{
public:
    barcodeTwo();
    barcodeTwo(barbase *one, barbase *two,bool allocateNewpointers);
    barcodeTwo(const barcodeTwo &obj);
    void init(barbase* one, barbase* two,bool allocateNewpointers);
    barbase* one=nullptr;
    barbase* two=nullptr;
    barcodeTwo* clone() const;
    int sum() const;
    void preprocessBar(int const &porog, bool normalize);
    float compireCTML(const barbase *bc) const;
    float compireCTS(barbase const *bc) const;
//    static float fullCompire_MaxLen(barcodeTwo const &b1, barcodeTwo const &b2, float proc=0.5f);
//    static float fullCompire_Sum(barcodeTwo const &b1, barcodeTwo const &b2, float proc=0.5f);

    // barbase interface
    void removePorog(const uchar porog);
    ~barcodeTwo();

    // barbase interface

    void relen();
};
}
#endif // BARCODE_H
