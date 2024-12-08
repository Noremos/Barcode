#ifndef SKIP_M_INC
#pragma once

#include <vector>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <string>
#include "presets.h"
#include "../extra/include_py.h"
#endif

//#include <compare>

using bytedata = char*;
using uchar = unsigned char;
//class AllocatePool
//{
//	struct ByteData
//	{
//		ByteData(int newSize = 8092, bool maxb = false)
//		{
//			newSize = newSize > 8092 ? newSize : 8092;
//			offset = data = new char[newSize];
//		}

//		bytedata data;
//		bytedata offset;
//		int sizeLast;

//		template<class T>
//		inline T* alloc(int count)
//		{
//			int size = count * sizeof(T);
//			static_assert(size < sizeLast);

//			sizeLast -= size;
//			offset += size;

//			return reinterpret_cast<T*>(offset - size);
//		}
//	};
//	std::vector<ByteData> blocks;

//	template<class T>
//	T* allocateForType(int count)
//	{
//		int toAll = count * sizeof(T);
//		if (blocks.back().sizeLast < toAll)
//		{
//			blocks.push_back(ByteData(toAll, true));
//		}

//		return blocks.back().alloc<T>(count);
//	}
//};

MEXP enum class BarType : char
{
	NONE = 0,
	BYTE8_1,
	BYTE8_3,
	BYTE8_4,
	FLOAT32_1,
	FLOAT32_3,
	INT32_1
	//INT32_3
};

MEXP class EXPORT Barscalar
{
	union BarData
	{
		unsigned char b1;
		unsigned char b3[4];
		float f;
		int i;
		//float* f3;
		//int* i3;
	};

public:
	BarData data;
	BarType type;
	// Overloaded assignment

	Barscalar(uchar i = 0)
	{
		data.b1 = i;
		type = BarType::BYTE8_1;
	}

	Barscalar(float f)
	{
		data.f = f;
		type = BarType::FLOAT32_1;
	}

	template<typename T>
	Barscalar(T i, BarType type)
	{
		this->type = type;
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = i;
			break;
		case BarType::BYTE8_3:
		case BarType::BYTE8_4:
			data.b3[0] = static_cast<unsigned char>(i);
			data.b3[1] = static_cast<unsigned char>(i);
			data.b3[2] = static_cast<unsigned char>(i);
			data.b3[3] = static_cast<unsigned char>(i);
			break;
		case BarType::FLOAT32_1:
			data.f = static_cast<float>(i);
			break;
		case BarType::INT32_1:
			data.i = static_cast<int>(i);
			break;
//			assert(false);
		default:
			data.b1 = 0;
			break;
		}
	}


	Barscalar(uchar i0, uchar i1, uchar i2, uchar i3)
	{
		data.b3[0] = i0;
		data.b3[1] = i1;
		data.b3[2] = i2;
		data.b3[3] = i3;
		type = BarType::BYTE8_4;
	}

	Barscalar(uchar i0, uchar i1, uchar i2)
	{
		data.b3[0] = i0;
		data.b3[1] = i1;
		data.b3[2] = i2;
		type = BarType::BYTE8_3;
	}

	Barscalar(const Barscalar& R)
	{
		memcpy(&data, &R.data, sizeof(BarData));
		type = R.type;
	}

	unsigned char getByte8() const
	{
		return data.b1;
	}


	int getInt() const
	{
		return data.i;
	}

	float getFloat() const
	{
		return data.f;
	}

	unsigned char getRGB(int id) const
	{
		return data.b3[id];
	}

	template<class TSTR, typename TO_STRING>
	TSTR text(bool asArray = false) const
	{
		TSTR out;
		switch (type)
		{
		case BarType::BYTE8_1:
			return TO_STRING::toStr(data.b1);
		case BarType::BYTE8_3:
			if (asArray)
			{
				out = "[";
				out += TO_STRING::toStr(data.b3[0]);
				out += ",";
				out += TO_STRING::toStr(data.b3[1]);
				out += ",";
				out += TO_STRING::toStr(data.b3[2]);
				out += "]";
			}
			else
			{
				out = "(";
				out += TO_STRING::toStr(data.b3[0]);
				out += ",";
				out += TO_STRING::toStr(data.b3[1]);
				out += ",";
				out += TO_STRING::toStr(data.b3[2]);
				out += ")";
			}
			break;
		case BarType::BYTE8_4:
			if (asArray)
			{
				out = "[";
				out += TO_STRING::toStr(data.b3[0]);
				out += ",";
				out += TO_STRING::toStr(data.b3[1]);
				out += ",";
				out += TO_STRING::toStr(data.b3[2]);
				out += ",";
				out += TO_STRING::toStr(data.b3[3]);
				out += "]";
			}
			else
			{
				out = "(";
				out += TO_STRING::toStr(data.b3[0]);
				out += ",";
				out += TO_STRING::toStr(data.b3[1]);
				out += ",";
				out += TO_STRING::toStr(data.b3[2]);
				out += ",";
				out += TO_STRING::toStr(data.b3[3]);
				out += ")";
			}
			break;
		case BarType::FLOAT32_1:
			return TO_STRING::toStr(data.f);
		default:
			break;
		}
		return out;
	}


