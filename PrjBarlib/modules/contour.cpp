#include "contour.h"

#include <set>

inline constexpr int poss[16][2] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1},
						{-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };

namespace bc {


class MapContour
{
	OutputContour& contur;
	ushort accum = 1;

	// Map
	std::set<unsigned int> points;

	// Runtime
	enum StartPos : char { LeftMid = 0, LeftTop = 1, TopMid = 2, RigthTop = 3, RigthMid = 4, RigthBottom = 5, BottomMid = 6, LeftBottom = 7 };

public:
	MapContour(OutputContour& contur) : contur(contur)
	{ }

	void set4Directions()
	{
		accum = 2;
	}

	void set8Directions()
	{
		accum = 1;
	}

	auto make_point(point p) const
	{
		return py::make_tuple(xm.convert(p.x), ym.convert(p.y));
	}

	Mutator xm, ym;
	void run(int startX, int startY, Mutator xm, Mutator ym, const bool aproxim = false)
	{
		if (points.empty())
			return;

		this->xm = xm;
		this->ym = ym;

		StartPos dirct = RigthMid;
		const bc::point startPoint(startX, startY);
		bc::point cur(startPoint);

		while (true)
		{
			ushort start = (dirct + 6) % 8; // Safe minus 2
			const ushort end = start + 7;
			// Check
			// 1 2 3
			// 0 X 4
			//   6 5

			for (; start < end; start += accum)
			{
				const int* off = poss[start];
				const bc::point newPoint(cur.x + off[0], cur.y + off[1]);

				if (this->exists(newPoint))
				{
					// Update dir
					const StartPos newDirection = (StartPos)(start % 8);

					// In case of approximation
					// put only if direction has changed
					if (!aproxim || dirct != newDirection)
					{
						contur.append(make_point(cur));
					}

					dirct = newDirection;
					cur = newPoint;
					if (cur == startPoint)
					{
						auto firstAddedPoint = contur[0].cast<py::tuple>();
						if (firstAddedPoint[0].cast<double>() != startPoint.x || firstAddedPoint[1].cast<double>() != startPoint.y)
						{
							contur.append(make_point(startPoint));
						}
						return;
					}

					break;
				}
			}

			// --------
			// not found assert
			assert(start < end);
			if (start >= end)
				return;
		}
	}

	void set(const bc::barvalue& p) { points.insert(p.getIndex()); }
	void set(int x, int y) { points.insert(bc::barvalue::getStatInd(x, y)); }

private:

	bool exists(bc::point p)
	{
		return points.contains(bc::barvalue::getStatInd(p.x, p.y));
	}
};


OutputContour findContour(const bc::barline& line, Mutator mx, Mutator my, bool aproximate)
{
	OutputContour contour;

	int stX = 99999999;
	int stY = 0;

	MapContour dictPoints(contour);
	dictPoints.set8Directions();

	const auto& points = line.matr;
	for (auto& p : points)
	{
		dictPoints.set(p);
		dictPoints.set(p.x, p.y + 1);
		dictPoints.set(p.x + 1, p.y);
		dictPoints.set(p.x + 1, p.y + 1);
		const int x = p.getX();
		const int y = p.getY();
		if (x < stX)
		{
			stX = x;
			stY = y;
		}
	}
	dictPoints.run(stX, stY, mx, my, aproximate);

	return contour;
}

template<class T>
py::dict convertLasPointsToDictInner(const bn::array& x, const bn::array& y, const bn::array& z)
{
	py::dict output;
	for (size_t i = 0, total = x.shape()[0]; i < total; i++)
	{
		T xv = *reinterpret_cast<const T*>(x.data(i));
		T yv = *reinterpret_cast<const T*>(y.data(i));
		T zv = *reinterpret_cast<const T*>(z.data(i));
		output[py::make_tuple(xv,yv)] = zv;
	}

	return output;
}
py::dict convertLasPointsToDict(const bn::array& x, const bn::array& y, const bn::array& z)
{
	auto mtype = z.dtype();
	if (mtype.is(pybind11::dtype::of<float>()))
		return convertLasPointsToDictInner<float>(x, y, z);
	else if (mtype.is(pybind11::dtype::of<double>()))
		return convertLasPointsToDictInner<double>(x, y, z);
	else if (mtype.is(pybind11::dtype::of<int8_t>()))
		return convertLasPointsToDictInner<int8_t>(x, y, z);
	else if (mtype.is(pybind11::dtype::of<int32_t>()))
		return convertLasPointsToDictInner<int32_t>(x, y, z);
	else if (mtype.is(pybind11::dtype::of<int64_t>()))
		return convertLasPointsToDictInner<int64_t>(x, y, z);
	else
		throw pybind11::type_error("Unsupported data type");
}

}
