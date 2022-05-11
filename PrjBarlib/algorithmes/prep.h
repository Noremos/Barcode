#pragma once
#include "barcodeCreator.h"

typedef bc::BarImg Bimg8;
typedef bc::BarMat Bmat8;
typedef bc::DatagridProvider Bbase8;

using std::string;



Barscalar sqr(Barscalar val)
{
	return val * val;
}