#ifdef USE_OPENCV
	cv::Vec3b toCvVec() const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return cv::Vec3b(data.b1, data.b1, data.b1);
		case BarType::BYTE8_3:
			return cv::Vec3b(data.b3[0], data.b3[1], data.b3[2]);
		default:
			return cv::Vec3b();
		}
	}
#endif

#ifdef _PYD
	// This function is used to cast Python objects to C++ Barscalar
	static py::handle cast(const Barscalar& bs, py::return_value_policy, py::handle)
	{
		return py::cast((int)bs.getAvgFloat());
	}

	// This function is used to cast C++ Barscalar to Python objects
	static py::handle cast(const char& value, py::return_value_policy, py::handle) {
		return py::cast(Barscalar(value, BarType::BYTE8_1));
	}

	// This function is used to cast C++ Barscalar to Python objects
	static py::handle cast(const uchar& value, py::return_value_policy, py::handle) {
		return py::cast(Barscalar(value, BarType::BYTE8_1));
	}

	// This function is used to cast C++ Barscalar to Python objects
	static py::handle cast(const int& value, py::return_value_policy, py::handle) {
		return py::cast(Barscalar(value, BarType::BYTE8_1));
	}

	// This function is used to cast C++ Barscalar to Python objects
	static py::handle cast(const float& value, py::return_value_policy, py::handle) {
		return py::cast(Barscalar(value, BarType::FLOAT32_1));
	}


	bp::tuple pyvalue()
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			{
				auto t = bp::tuple(1);
				t[0] = data.b1;
				return t;
			}
		case BarType::BYTE8_3:
		{
			auto t = bp::tuple(3);
			t[0] = data.b3[0];
			t[1] = data.b3[1];
			t[2] = data.b3[2];
			return t;
		}
		case BarType::FLOAT32_1:
		{
			auto t = bp::tuple(1);
			t[0] = data.f;
			return t;
		}
		default:
			return bp::tuple(0);
		}
	}
#endif

