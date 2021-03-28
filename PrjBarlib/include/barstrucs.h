#ifndef BARCODE_H
#define BARCODE_H

#include "presets.h"

#include <vector>
#include <cassert>
#include <cstring>

#include "include_cv.h"

namespace bc
{

	template<class T>
	struct EXPORT CachedValue
	{
		T val;
		bool isCached = false;

		void set(T val)
		{
			this->val = val;
			this->isCached = true;
		}

		T getOrDefault(T defValue) const
		{
			return isCached ? val : defValue;
		}
	};

	struct BarRect
	{
		int x, y, width, height;
		BarRect(int _x, int _y, int _width, int _height)
		{
			x = _x;
			y = _y;
			width = _width;
			height = _height;
		}
		int right()
		{
			return x+width;
		}
		int botton()
		{
			return y+height;
		}

		int area()
		{
			return width * height;
		}

		bool isItemInside(BarRect anItem)
		{
			return (anItem.x >= x && anItem.y >= y && anItem.right() <= right() && anItem.botton() <= botton());
		}
	};

	struct EXPORT BarVec3b
	{
	public:
		uchar vls[3];
		uchar r()
		{
			return vls[2];
		}
		uchar g()
		{
			return vls[1];
		}
		uchar b()
		{
			return vls[0];
		}
		int sum()
		{
			return (int)vls[0] + vls[1] + vls[2];
		}

		inline BarVec3b()
		{
			memset(vls, 0, 3);
		}

		inline BarVec3b(uchar _r, uchar _g, uchar _b)
		{
			vls[0] = _b;
			vls[1] = _g;
			vls[2] = _r;
		}
		uchar operator[](std::size_t idx)
		{
			assert(idx <= 2);
			return vls[idx];
		}
	};

	static bool operator > (BarVec3b c1, BarVec3b c2)
	{
		return c1.sum() > c2.sum();
	}
	static bool operator < (BarVec3b c1, BarVec3b c2)
	{
		return c1.sum() < c2.sum();
	}

	struct barstruct
	{
		ComponentType comtype;
		ProcType proctype;
		ColorType coltype;
		barstruct()
		{
			comtype = ComponentType::Hole;
			proctype = ProcType::f0t255;
			coltype = ColorType::gray;
		}
		barstruct(ProcType pt, ColorType colT, ComponentType comT)
		{
			this->comtype = comT;
			this->proctype = pt;
			this->coltype = colT;
		}
	};


	template<class T>
	struct BarConstructor
	{
		CachedValue<T> stepPorog;
		CachedValue<T> maxLen;
	public:
		//T foneStart;
		//T foneEnd;
		std::vector<barstruct> structure;
		ReturnType returnType;

		bool createGraph = false;
		bool createBinayMasks = false;
		bool createNewComponentOnAttach = false;
		bool killOnMaxLen = false;

#ifdef USE_OPENCV
		bool visualize = false;
#endif // USE_OPENCV

		inline void addStructure(ProcType pt, ColorType colT, ComponentType comT)
		{
			structure.push_back(barstruct(pt, colT, comT));
		}

		void checkCorrect() const
		{
			//if (returnType == ReturnType::barcode2d || returnType == ReturnType::barcode3d)
			//	createBinayMasks = true;

			//if (createGraph && !createBinayMasks)
			//	throw std::exception();

			if (structure.size() == 0)
				throw std::exception();

			//getStepPorog();
		}

		// разница соединяемх значений должна быть меньше этого значения
		T getMaxStepPorog() const
		{
			return stepPorog.getOrDefault(0);
		}

		void setStep(T val)
		{
			stepPorog.set(val);
		}

		T getMaxLen()
		{
			return maxLen.getOrDefault(0);
		}

		void setMaxLen(T val)
		{
			maxLen.set(val);
		}
	};


	template<class T>
	static BarConstructor<T>* createStructure(bc::BarType type)
	{
		switch (type)
		{
		case bc::BarType::bc_byte:
			return new BarConstructor<uchar>();
			break;
		case bc::BarType::bc_float:
			return new BarConstructor<float>();
			break;
		case bc::BarType::bc_int:
			return new BarConstructor<int>();
			break;
		case bc::BarType::bc_short:
			return new BarConstructor<short>();
			break;
		case bc::BarType::bc_ushort:
			return new BarConstructor<ushort>();
			break;
		default:
			return nullptr;
			break;
		}
	}


	struct EXPORT point
	{
		int x = -1;
		int y = -1;
		inline point()
		{
			x = -1; y = -1;
		}

		inline point(int x, int y) :x(x), y(y)
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

#ifdef USE_OPENCV
		inline cv::Point cvPoint()
		{
			return cv::Point(x, y);
		}
#endif // USE_OPENCV

		size_t getLiner(int wid)
		{
			assert(x >= 0);
			assert(y >= 0);

			return y * static_cast<size_t>(wid) + x;
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

	template<class T>
	struct bar3dvalue
	{
		size_t count;
		T value;

		bar3dvalue(T value, size_t count)
		{
			this->count = count;
			this->value = value;
		}
		bar3dvalue()
		{
			this->count = 0;
		}
	};

	template<class T>
	struct barvalue
	{
		bc::point point;
		T value;

		barvalue(bc::point point, T value)
		{
			this->point = point;
			this->value = value;
		}

		barvalue()
		{ }

		int getX() const
		{
			return point.x;
		}
		void setX(int x)
		{
			point.x = x;
		}

		int getY() const
		{
			return point.y;
		}
		void setY(int y)
		{
			point.y = y;
		}
	};

	template<class T>
	using barvector = std::vector<barvalue<T>>;

	template<class T>
	using barcounter = std::vector<bar3dvalue<T>> ;
	
	//**********************************************

INIT_TEMPLATE_STRUCT(CachedValue)
INIT_TEMPLATE_STRUCT(BarConstructor)
}

#endif // BARCODE_H
