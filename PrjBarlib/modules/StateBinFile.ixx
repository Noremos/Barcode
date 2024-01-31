module;
#define SKIP_M

#include <bitset>
#include <fstream>
#include <functional>
#include <sstream>
#include <cassert>


// #include "BinFile.h"

export module StateBinIO;

import BarScalarModule;

#include "StateBinFile.h"

export namespace StateBinFile
{
	BinState;
	BinStateReader;
	BinStateWriter;
}