private:
	bool more(const Barscalar& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
		case BarType::BYTE8_3:
		case BarType::BYTE8_4:
		case BarType::FLOAT32_1:
		case BarType::FLOAT32_3:
		{
			float a = this->getAvgFloat();
			float b = X.getAvgFloat();
			return (a > b);
		}
		default:
			assert(false);
			return false;
		}
	}

	bool equal(const Barscalar& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
		{
			switch (X.type)
			{
			case BarType::BYTE8_1:
				return this->data.b1 == X.data.b1;
			case BarType::BYTE8_3:
				return this->data.b1 == X.data.b3[0] && this->data.b1 == X.data.b3[1] && this->data.b1 == X.data.b3[2];
			case BarType::BYTE8_4:
				return this->data.b1 == X.data.b3[0] && this->data.b1 == X.data.b3[1] && this->data.b1 == X.data.b3[2] && this->data.b1 == X.data.b3[3];
			case BarType::FLOAT32_1:
				return this->data.f == static_cast<uchar>(X.data.f);
			default:
				assert(false);
				return false;
			}
		}
		case BarType::BYTE8_3:
		case BarType::BYTE8_4:
		{
			switch (X.type)
			{
			case BarType::BYTE8_1:
				return this->data.b3[0] == X.data.b1 && this->data.b3[1] == X.data.b1 && this->data.b3[2] == X.data.b1;
			case BarType::BYTE8_3:
				return memcmp(data.b3, X.data.b3, 3) == 0;
			case BarType::BYTE8_4:
				return memcmp(data.b3, X.data.b3, 4) == 0;
			case BarType::FLOAT32_1:
			{
				uchar xf = static_cast<uchar>(X.data.f);
				return this->data.b3[0] == xf && this->data.b3[1] == xf && this->data.b3[2] == xf;
			}
			default:
				assert(false);
				return false;
			}
		}
		case BarType::FLOAT32_1:
			switch (X.type)
			{
			case BarType::BYTE8_1:
				return this->data.b1 == static_cast<float>(X.data.b1);
			case BarType::BYTE8_3:
			{
				uchar xf = static_cast<uchar>(X.data.f);
				return xf == static_cast<float>(X.data.b3[0]) && xf == static_cast<float>(X.data.b3[1]) && xf == static_cast<float>(X.data.b3[2]);
			}
			case BarType::FLOAT32_1:
				return this->data.f == X.data.f;
			default:
				assert(false);
				return false;
			}
		default:
			assert(false);
			return false;
		}
	}

	bool more_equal(const Barscalar& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
		case BarType::BYTE8_3:
		case BarType::FLOAT32_1:
		{
			float a = this->getAvgFloat();
			float b = X.getAvgFloat();
			return (a >= b);
		}
		case BarType::INT32_1:
			return data.i >= X.getAvgFloat();
		default:
			assert(false);
			return false;
		}
	}

	template<typename T>
	bool more(T X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 > X;
		case BarType::BYTE8_3:
		case BarType::FLOAT32_1:
		{
			float a = this->getAvgFloat();
			return a > X;
		}
		case BarType::INT32_1:
			return data.i > X;
		default:
			assert(false);
			return false;
		}
	}

	template<typename T>
	bool equal(T X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 == X;
		case BarType::BYTE8_3:
		case BarType::FLOAT32_1:
		{
			float a = this->getAvgFloat();
			return a == X;
		}
		case BarType::INT32_1:
			return data.i == X;
		default:
			assert(false);
			return false;
		}
	}

	template<typename T>
	bool more_equal(T X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 >= X;
		case BarType::BYTE8_3:
		case BarType::FLOAT32_1:
		{
			float a = this->getAvgFloat();
			return (a >= X);
		}
		case BarType::INT32_1:
			return data.i >= X;
		default:
			assert(false);
			return false;
		}
	}

