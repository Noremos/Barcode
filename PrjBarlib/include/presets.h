#ifndef SKIP_M_INC
#pragma once
#endif
#define MEXP
// #undef USE_OPENCV
//#define VDEBUG
//#define ENABLE_ENERGY

//#define POINTS_ARE_AVAILABLE


// #include "../side/flat_hash_map/unordered_map.hpp"
#include <unordered_map>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;


template<class TKey, class TVal>
using barmap = std::unordered_map<TKey, TVal>;

template<class TKey, class TVal, class THasher>
using barmapHash = std::unordered_map<TKey, TVal, THasher>;







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
