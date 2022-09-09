#pragma once
#include <vector>
#include<assert.h>
#include<string.h>
#include <math.h>
#include "presets.h"
#include "include_py.h"

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

enum class BarType : char
{
	NONE = 0,
	BYTE8_1,
	BYTE8_3,
	//FLOAT32_1
};

class EXPORT Barscalar
{
	union BarValue
	{
		unsigned char b1;
		unsigned char b3[3];
	};


public:
	BarValue data;
	BarType type;
	// Overloaded assignment

	Barscalar(uchar i = 0)
	{
		data.b1 = i;
		type = BarType::BYTE8_1;
	}

	Barscalar(uchar i, BarType type)
	{
		this->type = type;
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = i;
			break;
		case BarType::BYTE8_3:
			data.b3[0] = i;
			data.b3[1] = i;
			data.b3[2] = i;
			break;
		default:
			data.b1 = 0;
			break;
		}
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
		memcpy(&data, &R.data, sizeof(BarValue));
		type = R.type;
	}

	unsigned char getByte8() const
	{
		return data.b1;
	}

	std::string text()
	{
		std::string out;
		switch (type)
		{
		case BarType::BYTE8_1:
			return std::to_string(data.b1);
		case BarType::BYTE8_3:
			out = "(" + std::to_string(data.b3[0]) + "," + std::to_string(data.b3[1]) + "," + std::to_string(data.b3[2]) + ")";
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
			break;
		}
	}
#endif

#ifdef _PYD

	bp::tuple value()
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return  bp::make_tuple(data.b1);
			break;
		case BarType::BYTE8_3:
			return bp::make_tuple(data.b3[0], data.b3[1], data.b3[2]);
			break;
		default:
			break;
		}
	}
#endif

private:
	bool more(const Barscalar& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 > X.data.b1;
			break;
		case BarType::BYTE8_3:
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
			return data.b1 == X.data.b1;
		case BarType::BYTE8_3:
		{
			if (X.type == BarType::BYTE8_3)
			{
				return this->data.b3[0] == X.data.b3[0] && this->data.b3[1] == X.data.b3[1] && this->data.b3[2] == X.data.b3[2];
			}
			else
				return this->data.b3[0] == X.data.b1 && this->data.b3[1] == X.data.b1 && this->data.b3[2] == X.data.b1;

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
			return data.b1 >= X.data.b1;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			float b = X.getAvgFloat();
			return (a >= b);
		}
		default:
			assert(false);
			return false;
		}
	}

	// int

	bool more(int& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 > X;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			return (a > X);
		}
		default:
			assert(false);
			return false;
		}
	}


	bool equal(int X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 == X;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			return a == X;
		}
		default:
			assert(false);
			return false;
		}
	}

	bool more_equal(int& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 >= X;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			return (a >= X);
		}
		default:
			assert(false);
			return false;
		}
	}

	// float

	bool more(float& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 > X;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			return (a > X);
		}
		default:
			assert(false);
			return false;
		}
	}


	bool equal(float X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 == X;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			return a == X;
		}
		default:
			assert(false);
			return false;
		}
	}

	bool more_equal(float& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 >= X;
		case BarType::BYTE8_3:
		{
			float a = this->getAvgFloat();
			return (a >= X);
		}
		default:
			assert(false);
			return false;
		}
	}

public:

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

	Barscalar& operator= (const uchar fraction)
	{
		return *this;
	}

	Barscalar& operator= (const int fraction)
	{

		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = fraction;
			break;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] = fraction;
			}
		}

		return *this;
	}

	Barscalar& operator= (const float fraction)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = fraction;
			break;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] = static_cast<uchar>(fraction);
			}
		}

		return *this;
	}

	Barscalar& operator= (const Barscalar& fraction)
	{
		type = fraction.type;
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = fraction.type == BarType::BYTE8_1 ? fraction.data.b1 : fraction.getAvgUchar();
			break;
		case BarType::BYTE8_3:
		default:
			if (fraction.type == BarType::BYTE8_3)
			{

				for (char i = 0; i < 3; ++i)
					data.b3[i] = fraction.data.b3[i];
			}
			else
			{
				for (char i = 0; i < 3; ++i)
					data.b3[i] = fraction.data.b1;
			}
		}

		return *this;
	}

	inline float getAvgFloat() const
	{
		if (type == BarType::BYTE8_3)
		{
			return ((float)data.b3[0] + (float)data.b3[1] + (float)data.b3[2]) / 3.f;
		}
		else
			return data.b1;
	}

	inline float getAvgUchar() const
	{
		return static_cast<uchar>(getAvgFloat());
	}

