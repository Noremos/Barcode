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
	template<class T, uint N>
	struct EXPORT barVector;
	struct barRGB;
	struct barBGR;
};
using barvec3b = bc::barVector<uchar, 3>;

#define INIT_TEMPLATE_TYPE(TYPE) \
template class TYPE<uchar>; \
template class TYPE<float>; \
template class TYPE<int>; \
template class TYPE<short>; \
template class TYPE<ushort>; \
template class TYPE<barvec3b>;
//template class TYPE<bc::barBGR>; \
//template class TYPE<bc::barRGB>;


#define INIT_TEMPLATE_STRUCT(TYPE) \
template struct TYPE<uchar>; \
template struct TYPE<float>; \
template struct TYPE<int>; \
template struct TYPE<short>; \
template struct TYPE<ushort>; \
template struct TYPE<barvec3b>;
//template struct TYPE<bc::barRGB>; \
//template struct TYPE<bc::barBGR>;



namespace bc
{
	enum class CompireStrategy { CommonToSum, CommonToLen, compire3dHist, compire3dBrightless, FromMethod, Complex, Temp1, Temp2 };

	enum class ComponentType { Component, Hole, RadiusComp, FullPrepair, PrepairComp };
	enum class ProcType { f0t255, f255t0, invertf0, experiment };
	enum class ColorType { gray, rgb, native };

	enum class ReturnType { barcode2d, barcode3d, barcode3dold };
	enum class AttachMode { firstEatSecond, secondEatFirst, createNew, dontTouch, morePointsEatLow };
	enum class ProcessStrategy { brightness, radius };// classic or geometry

	enum class BarType { bc_byte, bc_float, bc_int, bc_short, bc_ushort };
}
