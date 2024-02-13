module;

#include "../include/presets.h"

#include <iterator>
#include <cassert>
#include <cstring>

#include "../include/include_cv.h"
#include "../include/include_py.h"

export module BarImgModule;
import BarTypes;
import BarScalarModule;

#define MEXP
#define SKIP_M_INC
#include "../include/barImg.h"
