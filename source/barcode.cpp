#include "barcode.h"
#include <math.h>
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

bc::Barbase::~Barbase() {}

float bc::Barbase::compireBarcodes(const bc::Barbase *X, const bc::Barbase *Y, const CompireFunction &type)
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
