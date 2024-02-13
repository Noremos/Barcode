module;
#include <exception>
#include <stdio.h>
#include <string>
#include <vector>
#include <queue>

#include "sidesrc/flat_hash_map/unordered_map.hpp"

#ifndef QSLOT_CODE
#include <iostream>
#define OUT std::cout
#else
#include <QDebug>
#define OUT qDebug()
#endif // !_QT

#include <stdio.h>

#include <algorithm>

#include <string>
#include <cassert>
#include "sidesrc/fast_float.h"

#include "base.h"
#include <queue>
#include "sidesrc/flat_hash_map/unordered_map.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include "base.h"
#include <cstring>
#include <bit>

export module ImgReader;

#define SKIP_M_INC
#define MEXP export
#include "ImageReader.h"
#include "convertion.h"
#include "decoder.h"
#include "tiffreader.h"
#include "tiffreader.cpp"
//#include "convertion.cpp"


export int getReaderTypeSize(ImageType type)
{
	return getImgTypeSize(type);
}
