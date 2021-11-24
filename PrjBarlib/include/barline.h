#pragma once
#include <unordered_map>
#include <opencv2/opencv.hpp>

#ifdef _WINDLL
#  define EXPORT __declspec(dllexport)
#elif _STL
#  define EXPORT __declspec(dllimport)
#else
# define EXPORT
#endif

namespace bc
{
	struct EXPORT point
	{
		int x = -1;
		int y = -1;
		point()
		{
			x = -1; y = -1;
		}

		point(int x, int y) :x(x), y(y)
		{
		}

		inline void init(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		inline void init(int p[2])
		{
			this->x = p[0];
			this->y = p[1];
		}

		inline cv::Point cvPoint()
		{
			return cv::Point(x, y);
		}



		point operator+(int* xy)
		{
			return point(x + xy[0], y + xy[1]);
		}
		point operator+(char* xy)
		{
			return point(x + xy[0], y + xy[1]);
		}
		point operator*(int c)
		{
			return point(x * c, y * c);
		}
		point operator+(point p)
		{
			return point(x + p.x, y + p.y);
		}
		int operator[](int i)
		{
			return i == 0 ? x : y;
		}
		bool operator==(const point& p) const
		{
			return x == p.x && y == p.y;
		}
		bool operator!=(const point& p) const
		{
			return x != p.x || y != p.y;
		}
	};
	class EXPORT pointHash
	{
	public:
		size_t operator()(const point& p) const
		{
			return ((size_t)p.x << 32) + (size_t)p.y;
		}
	};

	typedef std::unordered_map<point, bool, pointHash> pmap;
	typedef std::pair<point, bool> ppair;
}
