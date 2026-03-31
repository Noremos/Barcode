#ifndef CONVERTION_H
#define CONVERTION_H
#ifndef M_SKIP_INC
#include "base.h"
#include <cstring>
#include <bit>
#endif

union toduble
{
	uchar data[8];
	double value;
};
union tofloat
{
	uchar data[4];
	float value;
};
namespace
{
	constexpr uint8_t x = 0x0001;
	constexpr uchar sysByteOredr = (std::endian::native == std::endian::little) ? 77 : 73;
	constexpr uchar GLOBALloclByteOreder = (std::endian::native == std::endian::little) ? 77 : 73;
}
struct increnenter
{
	constexpr increnenter(int st, int ed)
	{
		rev = GLOBALloclByteOreder != sysByteOredr;
		i = rev ? ed : st;
	}
	constexpr increnenter(int st, int ed, uchar imgByteOrder)
	{
		rev = imgByteOrder != sysByteOredr;
		i = rev ? ed : st;
	}
	bool rev;
	int i = 0;
	constexpr int v()
	{
		return rev ? i-- : i++;
	}
};


template<class T>
struct increnenterBytes
{
	constexpr increnenterBytes(uchar *bytes, int st, int ed, uchar imgByteOrder)
	{
		this->bytes = bytes;
		rev = imgByteOrder != ::sysByteOredr;
		i = rev ? ed : st;
	}
	constexpr increnenterBytes(uchar *bytes, int st, int ed)
	{
		this->bytes = bytes;
		rev = ::GLOBALloclByteOreder != ::sysByteOredr;
		i = rev ? ed : st;
	}
	uchar *bytes;
	bool rev;
	int i = 0;

	constexpr T v(int off)
	{
		return (T)bytes[(rev ? i-- : i++)] << off;
	}
};


//void reorder(uchar *bytes, int size)
//{
//	if (sysByteOredr != imgByteOrder)
//	{
//		uchar* istart = bytes, * iend = istart + size;
//		std::reverse(istart, iend);
//	}
//}
constexpr ushort toShort(uchar *bytes)
{
	increnenter t(0, 1);

	return short((unsigned char)(bytes[t.v()]) << 8 | (bytes[t.v()]));
}
constexpr uint toInt(uchar *bytes)
{
	//alternative int* y; y = reinterpret_cast<int*>(bytes); return *y;
	increnenter t(0, 3);
	return int((unsigned char)(bytes[t.v()]) << 24 | (bytes[t.v()]) << 16 | (bytes[t.v()]) << 8 | (bytes[t.v()]));
}
constexpr float toFloat(uchar* bytes)
{
	tofloat conv;
	memcpy(conv.data, bytes, 4);
	//reorder(conv.data, 4);
	return conv.value;
}
constexpr double toDouble(uchar *bytes)
{
	toduble conv;
	memcpy(conv.data, bytes, 8);
//	reorder(conv.data, 8);
	return conv.value;
}


constexpr long long toInt64(uchar *bytes)
{
	//alternative int* y; y = reinterpret_cast<int*>(bytes); return *y;
	increnenterBytes<long long> t(bytes, 0, 7);
	return t.v(56) |t.v(48) |t.v(40) |t.v(32) |t.v(24) | t.v(16) | t.v(8) | t.v(0);
}

constexpr void convert(uchar *bytes, float &out) { out = toFloat(bytes); }
constexpr void convert(uchar *bytes, int &out) { out = toInt(bytes);}
constexpr void convert(uchar* bytes, uchar& out)	{ out = bytes[0];}
constexpr void convert(uchar* bytes, double& out)	{out = toDouble(bytes);}
constexpr void convert(uchar* bytes, ushort& out)	{out = toShort(bytes);}
constexpr void convert(uchar* bytes, short& out)	{out = toShort(bytes);}

#endif // CONVERTION_H
