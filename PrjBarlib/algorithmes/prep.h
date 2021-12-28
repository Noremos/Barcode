#pragma once
#include "barcodeCreator.h"

typedef bc::BarImg<uchar> Bimg8;
typedef bc::BarMat<uchar> Bmat8;
typedef bc::DatagridProvider<uchar> Bbase8;

using std::string;


template<class T>
T sqr(T val)
{
	return val * val;
}
