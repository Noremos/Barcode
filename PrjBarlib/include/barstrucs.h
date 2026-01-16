#ifndef SKIP_M_INC
#pragma once

#include "presets.h"

#include <vector>
#include <cassert>
#include <cstring>
#include "../extra/include_cv.h"
#include "../extra/include_py.h"
#include "barscalar.h"

const unsigned int BAR_MAX_WID = 65535;

#endif

#define BARVALUE_RAM_OPTIMIZATION


MEXP namespace bc
{
	struct barRGB;
	struct barBGR;

	enum class CompareStrategy { CommonToSum, CommonToLen, compare3dHist, compare3dBrightless, FromMethod, Complex, Temp1, Temp2 };

	enum class ComponentType { Component = 0, Hole = 1, FullPrepair, PrepairComp };
	enum class ProcType { f0t255 = 0, f255t0 = 1, Radius = 2, invertf0 = 3, experiment = 4, StepRadius = 5, ValueRadius = 6 };
	enum class ColorType { native = 0, gray = 1, rgb = 2 };

	enum class ReturnType { barcode2d, barcode3d, barcode3dold };
	enum class AttachMode { firstEatSecond, secondEatFirst, createNew, dontTouch, morePointsEatLow, closer };
	// enum class ProcessStrategy { brightness, radius };// classic or geometry

	typedef unsigned int poidex;


	struct barRGB : public Barscalar
	{
		uchar getR() { return this->data.b3[0]; }
		uchar getG() { return this->data.b3[1]; }
		uchar getB() { return this->data.b3[2]; }
	};

	struct barBGR : public Barscalar
	{
		uchar getB() const { return this->data.b3[0]; }
		uchar getG() const { return this->data.b3[1]; }
		uchar getR() const { return this->data.b3[2]; }
	};


	struct EXPORT CachedValue
	{
		Barscalar val;
		bool isCached = false;

		void set(Barscalar val)
		{
			this->val = val;
			this->isCached = true;
		}

		Barscalar getOrDefault(Barscalar defValue) const
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

		float coof()
		{
			return width > height ? static_cast<float>(width) / height : static_cast<float>(height) / width;
		}

		int right()
		{
			return x + width;
		}
		int botton()
		{
			return y + height;
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

	//	static bool operator > (BarVec3b c1, BarVec3b c2)
	//	{
	//		return c1.sum() > c2.sum();
	//	}
	//	static bool operator < (BarVec3b c1, BarVec3b c2)
	//	{
	//		return c1.sum() < c2.sum();
	//	}
	class DatagridProvider;


	struct barstruct
	{
		float maxLen = 999999;
		float maxRadius = 999999;
		float minAttachRadius = 0;
	public:
		//Barscalar foneStart;
		//Barscalar foneEnd;
		ReturnType returnType = ReturnType::barcode2d;
		BarType type = BarType::NONE;

		bool createGraph = false;
		bool createBinaryMasks = false;
		AttachMode attachMode = AttachMode::firstEatSecond;
		//ProcessStrategy processMode = ProcessStrategy::brightness;
		bool killOnMaxLen = false;
		bool extracheckOnPixelConnect = false;
		int colorRange = INT32_MAX;
		float energyStart = 100;

		bool trueSort = false;

#ifdef USE_OPENCV
		bool visualize = false;
		int waitK = 1;
#endif // USE_OPENCV

		ComponentType comtype = ComponentType::Component;
		ProcType proctype = ProcType::f0t255;
		ColorType coltype = ColorType::gray;

		const bc::DatagridProvider* mask = nullptr;
		int maskValueId = 0;

		inline void addStructure(ProcType pt, ColorType colT, ComponentType comT)
		{
			this->comtype = comT;
			this->proctype = pt;
			this->coltype = colT;
		}

		void checkCorrect() const
		{
			//if (returnType == ReturnType::barcode2d || returnType == ReturnType::barcode3d)
			//	createBinaryMasks = true;

			//if (createGraph && !createBinaryMasks)
			//	throw std::exception();

			//if (structure.size() == 0)
				//throw std::exception();

			//getStepPorog();
		}

		// разница соединяемых значений должна быть меньше этого значения
		Barscalar getMaxStepPorog() const
		{
			return maxRadius;
		}

		Barscalar getMaxLen() const
		{
			return maxLen;
		}

		void setMaxLen(float val)
		{
			maxLen = val;
		}
	};

	class BarConstructor
	{
	public:
		std::vector<barstruct> structs;
		inline void addStructure(ProcType pt, ColorType colT, ComponentType comT)
		{
			auto& e = structs.emplace_back();
			e.comtype = comT;
			e.proctype = pt;
			e.coltype = colT;
		}
		inline void add(barstruct str)
		{
			structs.push_back(str);
		}
	};

	struct EXPORT point
	{
		int x = -1;
		int y = -1;
		inline point() : x(-1), y(-1)
		{ }

		inline point(int x, int y) :x(x), y(y)
		{ }

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
		inline cv::Point cvPoint() const
		{
			return cv::Point(x, y);
		}
#endif // USE_OPENCV

		poidex getLiner(int wid) const
		{
			assert(x >= 0);
			assert(y >= 0);

			return y * wid + x;
		}

		point operator+(const int* xy) const
		{
			return point(x + xy[0], y + xy[1]);
		}

		point operator-(const point& other) const
		{
			return point(x - other.x, y - other.y);
		}

		point operator+(const char* xy) const
		{
			return point(x + xy[0], y + xy[1]);
		}
		point operator*(int c) const
		{
			return point(x * c, y * c);
		}
		point operator+(point p) const
		{
			return point(x + p.x, y + p.y);
		}
		int operator[](int i) const
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

	typedef barmapHash<point, bool, pointHash> pmap;
	typedef std::pair<point, bool> ppair;


	struct bar3dvalue
	{
		Barscalar value;
		size_t count = 0;
		float cx = 0.f, cy = 0.f;
		float rat = 0.f;

		bar3dvalue(Barscalar value, size_t count)
		{
			this->count = count;
			this->value = value;
		}
		bar3dvalue()
		{
		}

		bar3dvalue(float x, float y, float r) : cx(x), cy(y), rat(r)
		{
		}
	};

	const uint MAX_WID = 65535;
#ifdef BARVALUE_RAM_OPTIMIZATION
	using BType = unsigned short;
	using BIndex = uint;
#else
	using BType = uint;
	using BIndex = size_t;
#endif


	struct barvalue
	{
		BType x;
		BType y;

		Barscalar value;

		barvalue(BType x, BType y, const Barscalar& value)
		{
			assert(x >= 0);
			assert(y >= 0);
			this->x = x;
			this->y = y;
			this->value = value;
		}

		barvalue(bc::point p, const Barscalar&  value)
		{
			assert(p.x >= 0);
			assert(p.y >= 0);

			x = p.x;
			y = p.y;
			this->value = value;
		}


		barvalue() : x(0), y(0)
		{
			 value.type = BarType::NONE;
		}

		barvalue(const barvalue& other)
			: x(other.x), y(other.y), value(other.value)
		{ }

		barvalue(barvalue&& other) /*: s(std::move(o.s))*/
		{
			this->x = other.x;
			this->y = other.y;
			this->value = other.value;
		}

		// copy assignment
		barvalue& operator=(const barvalue& other)
		{
			// Guard self assignment
			if (this == &other)
				return *this;

			this->x = other.x;
			this->y = other.y;
			this->value = other.value;

			return *this;
		}

		// move assignment
		barvalue& operator=(barvalue&& other) noexcept
		{
			// Guard self assignment
			if (this == &other)
				return *this; // delete[]/size=0 would also be ok

			this->x = other.x;
			this->y = other.y;
			this->value = other.value;

			return *this;
		}

		bc::point getPoint() const
		{
			return bc::point(getX(), getY());
		}

		constexpr BIndex getIndex() const
		{
#ifdef BARVALUE_RAM_OPTIMIZATION
			return getStatInd(x, y);
#else
			return getStatIndBig(x, y);
#endif
		}

		constexpr static uint getStatInd(int x, int y, int widr = MAX_WID)
		{
			return y * widr + x;
		}

		constexpr static size_t getStatIndBig(int x, int y)
		{
			return ((size_t)x << 32) + (size_t)y;
		}

		static bc::point getStatPointBig(size_t index)
		{
			int y = index >> 32;
			int x = (index << 32) >> 32;
			return bc::point(x, y);
		}

		static bc::point getStatPoint(uint index, int widr = MAX_WID)
		{
			return bc::point(index % widr, index / widr);
		}

		int getX() const
		{
			return x;
		}

		int getY() const
		{
			return y;
		}

		void setX(int x)
		{
			this->x = x;
		}

		void setY(int y)
		{
			this->y = y;
		}

		void setXY(int x, int y)
		{
			this->x = x;
			this->y = y;
		}
	};


	using barvector = std::vector<barvalue>;


	using barcounter = std::vector<bar3dvalue>;


	class EXPORT DatagridProvider
	{
	public:
		virtual int wid() const = 0;
		virtual int hei() const = 0;
		virtual int channels() const = 0;

		virtual void maxAndMin(Barscalar& min, Barscalar& max) const = 0;

		// Deprecated
		virtual size_t typeSize() const
		{ return 1; }

		virtual Barscalar get(int x, int y) const = 0;

		virtual Barscalar get(bc::point p) const
		{
			return get(p.x, p.y);
		}

		virtual size_t length() const
		{
			return static_cast<size_t>(wid()) * hei();
		}

		// wid * y + x;
		virtual Barscalar getLiner(size_t pos) const
		{
			return get((int)(pos % wid()), (int)(pos / wid()));
		}

		virtual BarType getType() const
		{
			return type;
		}

		point getPointAt(size_t iter) const
		{
			return point((int)(iter % wid()), (int)(iter / wid()));
		}
		virtual ~DatagridProvider()
		{ }

	public:
		BarType type = BarType::BYTE8_1;
	};

	//**********************************************
}
