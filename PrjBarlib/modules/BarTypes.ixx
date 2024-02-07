module;
#define SKIP_M_INC

#include "../include/include_py.h"
#include "../include/include_cv.h"

#include <math.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <stack>
#include <cassert>
#include <cstring>
#include "../include/presets.h"

export module BarTypes;

import BarScalarModule;

#include "../include/barstrucs.h"
#include "../include/barline.h"

export namespace bc
{
	const unsigned int BAR_MAX_WID = 65535;

	BType;
	BIndex;
	poidex;

	point;
	pointHash;
	pmap;

	CachedValue;
	BarConstructor;
	DatagridProvider;

	BarRect;
	barline;
	BarclinesHolder;
	using bc::BarRoot;

	using bc::barlinevector;
	using bc::barvector;
	using bc::barcounter;
	barvalue;
	CachedValue;
	bar3dvalue;

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
}
