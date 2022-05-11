#pragma once
//#define USE_OPENCV
//#define VDEBUG





typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;








#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif defined _STL
#  define EXPORT /*__declspec(dllimport)*/
#else
# define EXPORT
#endif

#ifdef _PYD
#define INCLUDE_PY
#endif // _PYD

#ifdef USE_OPENCV
#define INCLUDE_CV
#endif // DEBUG

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif


namespace bc
{
	struct barRGB;
	struct barBGR;
};

namespace bc
{
	enum class CompireStrategy { CommonToSum, CommonToLen, compire3dHist, compire3dBrightless, FromMethod, Complex, Temp1, Temp2 };

	enum class ComponentType { Component, Hole, RadiusComp, FullPrepair, PrepairComp };
	enum class ProcType { f0t255, f255t0, invertf0, experiment };
	enum class ColorType { gray, rgb, native };

	enum class ReturnType { barcode2d, barcode3d, barcode3dold };
	enum class AttachMode { firstEatSecond, secondEatFirst, createNew, dontTouch, morePointsEatLow };
	enum class ProcessStrategy { brightness, radius };// classic or geometry
}
