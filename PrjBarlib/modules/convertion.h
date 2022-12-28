#ifndef CONVERTION_H
#define CONVERTION_H
#include "base.h"
#include <cstring>

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
struct increnenter
{
	static uchar sysByteOredr;
	static uchar GLOBALloclByteOreder;
	static uint16_t x;
	increnenter(int st, int ed)
	{
		rev = GLOBALloclByteOreder != sysByteOredr;
		i = rev ? ed : st;
	}
	increnenter(int st, int ed, uchar imgByteOrder)
	{
		rev = imgByteOrder != sysByteOredr;
		i = rev ? ed : st;
	}
	bool rev;
	int i = 0;
	int v()
	{
		return rev ? i-- : i++;
	}
};


template<class T>
struct increnenterBytes
{
	increnenterBytes(uchar *bytes, int st, int ed, uchar imgByteOrder)
	{
		this->bytes = bytes;
		rev = imgByteOrder != increnenter::sysByteOredr;
		i = rev ? ed : st;
	}
	increnenterBytes(uchar *bytes, int st, int ed)
	{
		this->bytes = bytes;
		rev = increnenter::GLOBALloclByteOreder != increnenter::sysByteOredr;
		i = rev ? ed : st;
	}
	uchar *bytes;
	bool rev;
	int i = 0;
	T v(int off)
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
static ushort toShort(uchar *bytes)
{
	increnenter t(0, 1);

	return short((unsigned char)(bytes[t.v()]) << 8 | (bytes[t.v()]));
}
static uint toInt(uchar *bytes)
{
	//alternative int* y; y = reinterpret_cast<int*>(bytes); return *y;
	increnenter t(0, 3);
	return int((unsigned char)(bytes[t.v()]) << 24 | (bytes[t.v()]) << 16 | (bytes[t.v()]) << 8 | (bytes[t.v()]));
}
static float toFloat(uchar* bytes)
{
	tofloat conv;
	memcpy(conv.data, bytes, 4);
	//reorder(conv.data, 4);
	return conv.value;
}
static double toDouble(uchar *bytes)
{
	toduble conv;
	memcpy(conv.data, bytes, 8);
//	reorder(conv.data, 8);
	return conv.value;
}


static long long toInt64(uchar *bytes)
{
	//alternative int* y; y = reinterpret_cast<int*>(bytes); return *y;
	increnenterBytes<long long> t(bytes, 0, 7);
	return t.v(56) |t.v(48) |t.v(40) |t.v(32) |t.v(24) | t.v(16) | t.v(8) | t.v(0);
}

static void convert(uchar *bytes, float &out) { out = toFloat(bytes); }
static void convert(uchar *bytes, int &out) { out = toInt(bytes);}
static void convert(uchar* bytes, uchar& out)	{ out = bytes[0];}
static void convert(uchar* bytes, double& out)	{out = toDouble(bytes);}
static void convert(uchar* bytes, ushort& out)	{out = toShort(bytes);}
static void convert(uchar* bytes, short& out)	{out = toShort(bytes);}

static void silense()
{
	uchar buff[16];
	float a;
	int b;
	uchar c;
	ushort d;
	short e;
	double f;
	convert(buff, a);
	convert(buff, b);
	convert(buff, c);
	convert(buff, d);
	convert(buff, e);
	convert(buff, f);
	toInt64(buff);
}
#endif // CONVERTION_H
