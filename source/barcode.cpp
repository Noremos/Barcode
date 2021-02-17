#include "barcode.h"
#include <math.h>

template<class T>
bc::Barbase<T>::~Barbase() {}

template<class T>
float bc::Barbase<T>::compireBarcodes(const bc::Barbase<T> *X, const bc::Barbase<T> *Y, const CompireFunction &type)
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

INIT_TEMPLATE_TYPE(bc::Barbase)
/////////////////////////////////////////////////////////
