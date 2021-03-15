#pragma once
//#define USE_OPENCV
//#define VDEBUG





typedef unsigned char uchar;






#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif _STL
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

#define INIT_TEMPLATE_TYPE(TYPE) \
template class TYPE<uchar>; \
template class TYPE<float>;


#define INIT_TEMPLATE_STRUCT(TYPE) \
template struct TYPE<uchar>; \
template struct TYPE<float>;




namespace bc
{
	enum class EXPORT CompireStrategy { CommonToSum, CommonToLen, FromMethod, Complex, Temp1, Temp2 };

	enum class ComponentType { Component, Hole, FullPrepair, PrepairComp };
	enum class ProcType { f0t255, f255t0, experement };
	enum class ColorType { gray, rgb, native };

	enum class ReturnType { betty, barcode2d, barcode3d };
}
