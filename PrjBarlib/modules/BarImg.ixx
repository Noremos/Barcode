module;
#define SKIP_M_INC
#include "../include/presets.h"

#include <iterator>
#include <cassert>
#include <cstring>

#include "../include/include_cv.h"
#include "../include/include_py.h"

export module BarImgModule;
import BarTypes;

#include "../include/barImg.h"

export namespace bc
{
    BarImg;
}