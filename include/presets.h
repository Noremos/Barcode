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

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

typedef unsigned char uchar;
