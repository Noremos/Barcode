#ifndef BARCODE_H
#define BARCODE_H

#include "presets.h"

#include <vector>
#include <cassert>
#include <cstring>
#include <unordered_map>
#include "include_cv.h"

#define BARVALUE_RAM_OPTIMIZATION

namespace bc
{

	template<class T, uint N>
	struct EXPORT barVector
	{
	protected:
		T vecdata[N];
	public:
		barVector()
		{
			memset(vecdata, 0, N * sizeof(T));
		}

		barVector<T, N>(int v)
		{
			memset(vecdata, v, N * sizeof(T));
		}

		barVector<T, N>(const barVector<T, N>& copy)
		{
			memcpy(vecdata, copy.vecdata, N * sizeof(T));
		}


		// Overloaded assignment
		auto& operator= (const barVector<T, N>& fraction)
		{
			memcpy(vecdata, fraction.vecdata, N * sizeof(T));
			return *this;
		}


		auto& operator= (int fraction)
		{
			memset(vecdata, fraction, N * sizeof(T));
			return *this;
		}

		explicit operator int() const
		{
			int avg = 0;
			for (size_t i = 0; i < N; i++)
			{
				avg += vecdata[i];
			}
			return avg / N;
		}

		explicit operator float() const
		{
			float avg = 0;
			for (size_t i = 0; i < N; i++)
			{
				avg += vecdata[i];
			}
			return avg / N;
		}

		template<class KJ>
		auto operator-(const KJ R) const
		{
			KJ res{};
			for (size_t i = 0; i < N; i++)
			{
				res.vecdata[i] = vecdata[i] - R.vecdata[i];
			}

			return res;
		}

		auto& operator-=(const barVector<T, N>& R)
		{
			for (size_t i = 0; i < N; i++)
			{
				vecdata[i] -= R.vecdata[i];
			}

			return *this;
		}

		auto operator+(const barVector<T, N>& R)
		{
			barVector<T, N> res;
			for (size_t i = 0; i < N; i++)
			{
				res.vecdata[i] = vecdata[i] + R.vecdata[i];
			}

			return res;
		}

		auto& operator+=(const barVector<T, N>& R)
		{
			for (size_t i = 0; i < N; i++)
			{
				vecdata[i] += R.vecdata[i];
			}

			return *this;
		}

		bool operator> (const barVector<T, N>& fraction) const
		{
			return (float)(*this) > (float)fraction;
		}

		bool operator< (const barVector<T, N>& fraction) const
		{
			return (float)(*this) < (float)fraction;
		}

		bool operator <= (const barVector<T, N>& fraction) const
		{
			return (float)(*this) <= (float)fraction;
		}

		bool operator >= (const barVector<T, N>& fraction) const
		{
			return (float)(*this) >= (float)fraction;
		}

		bool operator== (const barVector<T, N>& fraction) const
		{
			return memcmp(vecdata, fraction.vecdata, N * sizeof(T)) == 0;
		}

		bool operator!= (const barVector<T, N>& fraction) const
		{
			return memcmp(vecdata, fraction.vecdata, N * sizeof(T)) != 0;
		}

		barVector<T, N> operator/ (const barVector<T, N>& fraction) const
		{
			std::exception();
			return fraction;
		}

		barVector<T, N> operator* (const barVector<T, N>& fraction) const
		{
			std::exception();
			return fraction;
		}

		T& operator[](uint index)
		{
			assert(index < N);
			return vecdata[index];
		}

		T operator[](uint index) const
		{
			assert(index < N);
			return vecdata[index];
		}
	};

	struct barRGB : public barVector<uchar, 3>
	{
		uchar getR() { return this->vecdata[0]; }
		uchar getG() { return this->vecdata[1]; }
		uchar getB() { return this->vecdata[2]; }
	};

	struct barBGR : public barVector<uchar, 3>
	{
		uchar getB() { return this->vecdata[0]; }
		uchar getG() { return this->vecdata[1]; }
		uchar getR() { return this->vecdata[2]; }
	};

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

