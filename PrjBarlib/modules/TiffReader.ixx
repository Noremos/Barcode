module;

#define SKIP_M_INC
#include "tiffreader.h"
#include "tiffreader.cpp"
//#include "convertion.cpp"

export module ImgReader;

export ImageReader;
export ImageType;
export SubImgInfo;
export DataRect;
export rowptr;
export roweltype;
export int getReaderTypeSize(ImageType type)
{
	return getImgTypeSize(type);
}

export Tags;
export tifftype;
export TiffTags;
export GeoTiffTags;
export TiffReader;

