#pragma once
#include <vector>

using bytedata = char*;

class AllocatePool
{
	struct ByteData
	{
		ByteData(int newSize = 8092, bool maxb = false)
		{
			newSize = newSize > 8092 ? newSize : 8092;
			offset = data = new char[newSize];
		}

		bytedata data;
		bytedata offset;
		int sizeLast;

		template<class T>
		inline T* alloc(int count)
		{
			int size = count * sizeof(T);
			static_assert(size < sizeLast);

			sizeLast -= size;
			offset += size;

			return reinterpret_cast<T*>(offset - size);
		}
	};
	std::vector<ByteData> blocks;

	template<class T>
	T* allocateForType(int count)
	{
		int toAll = count * sizeof(T);
		if (blocks.back().sizeLast < toAll)
		{
			blocks.push_back(ByteData(toAll, true));
		}

		return blocks.back().alloc<T>(count);
	}
};

enum class BarType : char
{
	NONE = 0,
	BYTE8_1,
	BYTE8_3,
	//FLOAT32_1
};

class Barscalar
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
		memset(&data, i, 3);
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
		memcpy(&data, &R.data, 3);
		type = R.type;
	}


	bool operator==(const int& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 == X;
		case BarType::BYTE8_3:
		default:
		{
			return (data.b3[0] == X && data.b3[1] == X && data.b3[2] == X);
		}
		}
	}

	bool operator==(const Barscalar& X) const
	{
		assert(type == X.type);

		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 == X.data.b1;
		case BarType::BYTE8_3:
		default:
		{
			int a = this->getAvgFloat();
			int b = X.getAvgFloat();
			return (a == b);
		}
		}
	}

	std::partial_ordering operator<=>(const Barscalar& X) const
	{
		//assert(type == X.type);

		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 <=> X.data.b1;
		case BarType::BYTE8_3:
		default:
		{
			float a = this->getAvgFloat();
			float b = X.getAvgFloat();
			return (a <=> b);
		}
		}
	}

	std::partial_ordering operator<=>(const uchar& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 <=> X;
		case BarType::BYTE8_3:
		default:
		{
			int a = this->getAvgFloat();
			return (a <=> X);
		}
		}
	}
	std::partial_ordering operator<=>(const int& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 <=> X;
		case BarType::BYTE8_3:
		default:
		{
			int a = this->getAvgFloat();
			return (a <=> X);
		}
		}
	}

	std::partial_ordering operator<=>(const float& X) const
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			return data.b1 <=> X;
		case BarType::BYTE8_3:
		default:
		{
			float a = this->getAvgFloat();
			return (a <=> X);
		}
		}
	}

	Barscalar& operator= (const uchar fraction)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = fraction;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] = fraction;
			}
		}

		return *this;
	}

	Barscalar& operator= (const int fraction)
	{

		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 = fraction;
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
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] = fraction;
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
			data.b1 = fraction.data.b1;
		case BarType::BYTE8_3:
		default:
			for (char i = 0; i < 3; ++i)
			{
				data.b3[i] = fraction.data.b3[i];
			}
		}

		return *this;
	}

	float getAvgFloat() const
	{
		if (type == BarType::BYTE8_3)
		{
			return ((float)data.b3[0] + (float)data.b3[1] + (float)data.b3[2]) / 3.f;
		}
		else
			return data.b1;
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
			float res = 0;
			for (char i = 0; i < 3; i++)
			{
				res += sp(difMax(data.b3[i], R.data.b3[i]));
			}
			return sqrtf(res);
		}
		else
			return difMax(data.b1, R.data.b1);

		float res;
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
			data.b3[0] += R.data.b3[0];
			data.b3[1] += R.data.b3[1];
			data.b3[2] += R.data.b3[2];
		}
		else
		{
			data.b1 += R.data.b1;
		}

		return *this;
	}


	auto& operator-=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 -= R;
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
			data.b3[0] -= R.data.b3[0];
			data.b3[1] -= R.data.b3[1];
			data.b3[2] -= R.data.b3[2];
		}
		else
		{
			data.b1 -= R.data.b1;
		}

		return *this;
	}

	auto& operator*=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 *= R;
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
			data.b3[0] *= R.data.b3[0];
			data.b3[1] *= R.data.b3[1];
			data.b3[2] *= R.data.b3[2];
		}
		else
		{
			data.b1 *= R.data.b1;
		}

		return *this;
	}

	auto& operator/=(const uchar R)
	{
		switch (type)
		{
		case BarType::BYTE8_1:
			data.b1 /= R;
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
			data.b3[0] /= R.data.b3[0];
			data.b3[1] /= R.data.b3[1];
			data.b3[2] /= R.data.b3[2];
		}
		else
		{
			data.b1 /= R.data.b1;
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

	uchar& operator[](uint index)
	{
		assert(index < 3);
		return data.b3[index];
	}

	uchar operator[](uint index) const
	{
		assert(index < 3);
		return data.b3[index];
	}

#ifdef USE_OPENCV
	cv::Vec3b toCvVec()
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
#endif // USE_OPENCV

};