public:
	float val_div(const Barscalar &X) const
	{
		assert(X.type == type);
		switch (type)
		{
		case BarType::BYTE8_1:
			return static_cast<float>(data.b1) / X.data.b1;
		case BarType::BYTE8_3:
		{
			float res = 0.f;
			for (unsigned char i = 0; i < 3; ++i)
			{
				res += static_cast<float>(data.b3[i]) / static_cast<float>(X.data.b3[i]);
			}
			return res / 3;
		}
		case BarType::FLOAT32_1:
			return static_cast<float>(data.f) / X.data.f;
		default:
			assert(false);
			break;
		}
		return 0;
	}

	bool operator==(const Barscalar& X) const
	{
		return equal(X);
	}

	bool operator>(const Barscalar& X) const
	{
		//assert(type == X.type);
		return more(X);
	}

	bool operator>=(const Barscalar& X) const
	{
		//assert(type == X.type);
		return more_equal(X);
	}

	bool operator<(const Barscalar& X) const
	{
		//assert(type == X.type);
		return !more_equal(X);
	}

	bool operator<=(const Barscalar& X) const
	{
		//assert(type == X.type);
		return !more(X);
	}

	bool operator!=(const Barscalar& X) const
	{
		//assert(type == X.type);
		return !equal(X);
	}


	// uchar

	bool operator==(uchar X) const
	{
		return equal((int)X);
	}

	bool operator!=(uchar X) const
	{
		return !equal((int)X);
	}

	bool operator>(uchar X) const
	{
		//assert(type == X.type);
		return more((int)X);
	}

	bool operator>=(uchar X) const
	{
		//assert(type == X.type);
		return more_equal((int)X);
	}

	bool operator<(uchar X) const
	{
		//assert(type == X.type);
		return !more_equal((int)X);
	}

	bool operator<=(uchar X) const
	{
		//assert(type == X.type);
		return !more((int)X);
	}


	// int

	bool operator==(int X) const
	{
		return equal(X);
	}

	bool operator!=(int X) const
	{
		return !equal(X);
	}

	bool operator>(int X) const
	{
		//assert(type == X.type);
		return more(X);
	}

	bool operator>=(int X) const
	{
		//assert(type == X.type);
		return more_equal(X);
	}

	bool operator<(int X) const
	{
		//assert(type == X.type);
		return !more_equal(X);
	}

	bool operator<=(int X) const
	{
		//assert(type == X.type);
		return !more(X);
	}


	// float

	bool operator==(float X) const
	{
		return equal(X);
	}

	bool operator!=(float X) const
	{
		return !equal(X);
	}

	bool operator>(float X) const
	{
		//assert(type == X.type);
		return more(X);
	}

	bool operator>=(float X) const
	{
		//assert(type == X.type);
		return more_equal(X);
	}

	bool operator<(float X) const
	{
		//assert(type == X.type);
		return !more_equal(X);
	}

	bool operator<=(float X) const
	{
		//assert(type == X.type);
		return !more(X);
	}

	//public:
	//	std::partial_ordering operator<=>(const Barscalar& X) const
	//	{
	//		switch (type)
	//		{
	//		case BarType::BYTE8_1:
	//			return data.b1 <=> X.data.b1;
	//		case BarType::BYTE8_3:
	//		default:
	//		{
	//			int t = 0, x = 0;
	//			t << this->data.b3[0] << this->data.b3[1] << this->data.b3[2];
	//			x << X.data.b3[0] << X.data.b3[1] << X.data.b3[2];
	//			return t <=> x;
	//		}
	//		}
	//	}

	//	std::partial_ordering operator<=>(const uchar& X) const
	//	{
	//		switch (type)
	//		{
	//		case BarType::BYTE8_1:
	//			return data.b1 <=> X;
	//		case BarType::BYTE8_3:
	//		default:
	//		{
	//			int a = this->getAvgFloat();
	//			return (a <=> X);
	//		}
	//		}
	//	}
	//	std::partial_ordering operator<=>(const int& X) const
	//	{
	//		switch (type)
	//		{
	//		case BarType::BYTE8_1:
	//			return data.b1 <=> X;
	//		case BarType::BYTE8_3:
	//		default:
	//		{
	//			int a = this->getAvgFloat();
	//			return (a <=> X);
	//		}
	//		}
	//	}

	//	std::partial_ordering operator<=>(const float& X) const
	//	{
	//		switch (type)
	//		{
	//		case BarType::BYTE8_1:
	//			return data.b1 <=> X;
	//		case BarType::BYTE8_3:
	//		default:
	//		{
	//			float a = this->getAvgFloat();
	//			return (a <=> X);
	//		}
	//		}
	//	}

	Barscalar& operator= (const uchar c)
	{
		type = BarType::BYTE8_1;
		data.b1 = c;

		return *this;
	}

	Barscalar& operator= (const float fraction)
	{
		type = BarType::FLOAT32_1;
		data.f = fraction;

		return *this;
	}

	Barscalar& operator= (const Barscalar& R)
	{
		type = R.type;
		memcpy(&data, &R.data, sizeof(BarData));

//		switch (type)
//		{
//		case BarType::BYTE8_1:
//			data.b1 = fraction.getAvgUchar();
//			break;
//		case BarType::BYTE8_3:
//			switch (fraction.type)
//			{
//				case BarType::BYTE8_1:
//					for (unsigned char i = 0; i < 3; ++i)
//						data.b3[i] = fraction.data.b1;
//					break;
//				case BarType::BYTE8_3:
//					for (unsigned char i = 0; i < 3; ++i)
//						data.b3[i] = fraction.data.b3[i];
//					break;
//				case BarType::FLOAT32_1:
//					for (unsigned char i = 0; i < 3; ++i)
//						data.b3[i] = fraction.data.f;
//					break;
//				default:
//					assert(false);
//			}
//			break;
//		case BarType::FLOAT32_1:
//			data.f = fraction.getAvgFloat();
//			break;
//		default:
//			assert(false);
//		}

		return *this;
	}

	inline float getAvgFloat() const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return static_cast<float>(data.b1);
		case BarType::BYTE8_3:
			return (static_cast<float>(data.b3[0]) + static_cast<float>(data.b3[1]) + static_cast<float>(data.b3[2])) / 3.f;
		case BarType::FLOAT32_1:
			return data.f;
		case BarType::INT32_1:
			return static_cast<float>(data.i);
		default:
			assert(false);
			return 0;
		}
	}

	inline uchar getAvgUchar() const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1;
		case BarType::BYTE8_3:
			return ((int)data.b3[0] + (int)data.b3[1] + (int)data.b3[2]) / 3;
		case BarType::FLOAT32_1:
			return static_cast<uchar>(data.f);
		default:
			assert(false);
			return 0;
		}
	}

