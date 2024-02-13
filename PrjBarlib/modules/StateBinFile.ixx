module;

#include <bitset>
#include <fstream>
#include <functional>
#include <sstream>
#include <cassert>


#include "../include/barscalar.h"
// #include "BinFile.h"

export module StateBinIO;

// import BarScalarModule;

#define SKIP_M
#undef MEXP
#define MEXP export
#include "StateBinFile.h"
