#pragma once
//#define USE_OPENCV
//#define VDEBUG










#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif _STL
#  define EXPORT __declspec(dllimport)
#else
# define EXPORT
#endif

#ifdef _PYD
#define INCLUDE_PY \
#include "boost/python.hpp" \
#include "boost/python/numpy.hpp" \
using namespace boost::python; \
namespace bp = boost::python; \
namespace bn = boost::python::numpy;
#endif // _PYD

#ifdef USE_OPENCV
#define INCLUDE_CV \
#include <opencv2/opencv.hpp> \
#include <opencv2/imgproc.hpp>
using cv::Mat;
#else
#define INCLUDE_CV
#endif // DEBUG

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

typedef unsigned char uchar;

template<class T>
struct CachedValue
{
	T val;
	bool isCached = false;

	void set(T val)
	{
		this->val = val;
		this->isCached = true;
	}

	T getOrDefault(T defValue)
	{
		return isCached ? val : defValue;
	}
};

enum class EXPORT CompireFunction { CommonToSum, CommonToLen, FromMethod, Complex, Temp1, Temp2 };