private:
	float difMax(uchar a, uchar b) const
	{
		return static_cast<float>(a > b ? a - b : b - a);
	}

	float difMax(float a, float b) const
	{
		return static_cast<float>(a > b ? a - b : b - a);
	}

	float sp(float v) const
	{
		return v * v;
	}
public:
	float val_distance(const Barscalar& R) const
	{
		assert(type == R.type);

		switch (type)
		{
		case BarType::BYTE8_1:
			return difMax(data.b1, R.data.b1);
		case BarType::BYTE8_3:
		{
			float res = 0;
			for (unsigned char i = 0; i < 3; i++)
				res += sp(difMax(data.b3[i], R.data.b3[i]));

			return sqrtf(res);
		}
		case BarType::FLOAT32_1:
			return difMax(data.f, R.data.f);
		default:
			assert(false);
			return 0;
		}
	}

	explicit operator int() const
	{
		return (int)getAvgFloat();
	}

	explicit operator uchar() const
	{
		return getAvgUchar();
	}

	explicit operator float() const
	{
		return getAvgFloat();
	}

	template<typename T>
	Barscalar& operator+=(const T R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 += R;
			break;
		case BarType::BYTE8_3:
			for (unsigned char i = 0; i < 3; ++i)
				data.b3[i] += R;
			break;
		case BarType::FLOAT32_1:
			data.f += R;
			break;
		default:
			assert(false);
		}

		return *this;
	}

	Barscalar& operator+=(const Barscalar& R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 += R.getAvgUchar();
			break;
		case BarType::BYTE8_3:
		{
			switch (type)
			{
			case BarType::BYTE8_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] += R.data.b1;
				break;
			case BarType::BYTE8_3:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] += R.data.b3[i];
				break;
			case BarType::FLOAT32_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] += R.data.f;
				break;
			default:
				assert(false);
			}
			break;
		}
		case BarType::FLOAT32_1:
			data.f += R.getAvgFloat();
			break;
		default:
			assert(false);
		}

		return *this;
	}


	template<typename T>
	Barscalar& operator-=(const T R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 -= R;
			break;
		case BarType::BYTE8_3:
		{
			switch (type)
			{
			case BarType::BYTE8_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] -= R;
				break;
			case BarType::BYTE8_3:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] -= R;
				break;
			case BarType::FLOAT32_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] -= R;
				break;
			default:
				assert(false);
			}
			break;
		}
		case BarType::FLOAT32_1:
			data.f -= R;
			break;
		default:
			assert(false);
		}
		return *this;
	}

	Barscalar& operator-=(const Barscalar& R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 -= R.getAvgUchar();
			break;
		case BarType::BYTE8_3:
		{
			switch (type)
			{
			case BarType::BYTE8_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] -= R.data.b1;
				break;
			case BarType::BYTE8_3:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] -= R.data.b3[i];
				break;
			case BarType::FLOAT32_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] -= (uchar)R.data.f;
				break;
			default:
				assert(false);
			}
			break;
		}
		case BarType::FLOAT32_1:
			data.f -= R.getAvgFloat();
			break;
		default:
			assert(false);
		}

		return *this;
	}

	constexpr void multiplySafe(uchar& src, uchar by)
	{
		src = std::min(static_cast<int>(src) * by, 255);
	}

	template<typename T>
	Barscalar& operator*=(T R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			multiplySafe(data.b1, R);
			break;
		case BarType::BYTE8_3:
			for (unsigned char i = 0; i < 3; ++i)
				multiplySafe(data.b3[i], R);
			break;
		default:
			data.f *= R;
		}

		return *this;
	}

	Barscalar& operator*=(const Barscalar& R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 *= R.getAvgUchar();
			break;
		case BarType::BYTE8_3:
		{
			switch (type)
			{
			case BarType::BYTE8_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] *= R.data.b1;
				break;
			case BarType::BYTE8_3:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] *= R.data.b3[i];
				break;
			case BarType::FLOAT32_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] *= R.data.f;
				break;
			default:
				assert(false);
			}
			break;
		}
		case BarType::FLOAT32_1:
			data.f *= R.getAvgFloat();
			break;
		default:
			assert(false);
		}

		return *this;
	}

	template<typename T>
	Barscalar& operator/=(const T R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 /= R;
			break;
		case BarType::BYTE8_3:
		{
			switch (type)
			{
			case BarType::BYTE8_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] /= R;
				break;
			case BarType::BYTE8_3:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] /= R;
				break;
			case BarType::FLOAT32_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] /= R;
				break;
			default:
				assert(false);
			}
			break;
		}
		case BarType::FLOAT32_1:
			data.f /= R;
			break;
		default:
			assert(false);
		}
		return *this;
	}

	Barscalar& operator/=(const Barscalar& R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 /= R.getAvgUchar();
			break;
		case BarType::BYTE8_3:
		{
			switch (type)
			{
			case BarType::BYTE8_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] /= R.data.b1;
				break;
			case BarType::BYTE8_3:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] /= R.data.b3[i];
				break;
			case BarType::FLOAT32_1:
				for (unsigned char i = 0; i < 3; ++i)
					data.b3[i] /= R.data.f;
				break;
			default:
				assert(false);
			}
			break;
		}
		case BarType::FLOAT32_1:
			data.f /= R.getAvgFloat();
			break;
		default:
			assert(false);
		}

		return *this;
	}


	// Operators

	Barscalar operator+(const Barscalar& R) const
	{
		Barscalar res = *this;
		res += R;
		return res;
	}

	template<typename T>
	Barscalar operator+(const T R) const
	{
		Barscalar res = *this;
		res += R;
		return res;
	}

	Barscalar operator-(const Barscalar& R) const
	{
		Barscalar res = *this;
		res -= R;
		return res;
	}

	template<typename T>
	Barscalar operator-(const T R) const
	{
		Barscalar res = *this;
		res -= R;
		return res;
	}

	Barscalar operator* (const Barscalar& R) const
	{
		Barscalar res = *this;
		res *= R;
		return res;
	}

	template<typename T>
	Barscalar operator* (const T R) const
	{
		Barscalar res = *this;
		res *= R;
		return res;
	}

	Barscalar operator/ (const Barscalar& R) const
	{
		Barscalar res = *this;
		res /= R;
		return res;
	}

	template<typename T>
	Barscalar operator/ (const T R) const
	{
		Barscalar res = *this;
		res /= R;
		return res;
	}

	Barscalar operator%(int n)
	{
		Barscalar res = *this;
		switch (type)
		{
		case BarType::BYTE8_1:
			res.data.b1 %= n;
			break;
		case BarType::BYTE8_3:
		{
			for (unsigned char i = 0; i < 3; ++i)
				res.data.b3[i] %= n;

			break;
		}
		case BarType::FLOAT32_1:
			res.data.f = static_cast<int>(data.f) % n;
			break;
		default:
			assert(false);
		}
		return res;
	}

	Barscalar operator%(const Barscalar& r)
	{
		Barscalar res = *this;
		switch (type)
		{
		case BarType::BYTE8_1:
			res.data.b1 %= r.getByte8();
			break;
		case BarType::BYTE8_3:
		{
			if (r.type == BarType::BYTE8_1)
			{
				for (unsigned char i = 0; i < 3; ++i)
					res.data.b3[i] %= r.getAvgUchar();
			}
			else
			{
				for (unsigned char i = 0; i < 3; ++i)
					res.data.b3[i] %= r.data.b3[i];
			}

			break;
		}
		case BarType::FLOAT32_1:
			res.data.f = static_cast<int>(data.f) % static_cast<int>(r.getAvgFloat());
			break;
		default:
			assert(false);
		}
		return res;
	}

	uchar& operator[](unsigned int index)
	{
		switch (type)
		{
		case BarType::BYTE8_3:
			assert(index < 3);
			return data.b3[index];
		case BarType::BYTE8_1:
		case BarType::FLOAT32_1:
			assert(index == 0);
			return data.b3[index];
		default:
			assert(false);
			return data.b1;
		}
	}

	uchar operator[](unsigned int index) const
	{
		switch (type)
		{
		case BarType::BYTE8_3:
			assert(index < 3);
			return data.b3[index];
		case BarType::BYTE8_1:
			assert(index == 0);
			return data.b1;
		case BarType::FLOAT32_1:
			assert(index == 0);
			return static_cast<uchar>(data.f);
		default:
			assert(false);
			return 0;
		}
	}

	Barscalar absDiff(const Barscalar& R) const
	{
		const Barscalar& L = *this;
		return L.getAvgFloat() > R.getAvgFloat() ? L - R : R - L;
	}
};
