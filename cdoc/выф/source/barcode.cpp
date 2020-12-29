#include "barcode.h"
#include <math.h>
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

bc::barcode::barcode()
{

}

bc::barcode::barcode(const bc::barcode &obj)
{
    bar.insert(bar.begin(), obj.bar.begin(), obj.bar.end());
}

void bc::barcode::add(uchar st, uchar len)
{
    bar.push_back(bline(st, len));
}

int  bc::barcode::sum() const
{
    int sum = 0;
    for (const bline &l : bar)
        sum += l.len;
    return sum;
}

bc::barcode* bc::barcode::clone() const
{
    barcode* nb=new barcode();
    nb->bar.insert(nb->bar.begin(), bar.begin(), bar.end());
    return nb;
}

uchar bc::barcode::maxLen() const
{
    uchar max = 0;
    for (const bline &l : bar)
        if (l.len > max)
            max = l.len;
    return max;
}

void bc::barcode::relen()
{
    if (bar.size() == 0)
        return;
    uchar mini = bar[0].start;
    for (size_t i = 1; i < bar.size(); ++i)
        if (bar[i].start < mini)
            mini = bar[i].start;

    for (size_t i = 0; i < bar.size(); ++i)
        bar[i].start -= mini;

    //mini = std::min_element(arr.begin(), arr.end(), [](bline &b1, bline &b2) { return b1.start < b2.start; })->start;
    //std::for_each(arr.begin(), arr.end(), [mini](bline &n) {return n.start - uchar(mini); });
}

void bc::barcode::removePorog(const uchar porog)
{
    if (porog == 0)
        return;
    barcode res;
    for (const bline &line : bar) {
        if (line.len >= porog)
            res.bar.push_back(line);
    }
    bar.clear();
    bar.insert(bar.begin(), res.bar.begin(), res.bar.end());
}

void bc::barcode::preprocessBar(const int &porog, bool normalize)
{
    if (porog > 0)
        this->removePorog((uchar)roundf((porog*float(this->maxLen()) / 100.f)));

    if (normalize)
        this->relen();
}


float bc::barcode::compireCTML(const bc::barbase *bc) const
{
    barcode *Y = dynamic_cast<const barcode *>(bc)->clone();
    barcode *X = clone();
    if (X->bar.size() == 0 || Y->bar.size() == 0)
        return 0;
    float sum = (float) (X->sum() + Y->sum());
    int n = static_cast<int>(MIN(bar.size(), Y->bar.size()));

    float tsum = 0.f;
    for (int re = 0; re < n; ++re) {
        float maxCoof = 0;
        float maxsum = 0;
        int ik = 0;
        int jk = 0;
        for (int i = 0, total = X->bar.size(); i < total; ++i)
        {
            for (int j = 0, total = Y->bar.size(); j < total; ++j) {
                short st = MAX(X->bar[i].start, Y->bar[j].start);
                short ed = MIN(X->bar[i].start + X->bar[i].len, Y->bar[j].start + Y->bar[j].len);
                float minlen = (float) (ed - st);
                float maxlen = MAX(X->bar[i].len, Y->bar[j].len);
                //Если меньше 0, значит линии не пересекаются
                if (minlen <= 0 || maxlen <= 0)
                    continue;

                float coof = minlen / maxlen;
                if (coof > maxCoof) {
                    maxCoof = coof;
                    maxsum = (float) (X->bar[i].len + Y->bar[j].len);
                    ik = i;
                    jk = j;
                }
            }
        }
        X->bar.erase(X->bar.begin() + ik);
        Y->bar.erase(Y->bar.begin() + jk);
        tsum += (maxsum / sum) * maxCoof;
    }
    return tsum;
}

