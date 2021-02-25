#ifndef BARCODE_H
#define BARCODE_H

#include "presets.h"

#include <vector>
#include <cassert>

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

		T getOrDefault(T defValue)
		{
			return isCached ? val : defValue;
		}
	};

	struct BarRect
	{
		int _x, _y, _wid, _hei;
	public:
		BarRect(int x, int y, int width, int height)
		{
			_x = x;
			_y = y;
			_wid = width;
			_hei = height;
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
	public:
		T foneStart;
		T foneEnd;

#ifdef USE_OPENCV
		bool visualize = false;
#endif // USE_OPENCV

		bool createGraph = false;
		bool createBinayMasks = false;
		ReturnType returnType;
		std::vector<barstruct> structure;

		inline void addStructire(ProcType pt, ColorType colT, ComponentType comT)
		{
			structure.push_back(barstruct(pt, colT, comT));
		}

		void checkCorrect()
		{
			if (returnType == ReturnType::barcode2d || returnType == ReturnType::barcode3d)
				createBinayMasks = true;

			if (createGraph && !createBinayMasks)
				throw std::exception();

			if (returnType == ReturnType::betty && !std::is_same<T, uchar>::value)
				throw std::exception();

			if (structure.size() == 0)
				throw std::exception();

			//getStepPorog();
		}

		// ������� ���������� �������� ������ ���� ������ ����� ��������
		T getStepPorog()
		{
			return stepPorog.getOrDefault(0);
		}

		void setStep(T val)
		{
			stepPorog.set(val);
		}

		void setFoneRange(T st, T ed)
		{
			foneStart = st;
			foneEnd = ed;
		}
	};


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
		inline cv::Point bc::point::cvPoint()
		{
			return cv::Point(x, y);
		}
#endif // USE_OPENCV

		size_t getLiner(int wid)
		{
			assert(x >= 0);
			assert(y >= 0);

			return y * wid + x;
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
	struct ppair
	{
		ppair(bc::point point, T value)
		{
			this->point = point;
			this->value = value;
		}
		bc::point point;
		T value;
	};


	template<class T>
	using pmap = std::vector<ppair<T>>;

	typedef std::vector<uint32_t> barcounter;
	//**********************************************

}

#endif // BARCODE_H
