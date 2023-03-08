module;

#include "barImg.h"

export module BarcodeModule;

namespace bc
{
	class BarcodeCreator
	{
		BarConstructor settings;
		const DatagridProvider* workingImg = nullptr;
		bc::BarImg drawimg;
	};
}

export namespace bc
{
	BarcodeCreator;
}