float bc::barcode::compireCTS(const bc::barbase *bc) const
{
    barcode *Y = dynamic_cast<const barcode *>(bc)->clone();
    barcode *X = clone();
    if (X->bar.size() == 0 || Y->bar.size() == 0)
        return 0;
    float sum = (float) (X->sum() + Y->sum());
    int n = static_cast<int>(MIN(X->bar.size(), Y->bar.size()));

    float tsum = 0.f;
    for (int re = 0; re < n; ++re) {
        float maxCoof = 0;
        float maxsum = 0;
        int ik = 0;
        int jk = 0;
        for (int i = 0, total = X->bar.size(); i < total; ++i)
        {
            for (int j = 0, total2 = Y->bar.size(); j < total2; ++j) {
                short st = MAX(X->bar[i].start, Y->bar[j].start);
                short ed = MIN(X->bar[i].start + X->bar[i].len, Y->bar[j].start + Y->bar[j].len);
                float minlen = (float) (ed - st);

                st = MIN(X->bar[i].start, Y->bar[j].start);
                ed = MAX(X->bar[i].start + X->bar[i].len, Y->bar[j].start + Y->bar[j].len);
                float maxlen = (float) (ed - st);

                //Если меньше 0, значит линии не пересекаются
                if (minlen <= 0 || maxlen <= 0)
                    continue;

                float coof = minlen / maxlen;
                if (coof > maxCoof) {
                    maxCoof = coof;
                    maxsum = (float) (X->bar[i].len + Y->bar[j].len);
                    ik = i;
                    jk = j;
                }
            }
        }
        X->bar.erase(X->bar.begin() + ik);
        Y->bar.erase(Y->bar.begin() + jk);
        tsum += (maxsum / sum) * maxCoof;
    }
    return tsum;
}

bc::barcode::~barcode()
{
    bar.clear();
}

//=======================barcodeTwo=====================

bc::barcodeTwo::barcodeTwo()
{

}

bc::barcodeTwo::barcodeTwo(bc::barbase *one, bc::barbase *two, bool allocateNewpointers)
{
    init(one, two,allocateNewpointers);
}

bc::barcodeTwo::barcodeTwo(const bc::barcodeTwo &obj)
{
    one = obj.one->clone();
    two = obj.two->clone();
}

void bc::barcodeTwo::init(bc::barbase* one, bc::barbase* two, bool allocateNewpointers)
{
    if (this->one != nullptr)
        delete one;
    if (this->two != nullptr)
        delete two;
    if(allocateNewpointers)
    {
        this->one = one->clone();
        this->two = two->clone();
    } else {
        this->one = one;
        this->two = two;
    }
}

bc::barcodeTwo *bc::barcodeTwo::clone() const
{
    return new barcodeTwo(one,two,true);
}

int bc::barcodeTwo::sum() const
{
    return one->sum() + two->sum();
}

void bc::barcodeTwo::preprocessBar(const int &porog, bool normalize)
{
    one->preprocessBar(porog, normalize);
    two->preprocessBar(porog, normalize);
}

float bc::barcodeTwo::compireCTML(const bc::barbase *bc) const
{
    const barcodeTwo *b2 = dynamic_cast<const barcodeTwo*>(bc);
    float r1 = one->compireCTML(b2->one);
    float r2 = two->compireCTML(b2->two);
    int summ1 = one->sum() + b2->one->sum();
    int summ2 = two->sum() + b2->two->sum();
    float procWhite = ((float)summ1 / (summ1 + summ2));
    return r1 * procWhite + r2 * (1-procWhite);
}

float bc::barcodeTwo::compireCTS(const barbase *bc) const
{
    const barcodeTwo *b2 = dynamic_cast<const barcodeTwo *>(bc);

    float r1 = one->compireCTS(b2->one);
    float r2 = two->compireCTS(b2->two);
    int summ1 = one->sum() + b2->one->sum();
    int summ2 = two->sum() + b2->two->sum();
    float procWhite = ((float)summ1 / (summ1 + summ2));
    return r1 * procWhite + r2 * (1-procWhite);
}

void bc::barcodeTwo::removePorog(const uchar porog)
{
    one->removePorog(porog);
    two->removePorog(porog);
}

bc::barcodeTwo::~barcodeTwo()
{
    if(one!=nullptr)
    {
        delete one;
        one = nullptr;
    }

    if(two!=nullptr)
    {
        delete two;
        two = nullptr;
    }
}

void bc::barcodeTwo::relen()
{
    one->relen();
    two->relen();
}

