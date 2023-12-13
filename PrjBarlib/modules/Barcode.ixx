module;


//EXCLUDE_FILES
//include/presets.h
//include/barline.h
//include/barImg.h
//include/barscalar.h
//include/barstrucs.h
//include/include_cv.h
//include/include_py.h
//END_EXLCUDE

//import std.core;

#include "barline.h"
#include "barImg.h"
#include "barcodeCreator.h"

#include <math.h>
#include <stack>
#include <string>
#include <assert.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>


#ifdef __linux
#include <climits>
#endif // __linux


export module BarcodeModule;
#define SKIP_M_INC

#include "../source/barcodeCreator.cpp"
#include "../source/barclasses.cpp"
#include "../source/component.cpp"
#include "../source/hole.cpp"


typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

#define BAREXPORT

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif


export namespace bc
{
	const unsigned int BAR_MAX_WID = 65535;
	BType;
	BIndex;
	poidex;
	Barscalar;

	point;
	pointHash;
	pmap;

	CachedValue;
	BarConstructor;
	DatagridProvider;
	BarRect;
	barline;
	using bc::BarRoot;

	using bc::barlinevector;
	using bc::barvector;
	using bc::barcounter;
	using bc::PloyPoints;
	barvalue;
	CachedValue;

	barstruct;

	CompireStrategy;
	ComponentType;
	ProcType;
	ColorType;
	ReturnType;
	ReturnType;
	AttachMode;
	ProcessStrategy;

	BarRect;
	bc::BarRect getBarRect(const bc::barvector& matrix)
	{
		int l, r, t, d;
		r = l = matrix[0].getX();
		t = d = matrix[0].getY();
		for (size_t j = 0; j < matrix.size(); ++j)
		{
			if (l > matrix[j].getX())
				l = matrix[j].getX();
			if (r < matrix[j].getX())
				r = matrix[j].getX();

			if (t > matrix[j].getY())
				t = matrix[j].getY();
			if (d < matrix[j].getY())
				d = matrix[j].getY();
		}
		return bc::BarRect(l, t, r - l + 1, d - t + 1);
	}

	Barcontainer;
	BarcodeCreator;
	Baritem;
	CloudPointsBarcode;
}