private:
	float difMax(uchar a, uchar b) const
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
		if (type == BarType::BYTE8_3)
		{
			assert(R.type == BarType::BYTE8_3);
			float res = 0;
			for (char i = 0; i < 3; i++)
			{
				res += sp(difMax(data.b3[i], R.data.b3[i]));
			}
			return sqrtf(res);
		}
		else
			return difMax(data.b1, R.data.b1);
	}

	explicit operator int() const
	{
		return (int)getAvgFloat();
	}

	explicit operator uchar() const
	{
		return (uchar)getAvgFloat();
	}

	explicit operator float() const
	{
		return getAvgFloat();
	}

	auto& operator+=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 += R;
			break;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] += R;
			}
		}

		return *this;
	}

	auto& operator+=(const Barscalar& R)
	{
		if (type == BarType::BYTE8_3)
		{
			if (R.type == BarType::BYTE8_3)
			{
				data.b3[0] += R.data.b3[0];
				data.b3[1] += R.data.b3[1];
				data.b3[2] += R.data.b3[2];
			}
			else
			{
				data.b3[0] += R.data.b1;
				data.b3[1] += R.data.b1;
				data.b3[2] += R.data.b1;
			}
		}
		else
		{
			data.b1 += static_cast<uchar>(R.getAvgFloat());
		}

		return *this;
	}


	auto& operator-=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 -= R;
			break;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] -= R;
			}
		}

		return *this;
	}

	auto& operator-=(const Barscalar& R)
	{
		if (type == BarType::BYTE8_3)
		{
			if (R.type == BarType::BYTE8_3)
			{

				data.b3[0] -= R.data.b3[0];
				data.b3[1] -= R.data.b3[1];
				data.b3[2] -= R.data.b3[2];
			}
			else
			{
				data.b3[0] -= R.data.b1;
				data.b3[1] -= R.data.b1;
				data.b3[2] -= R.data.b1;
			}
		}
		else
		{
			data.b1 -= static_cast<uchar>(R.getAvgFloat());
		}

		return *this;
	}

	auto& operator*=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 *= R;
			break;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] *= R;
			}
		}

		return *this;
	}

	auto& operator*=(const Barscalar& R)
	{
		if (type == BarType::BYTE8_3)
		{
			if (R.type == BarType::BYTE8_3)
			{
				data.b3[0] *= R.data.b3[0];
				data.b3[1] *= R.data.b3[1];
				data.b3[2] *= R.data.b3[2];
			}
			else
			{
				data.b3[0] *= R.data.b1;
				data.b3[1] *= R.data.b1;
				data.b3[2] *= R.data.b1;
			}
		}
		else
		{
			data.b1 *= static_cast<uchar>(R.getAvgFloat());
		}

		return *this;
	}

	auto& operator/=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 /= R;
			break;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] /= R;
			}
		}

		return *this;
	}

	auto& operator/=(const Barscalar& R)
	{
		if (type == BarType::BYTE8_3)
		{
			if (R.type == BarType::BYTE8_3)
			{
				data.b3[0] /= R.data.b3[0];
				data.b3[1] /= R.data.b3[1];
				data.b3[2] /= R.data.b3[2];
			}
			else
			{
				data.b3[0] /= R.data.b1;
				data.b3[1] /= R.data.b1;
				data.b3[2] /= R.data.b1;
			}
		}
		else
		{
			data.b1 /= R.getAvgUchar();
		}

		return *this;
	}

	// Operators
	auto operator+(const Barscalar& R) const
	{
		Barscalar res = *this;
		res += R;
		return res;
	}

	auto operator+(const uchar R) const
	{
		Barscalar res = *this;
		res += R;
		return res;
	}

	auto operator-(const uchar R) const
	{
		Barscalar res = *this;
		res -= R;
		return res;
	}
	auto operator-(const Barscalar& R) const
	{
		Barscalar res = *this;
		res -= R;
		return res;
	}


	Barscalar operator* (const uchar& R) const
	{
		Barscalar res = *this;
		res *= R;
		return res;
	}
	Barscalar operator* (const Barscalar& R) const
	{
		Barscalar res = *this;
		res *= R;
		return res;
	}

	Barscalar operator/ (const uchar& R) const
	{
		Barscalar res = *this;
		res /= R;
		return res;
	}

	Barscalar operator/ (const Barscalar& R) const
	{
		Barscalar res = *this;
		res /= R;
		return res;
	}

	uchar& operator[](unsigned int index)
	{
		assert(index < 3);
		return data.b3[index];
	}

	uchar operator[](unsigned int index) const
	{
		assert(index < 3);
		return data.b3[index];
	}


	Barscalar absDiff(const Barscalar& R) const
	{
		return this->getAvgFloat() > R.getAvgFloat() ? *this - R : R - *this;
	}
};
