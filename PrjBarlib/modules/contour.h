#pragma once
#include "barline.h"
#include "../extra/include_py.h"


namespace bc
{
	struct ContourPoint
	{
		double x;
		double y;

		bool operator==(ContourPoint rhs) const
		{
			return x == rhs.x && y == rhs.y;
		}

		bool operator!=(ContourPoint rhs) const
		{
			return !operator==(rhs);
		}
	};

// #ifdef INCLUDE_PY
	using OutputContour = py::list;
// #else
	// using OutputContour = std::vector<ContourPoint>;
// #endif

	struct Mutator
	{
		double local;
		double global;
		double offset;

		constexpr double convert(double value) const
		{
			return (offset + value * local) * global;
		}
	};

	OutputContour findContour(const bc::barline& line, Mutator x, Mutator y, bool aproximate);
	py::dict convertLasPointsToDict(const bn::array& x, const bn::array& y, const bn::array& z);
}