//====================================================

bc::barbase::~barbase() {}

float bc::barbase::compireBarcode(const bc::barbase *X, const bc::barbase *Y, const CompireFunction &type)
{
    switch (type) {
    case CompireFunction::CommonToLen:
        return X->compireCTML(Y);
        break;
    case CompireFunction::CommonToSum:
        return X->compireCTS(Y);
        break;
    default:
        return 0;
        //X->compireCTML(Y);
        break;
    }
}
/////////////////////////////////////////////////////////
//=================================barocdeTGB

bc::barcodeRGB* bc::barcodeRGB::clone() const
{
    return new barcodeRGB(barR, barG, barB,true);
}

bc::barcodeRGB::barcodeRGB() {}

bc::barcodeRGB::barcodeRGB(bc::barbase *r, bc::barbase *g, bc::barbase *b, bool allocateNewpointers)
{
    init(r, g, b,allocateNewpointers);
}

bc::barcodeRGB::barcodeRGB(const bc::barcodeRGB &obj)
{
    this->barR = obj.barR->clone();
    this->barG = obj.barG->clone();
    this->barB = obj.barB->clone();
}

void bc::barcodeRGB::init(bc::barbase *r, bc::barbase *g, bc::barbase *b, bool allocateNewpointers)
{
    if(barR!=nullptr)
        delete barR;

    if (barG != nullptr)
        delete barG;

    if(barB!=nullptr)
        delete barB;

    if (allocateNewpointers) {
        this->barR = r->clone();
        this->barG = g->clone();
        this->barB = b->clone();
    } else {
        this->barR = r;
        this->barG = g;
        this->barB = b;
    }
}


void bc::barcodeRGB::preprocessBar(const int &porog, bool normalize)
{
    this->barR->preprocessBar(porog, normalize);
    this->barG->preprocessBar(porog, normalize);
    this->barB->preprocessBar(porog, normalize);
}

float bc::barcodeRGB::compireCTML(const bc::barbase *bc) const
{
    const barcodeRGB *Y = dynamic_cast<const barcodeRGB *>(bc);
    float r0 = barR->compireCTML(Y->barR);
    float r1 = barG->compireCTML(Y->barG);
    float r2 = barB->compireCTML(Y->barB);
    int sum0 = (barR->sum() + Y->barR->sum());
    int sum1 = (barG->sum() + Y->barG->sum());
    int sum2 = (barB->sum() + Y->barB->sum());
    float tsum = (float) (sum0 + sum1 + sum2);
    return r0 * (sum0 / tsum) + r1 * (sum1 / tsum) + r2 * (sum2 / tsum);
}

float bc::barcodeRGB::compireCTS(const bc::barbase *bc) const
{
    const barcodeRGB *Y = dynamic_cast<const barcodeRGB *>(bc);
    float r0 = barR->compireCTS(Y->barR);
    float r1 = barG->compireCTS(Y->barG);
    float r2 = barB->compireCTS(Y->barB);
    int sum0 = (barR->sum() + Y->barR->sum());
    int sum1 = (barG->sum() + Y->barG->sum());
    int sum2 = (barB->sum() + Y->barB->sum());
    float tsum = (float) (sum0 + sum1 + sum2);
    return r0 * (sum0 / tsum) + r1 * (sum1 / tsum) + r2 * (sum2 / tsum);
}

void bc::barcodeRGB::removePorog(const uchar porog)
{
    this->barR->removePorog(porog);
    this->barG->removePorog(porog);
    this->barB->removePorog(porog);
}

bc::barcodeRGB::~barcodeRGB()
{
    if(barR!=nullptr)
    {

        delete barR;
        barR = nullptr;
    }
    if (barG != nullptr) {
        delete barG;
        barG = nullptr;
    }
    if(barB!=nullptr)
    {
        delete barB;
        barB= nullptr;
    }
}

int bc::barcodeRGB::sum() const
{
    return barR->sum() + barG->sum() + barB->sum();
}

void bc::barcodeRGB::relen()
{
    barR->relen();
    barG->relen();
    barB->relen();
}

