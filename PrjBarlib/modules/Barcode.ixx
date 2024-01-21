module;
#define SKIP_M_INC


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


#include <math.h>
#include <stack>
#include <string>
#include <assert.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>
#include <iostream>
#include "presets.h"
#include <unordered_set>
#ifdef __linux
#include <climits>
#endif // __linux


import BarScalarModule;
import BarTypes;
import BarImgModule;
import BarImgModule;

#include "../include/barclasses.h"
#include "../include/component.h"
#include "../include/hole.h"
#include "../include/barcodeCreator.h"

#include "../source/barcodeCreator.cpp"
#include "../source/barclasses.cpp"
#include "../source/component.cpp"
#include "../source/hole.cpp"

export module BarcodeModule;


export namespace bc
{
	Barcontainer;
	Baritem;
	BarcodeCreator;
	using bc::PloyPoints;
	//CloudPointsBarcode;
	indexCov;
}