	struct barstruct
	{
		ComponentType comtype;
		ProcType proctype;
		ColorType coltype;
		barstruct()
		{
			comtype = ComponentType::Component;
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
		T d = 0;
		bool createGraph = false;
		bool createBinaryMasks = false;
		AttachMode attachMode = AttachMode::firstEatSecond;
		//ProcessStrategy processMode = ProcessStrategy::brightness;
		bool killOnMaxLen = false;
		bool extracheckOnPixelConnect = false;
#ifdef USE_OPENCV
		bool visualize = false;
		int waitK = 1;
#endif // USE_OPENCV

		inline void addStructure(ProcType pt, ColorType colT, ComponentType comT)
		{
			structure.push_back(barstruct(pt, colT, comT));
		}

		void checkCorrect() const
		{
			//if (returnType == ReturnType::barcode2d || returnType == ReturnType::barcode3d)
			//	createBinaryMasks = true;

			//if (createGraph && !createBinaryMasks)
			//	throw std::exception();

			if (structure.size() == 0)
				throw std::exception();

			//getStepPorog();
		}

		// разница соединяемых значений должна быть меньше этого значения
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

	typedef std::unordered_map<point, bool, pointHash> pmap;
	typedef std::pair<point, bool> ppair;

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
	struct pybarvalue
	{
		T value;
		int x, y;

		pybarvalue()
		{}

		pybarvalue(int x, int y, T value)
		{
			this->x = x;
			this->y = y;
			this->value = value;
		}
	};

#ifdef BARVALUE_RAM_OPTIMIZATION
	const uint MAX_WID = 65535;
#define OPTTIF(A, B) (A)
#else
#define OPTTIF((A), (B)) (B)
#endif

	template<class T>
	struct barvalue
	{
#ifdef BARVALUE_RAM_OPTIMIZATION
		uint index;
#else
		uint x;
		uint y;
#endif
		T value;

		barvalue(int x, int y, T value)
		{
			assert(x >= 0);
			assert(y >= 0);
			OPTTIF(index = y * MAX_WID + x, this->x = x; this->y = y);
			this->value = value;
		}

		barvalue(bc::point p, T value)
		{
			assert(p.x >= 0);
			assert(p.y >= 0);

			OPTTIF(index = p.y * MAX_WID + p.x,
				x = p.x; y = p.y);
			this->value = value;
		}


		barvalue()
		{ }

		barvalue(const barvalue& other) /*: s(other.s)*/
		{
			OPTTIF(index = this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;
		}

		barvalue(barvalue&& other) /*: s(std::move(o.s))*/
		{
			OPTTIF(this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;
		}

		// copy assignment
		barvalue& operator=(const barvalue& other)
		{
			// Guard self assignment
			if (this == &other)
				return *this;

			OPTTIF(this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;

			return *this;
		}

		// move assignment
		barvalue& operator=(barvalue&& other) noexcept
		{
			// Guard self assignment
			if (this == &other)
				return *this; // delete[]/size=0 would also be ok

			OPTTIF(this->index = other.index, this->x = other.x; this->y = other.y);
			this->value = other.value;

			return *this;
		}

		bc::point getPoint() const
		{
			return bc::point(getX(), getY());
		}

		int getIndex(int wid = 0) const
		{
			return OPTTIF(index, y * wid + x);
		}

		int getX() const
		{
			return OPTTIF(index % MAX_WID, x);
		}

		void setX(int x)
		{
			OPTTIF(index = getY() * MAX_WID + x, this->x = x);
		}

		int getY() const
		{
			return OPTTIF(index / MAX_WID, y);
		}

		void setY(int y)
		{
			OPTTIF(index = y * MAX_WID + getX(), this->y = y);
		}

		pybarvalue<T> getPyValue() const
		{
			return pybarvalue<T>(getX(), getY(), value);
		}
	};

	template<class T>
	using barvector = std::vector<barvalue<T>>;

	template<class T>
	using barcounter = std::vector<bar3dvalue<T>>;

	//**********************************************

	INIT_TEMPLATE_STRUCT(CachedValue)
	INIT_TEMPLATE_STRUCT(BarConstructor)
}

#endif // BARCODE_H
