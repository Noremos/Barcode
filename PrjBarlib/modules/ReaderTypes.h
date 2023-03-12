#pragma once
#include "base.h"
#include <queue>
#include "sidesrc/flat_hash_map/unordered_map.hpp"

enum class ImageType { int8, int16, int32, float8, float16, float32, float64, rgb8, argb8 };

int getImgTypeSize(ImageType type)
{
	switch (type)
	{
	case ImageType::int8:
		return 1;
	case ImageType::int16:
		return 2;
	case ImageType::int32:
		//	case ImageType::argb8:
	case ImageType::float32:
		return 4;
	default:
		throw std::exception("");
	}
	//return 0;
}



union valtypes1
{
	short s;
	float f;
	int i;
};

struct RT_rgb
{
	valtypes1 samples[4];
	char sampl;
	float avg(ImageType type) const
	{
		float r = 0;
		for (int i = 0; i < sampl; ++i)
		{
			switch (type)
			{
			case ImageType::int8:
			case ImageType::int16:
				r += samples[i].s;
				break;
			case ImageType::float32:
				r += samples[i].f;
				break;
			case ImageType::int32:
				r += samples[i].i;
				break;
			default:
				std::exception_ptr();
				break;
			}
		}
		return r / sampl;
	}
	bool eq(const RT_rgb& o, ImageType type) const
	{
		bool r = true;
		for (int i = 0; i < sampl; ++i)
		{
			switch (type)
			{
			case ImageType::int8:
			case ImageType::int16:
				r = r && (samples[i].s == o.samples[i].s);
				break;
			case ImageType::float32:
				r = r && (samples[i].f == o.samples[i].f);
				break;
			case ImageType::int32:
				r = r && (samples[i].i == o.samples[i].i);
				break;
			default:
				std::exception();
				break;
			}
		}

		return false;
	}
};

union valtype
{
	short s;
	float f;
	int i;
	RT_rgb rgba;
};

// TODO rewrite it!
struct roweltype
{
	ImageType type;
	valtype val;
	int sampl = 1;
	roweltype(int v = 0)
	{
		val.i = v;
		type = ImageType::int32;
	}

	roweltype(float v)
	{
		val.f = v;
		type = ImageType::float32;
	}

	roweltype(short v)
	{
		val.s = v;
		type = ImageType::int16;
	}

	roweltype(uchar v)
	{
		val.s = v;
		type = ImageType::int8;
	}

	roweltype(uchar r, uchar g, uchar b, uchar a, ImageType sampelType)
	{
		sampl = 4;
		type = sampelType;
		switch (type)
		{
		case ImageType::int8:
		case ImageType::int16:
			val.rgba.samples[0].s = r;
			val.rgba.samples[1].s = g;
			val.rgba.samples[2].s = b;
			val.rgba.samples[3].s = a;
			val.rgba.sampl = 4;
			break;
		case ImageType::int32:
			val.rgba.samples[0].i = r;
			val.rgba.samples[1].i = g;
			val.rgba.samples[2].i = b;
			val.rgba.samples[3].i = a;
			val.rgba.sampl = 4;
			break;
		case ImageType::float32:
			val.rgba.samples[0].f = r;
			val.rgba.samples[1].f = g;
			val.rgba.samples[2].f = b;
			val.rgba.samples[3].f = a;
			val.rgba.sampl = 4;
			break;
		default:
			std::exception();
		}
		type = ImageType::argb8;
	}

	bool eq(const roweltype& o) const
	{
		assert(sampl == o.sampl);
		assert(type == o.type);
		if (sampl > 1)
		{
			return val.rgba.eq(o.val.rgba, type);
		}
		switch (type)
		{
		case ImageType::int8:
		case ImageType::int16:
			return val.s == o.val.s;
		case ImageType::int32:
			return val.i == o.val.i;
			//		case ImageType::argb8:
			//			return memcmp(val.rgba.rgba, o.val.rgba.rgba, 4) == 0;
		case ImageType::float32:
			return val.f == o.val.f;
		default:
			std::exception();
		}

		return false;
	}

	bool more(const roweltype& o) const
	{
		assert(sampl == o.sampl);
		assert(type == o.type);
		if (sampl > 1)
		{
			return val.rgba.avg(type) > o.val.rgba.avg(type);
		}

		switch (type)
		{
		case ImageType::int8:
		case ImageType::int16:
			return val.s > o.val.s;
		case ImageType::int32:
			return val.i > o.val.i;
		case ImageType::float32:
			return val.f > o.val.f;
		default:
			std::exception();
		}

		return false;
	}

	float avg()
	{
		if (sampl > 1)
		{
			return val.rgba.avg(type);
		}
		switch (type)
		{
		case ImageType::int8:
			return val.s;
		case ImageType::int16:
			return val.s;
		case ImageType::int32:
			return val.i;
		case ImageType::float32:
			return val.f;
		default:
			std::exception();
		}

		return 0;
	}

	bool operator==(const roweltype& o) const
	{
		return eq(o);
	}

	bool operator>(const roweltype& o) const
	{
		return more(o);
	}

	bool operator<(const roweltype& o) const
	{
		return !more(o);
	}
	bool operator>=(const roweltype& o) const
	{
		return more(o) || eq(o);
	}

	bool operator<=(const roweltype& o) const
	{
		return !more(o) || eq(o);
	}
};


union rowtype
{
	short* s;
	float* f;
	int* i;
	uchar* b;
	//	RT_rgb* rgba;
};


struct rowptr
{
	ImageType type;
	rowtype ptr;
	size_t count;
	int samples = 1;
	bool deletePtr = false;

	rowptr() noexcept
	{
		type = ImageType::int32;
		ptr.i = nullptr;
		deletePtr = false;
	}

	rowptr(const rowptr& other) noexcept
	{
		//		assert(false);
		deletePtr = false;
		if (other.deletePtr)
		{
			set(other.count, other.type, other.samples);
			copyDataFrom(other.getBuffer());
		}
		else
			set(other.getBuffer(), other.type, other.count, other.samples, false);
	}

	rowptr(rowptr&& other) noexcept
	{
		deletePtr = false;
		set(other.getBuffer(), other.type, other.count, other.samples);
		deletePtr = std::exchange(other.deletePtr, false);
		other.setNull();
	}

	// copy assignment
	rowptr& operator=(const rowptr& other)
	{
		// Guard self assignment
		if (this == &other)
			return *this;

		if (other.deletePtr)
		{
			set(other.count, other.type, other.samples);
			copyDataFrom(other.getBuffer());
		}
		else
			set(other.getBuffer(), other.type, other.count, other.samples, false);

		return *this;
	}

	// move assignment
	rowptr& operator=(rowptr&& other) noexcept
	{
		// Guard self assignment
		if (this == &other)
			return *this; // delete[]/size=0 would also be ok

		set(other.getBuffer(), other.type, other.count, other.samples);
		deletePtr = std::exchange(other.deletePtr, false);
		other.setNull();
		return *this;
	}

	void set(uchar* buffer, ImageType type, size_t count, int samples = 1, bool deleteData = false)
	{
		release();
		deletePtr = deleteData;
		this->samples = samples;

		this->count = count;
		this->type = type;
		switch (type)
		{
		case ImageType::int8:
			ptr.b = reinterpret_cast<uchar*>(buffer);
			break;
		case ImageType::int16:
			ptr.s = reinterpret_cast<short*>(buffer);
			break;
		case ImageType::int32:
			ptr.i = reinterpret_cast<int*>(buffer);
			break;
			//		case ImageType::argb8:
			//			ptr.rgba = reinterpret_cast<RT_rgb*>(buffer);
			//			break;
		case ImageType::float32:
			ptr.f = reinterpret_cast<float*>(buffer);
			break;
		default:
			std::exception();
		}
	}

	void set(size_t count, ImageType type, int samples = 1)
	{
		release();
		deletePtr = true;
		this->count = count;
		this->type = type;
		this->samples = samples;
		int rcount = count * samples;
		switch (type)
		{
		case ImageType::int8:
			ptr.b = new uchar[rcount];
			break;
		case ImageType::int16:
			ptr.s = new short[rcount];
			break;
		case ImageType::int32:
			ptr.i = new int[rcount];
			break;
			//		case ImageType::argb8:
			//			samples = 4;
			//			ptr.i = new RT_rgb[count];
			//			break;
		case ImageType::float32:
			ptr.f = new float[rcount];
			break;
		default:
			std::exception();
		}
	}

	//	void appendOffset(size_t off)
	//	{
	//		switch (type)
	//		{
	//		case ImageType::int16:
	//			ptr.s += off;
	//			break;
	//		case ImageType::int32:
	//			ptr.i += off;
	//			break;
	//		case ImageType::argb8:
	//			ptr.rgba += off;
	//			break;
	//		case ImageType::float32:
	//			ptr.f += off;
	//			break;
	//		default:
	//			std::exception();
	//		}
	//	}

	uchar* getBuffer() const
	{
		return getOffset(0);
	}

	uchar* getOffset(size_t off) const
	{
		off *= samples;
		switch (type)
		{
		case ImageType::int8:
			return reinterpret_cast<uchar*>(ptr.b + off);
		case ImageType::int16:
			return reinterpret_cast<uchar*>(ptr.s + off);
		case ImageType::int32:
			return reinterpret_cast<uchar*>(ptr.i + off);
			//		case ImageType::argb8:
			//			return reinterpret_cast<uchar*>(ptr.rgba + off);
		case ImageType::float32:
			return reinterpret_cast<uchar*>(ptr.f + off);
		default:
			std::exception();
		}

		return nullptr;
	}

	void setValue(size_t index, int sample, valtype val)
	{
		index *= samples + sample;
		switch (type)
		{
		case ImageType::int8:
			ptr.b[index] = val.s;
			break;
		case ImageType::int16:
			ptr.s[index] = val.s;
			break;
		case ImageType::int32:
			ptr.i[index] = val.i;
			break;
			//		case ImageType::argb8:
			//			ptr.rgba[index] = val.rgba;
			break;
		case ImageType::float32:
			ptr.f[index] = val.f;
			break;
		default:
			std::exception();
		}
	}

	void setValue(size_t index, valtype val)
	{
		index *= samples;
		for (int i = 0; i < samples; ++i)
		{
			switch (type)
			{
			case ImageType::int8:
				ptr.b[index] = val.s;
				break;
			case ImageType::int16:
				ptr.s[index] = val.s;
				break;
			case ImageType::int32:
				ptr.i[index] = val.i;
				break;
				//		case ImageType::argb8:
				//			ptr.rgba[index] = val.rgba;
				break;
			case ImageType::float32:
				ptr.f[index] = val.f;
				break;
			default:
				std::exception();
			}

		}

	}

	valtype getValue(size_t index) const
	{
		valtype r;
		index *= samples;
		if (samples > 1)
		{
			r.rgba.sampl = samples;
			for (int i = 0; i < samples; ++i)
			{
				switch (type)
				{
				case ImageType::int8:
					r.rgba.samples[i].s = *(ptr.b + index + i);
					break;
				case ImageType::int16:
					r.rgba.samples[i].s = *(ptr.s + index + i);
					break;
				case ImageType::int32:
					r.rgba.samples[i].i = *(ptr.i + index + i);
					break;
					//		case ImageType::argb8:
					//			r.rgba = *(ptr.rgba + index);
					break;
				case ImageType::float32:
					r.rgba.samples[i].f = *(ptr.f + index + i);
					break;
				default:
					throw;
				}
			}
		}
		else
		{
			switch (type)
			{
			case ImageType::int8:
				r.s = *(ptr.b + index);
				break;
			case ImageType::int16:
				r.s = *(ptr.s + index);
				break;
			case ImageType::int32:
				r.i = *(ptr.i + index);
				break;
				//		case ImageType::argb8:
				//			r.rgba = *(ptr.rgba + index);
				break;
			case ImageType::float32:
				r.f = *(ptr.f + index);
				break;
			default:
				std::exception();
			}
		}

		return r;
	}

	roweltype getValueV(size_t index) const
	{
		roweltype r;
		r.type = type;
		r.sampl = samples;
		r.val = getValue(index);
		return r;
	}

	float getFloat(size_t index) const
	{
		roweltype r;
		r.type = type;
		r.val = getValue(index);
		r.sampl = MIN(samples, 3);
		return r.avg();
	}

	void copyDataFrom(uchar* buffer, int selfOffsetCount = 0, int fromCount = 0)
	{
		if (fromCount == 0)
			fromCount = count;
		uchar* out;
		size_t size = 0;
		switch (type)
		{
		case ImageType::int8:
			size = 1;
			out = ptr.b;
			break;
		case ImageType::int16:
			size = 2;
			out = reinterpret_cast<uchar*>(ptr.s);
			break;
		case ImageType::int32:
			size = 4;
			out = reinterpret_cast<uchar*>(ptr.i);
			break;
			//		case ImageType::argb8:
			//			size = 4;
			//			out = reinterpret_cast<uchar*>(ptr.rgba);
			break;
		case ImageType::float32:
			size = 4;
			out = reinterpret_cast<uchar*>(ptr.f);
			break;
		default:
			out = nullptr;
			std::exception();
		}

		//		assert()
		memcpy(out + selfOffsetCount * size * samples, buffer, fromCount * size * samples);
	}

	bool isNull() const
	{
		switch (type)
		{
		case ImageType::int8:
			return ptr.b == nullptr;
		case ImageType::int16:
			return ptr.s == nullptr;
		case ImageType::int32:
			return ptr.i == nullptr;
			//		case ImageType::argb8:
			//			return ptr.rgba == nullptr;
		case ImageType::float32:
			return ptr.f == nullptr;
		default:
			std::exception();
		}
		return false;
	}

	void setNull()
	{
		release();
		count = 0;
		switch (type)
		{
		case ImageType::int8:
			ptr.b = nullptr;
			break;
		case ImageType::int16:
			ptr.s = nullptr;
			break;
		case ImageType::int32:
			ptr.i = nullptr;
			break;
			//		case ImageType::argb8:
			//			ptr.rgba = nullptr;
			//			break;
		case ImageType::float32:
			ptr.f = nullptr;
			break;
		default:
			std::exception();
		}
	}

	void release()
	{
		if (count == 0 && ptr.i == nullptr)
			return;

		if (deletePtr)
		{
			switch (type)
			{
			case ImageType::int8:
				delete[] ptr.b;
				ptr.b = nullptr;
				break;
			case ImageType::int16:
				delete[] ptr.s;
				ptr.s = nullptr;
				break;
			case ImageType::int32:
				delete[] ptr.i;
				ptr.i = nullptr;
				break;
				//			case ImageType::argb8:
				//				delete[] ptr.rgba;
				//				ptr.rgba = nullptr;
				//				break;
			case ImageType::float32:
				delete[] ptr.f;
				ptr.f = nullptr;
				break;
			default:
				std::exception();
			}
			deletePtr = false;
		}
		count = 0;
	}

	~rowptr()
	{
		release();
	}
};


class RowPtrHolder
{
public:
	rowptr row;
	RowPtrHolder(rowptr& r) : row(std::move(r))
	{
	}
	RowPtrHolder()
	{
		row.setNull();
	}
	//s	operator rowptr() const { return row; }
};


//typedef double rowtype;
using cachedRow = RowPtrHolder*;


struct DataRect
{
	roweltype minVal;
	roweltype maxVal;
	rowptr data;
	int wid;
	int hei;
	DataRect(uchar* data = nullptr, int width = 0, int height = 0, ImageType type = ImageType::float32, int samples = 0) : wid(width), hei(height)
	{
		minVal = -9999;
		maxVal = -9999;
		this->data.set(data, type, width * height, samples);
	}
	DataRect(int width, int height, ImageType type, int samples) : wid(width), hei(height)
	{
		data.set(wid * hei, type, samples);
	}
	~DataRect() { release(); }

	DataRect(const DataRect& other) : minVal(other.minVal), maxVal(other.maxVal), wid(other.wid), hei(other.hei) /*: s(other.s)*/
	{
		assert(false);
		data.set(wid * hei, other.data.type);
		memcpy(data.getOffset(0), other.data.getOffset(0), getImgTypeSize(other.data.type));
	}

	DataRect(DataRect&& other) : minVal(other.minVal), maxVal(other.maxVal) /*: s(std::move(o.s))*/
	{
		data = std::move(other.data);
		wid = std::exchange(other.wid, 0); // leave other in valid state
		hei = std::exchange(other.hei, 0); // leave other in valid state
	}

	// copy assignment
	DataRect& operator=(const DataRect& other)
	{
		// Guard self assignment
		if (this == &other)
			return *this;

		maxVal = other.maxVal;
		minVal = other.minVal;

		if (wid == other.wid || hei == other.hei)
		{
			if (data.isNull())
				data.set(wid * hei, other.data.type);
		}
		else
		{
			wid = other.wid; // leave other in valid state
			hei = other.hei; // leave other in valid state
			release();
			data.set(wid * hei, other.data.type);
		}

		data.copyDataFrom(other.data.getBuffer());
		return *this;
	}

	// move assignment
	DataRect& operator=(DataRect&& other) noexcept
	{
		// Guard self assignment
		if (this == &other)
			return *this; // delete[]/size=0 would also be ok

		wid = std::exchange(other.wid, 0); // leave other in valid state
		hei = std::exchange(other.hei, 0); // leave other in valid state

		release();
		data = std::move(other.data);

		maxVal = other.maxVal;
		minVal = other.minVal;

		return *this;
	}


	roweltype get(int off) const { return data.getValueV(off); }
	roweltype get(int x, int y) const { return data.getValueV(y * wid + x); }

	uint minmaxX(int x)
	{
		return MAX(0, MIN(x, wid - 1));
	}

	uint minmaxY(int x)
	{
		return MAX(0, MIN(x, hei - 1));
	}

	roweltype getSafe(int x, int y) const
	{
		x = MAX(0, MIN(x, wid - 1));
		y = MAX(0, MIN(y, hei - 1));
		return data.getValueV(y * wid + x);
	}

	roweltype getSafe(int x, int y, const roweltype& defValue) const
	{
		if (x < 0 || x >= wid || y < 0 || y >= hei)
			return defValue;
		return data.getValueV(y * wid + x);
	}

	void set(int x, int y, valtype val) { data.setValue(y * wid + x, val); }

	inline DataRect clone() const
	{
		DataRect clo(wid, hei, data.type, data.samples);
		clo.data.copyDataFrom(data.getBuffer());
		clo.maxVal = maxVal;
		clo.minVal = minVal;
		return clo;
	}

	inline uint getTotal()
	{
		return wid * hei;
	}

	void zeroing() { memset(data.getBuffer(), 0, wid * hei * getImgTypeSize(data.type)); }

	void setInRow(int y, int x, uchar* inputData, int len)
	{
		//		memset(data.getBuffer(), 0, wid * hei * getTypeSize(data.type));
		memcpy(data.getOffset(wid * y + x), inputData, len * getImgTypeSize(data.type) * data.samples);
	}

	inline DataRect zeroClone() const
	{
		DataRect clo(wid, hei, data.type, data.samples);
		clo.zeroing();
		return clo;
	}

	inline void release()
	{
		data.release();
	}

	void getRect(int x, int y, int wid, int hei, DataRect& ret)
	{
		ret.wid = wid;
		ret.hei = hei;
		ret.release();

		ret.data.set(ret.wid * ret.hei, data.type);
		int typeSize = getImgTypeSize(data.type);
		memset(ret.data.getBuffer(), 0, ret.wid * ret.hei * typeSize);

		for (int j = 0; j < ret.hei; ++j)
		{
			uint cs = minmaxX(x);
			uint ce = minmaxX(x + wid);

			//j = row/y in dest;
			//j * ret.wid = rowtype pointer start at row 'j' = ret[:,j]

			//y + j  = row/y in source;
			//(y + j) * this->wid + cs
			uchar* dsa = ret.data.getOffset(j * ret.wid);// ret[:,j]
			uchar* src = ret.data.getOffset((y + j) * this->wid + cs);// ret[cs:,(y+j)]
			memcpy(dsa, src, (ce - cs) * typeSize);
		}
		//		ret.maxVal = maxval;
	}

	void genMinMax(roweltype nullVal)
	{
		minVal = maxVal;
		for (uint i = 0; i < getTotal(); ++i)
		{
			roweltype v = get(i);
			if (v == nullVal)
				continue;

			if (v > maxVal)
				maxVal = v;
			if (v < minVal)
				minVal = v;
		}
	}

	int samples()
	{
		return data.samples;
	}

	uchar* getData()
	{
		return data.ptr.b;
	}
};


//#define DEBUG_OUT

template<class T>
class PointerDel
{
public:
	void free(T val)
	{
		delete val;
	}
};

template<class T>
class PointerArrayDel
{
public:
	void free(T val)
	{
		delete[] val;
	}
};

template<class T, class DEL>
class Cache
{
	DEL deallocator;
protected:
	std::queue<int> cacheIndexs;
	ska::unordered_map<int, T> cachedData;
	size_t maxElementsSize = 16;
	size_t maxCacheSize = 10000000;
	size_t elementSize;
public:
	Cache(size_t maxElemCount = 16, size_t maxCachSize = 10000000, size_t sizeOfElement = sizeof(T)) : maxElementsSize(maxElemCount), maxCacheSize(maxCachSize), elementSize(sizeOfElement) {}

	~Cache()
	{
		clear();
	}


	void setMaxElems(size_t number)
	{
		maxElementsSize = number;
		while (cachedData.size() > number)
		{
			remove();
		}
	}
	void remove()
	{
		int ol = cacheIndexs.front();
		cacheIndexs.pop();
#ifdef DEBUG_OUT
		if (ol % 5 == 0)
			qDebug() << "get:" << ol;
#endif

		auto iter = cachedData.find(ol);
		T& val = iter->second;
		//std::exchange(iter->second, nullptr);

		deallocator.free(val);
		cachedData.erase(iter);
	}
	void setMaxSize(size_t size)
	{
		maxCacheSize = size;
		while (cachedData.size() * elementSize > size)
			remove();
	}

	void storeData(int i, const T& data)
	{
#ifdef DEBUG_OUT
		if (i % 5 == 0)
			qDebug() << "get:" << i;
#endif
		size_t s = cachedData.size();
		if (s > maxElementsSize || s * elementSize > maxCacheSize)
			remove();

		cachedData.insert(std::pair<int, T>(i, data));
		cacheIndexs.push(i);
	}

	void storeMoveData(int i, const T& data)
	{
		size_t s = cachedData.size();
		if (s > maxElementsSize || s * elementSize > maxCacheSize)
			remove();

		cachedData.insert(std::pair<int, T>(i, std::move(data)));
		cacheIndexs.push(i);
	}


	bool isInCache(int i) { return cachedData.find(i) != cachedData.end(); }

	T& getData(int i)
	{
		auto t = cachedData.find(i);
		if (t != cachedData.end())
			return t->second;
		else
			throw std::exception();

	}
	void setNull(int i, const T& value)
	{
		auto t = cachedData.find(i);
		if (t != cachedData.end())
			t->second = value;
	}

	T& getData(int i, T& defaultValue)
	{
		auto t = cachedData.find(i);
		if (t != cachedData.end())
			return t->second;
		else
			return defaultValue;
	}

	void clear()
	{
		for (auto& it : cachedData)
		{
			deallocator.free(it.second);
		}
		cachedData.clear();
		std::queue<int> empty;
		std::swap(cacheIndexs, empty);
	}
};

class ForceRowDel
{
public:
	void free(RowPtrHolder* rowHolder)
	{
		delete rowHolder;
	}
};

using RowptrCache = Cache<RowPtrHolder*, ForceRowDel>;


template<class T>
using PointerCache = Cache<T, PointerDel<T>>;

//typedef float rowtype;



//struct CoordProjection
//{
//	int wid = 1, hei = 1;
//	//GeographicTypeGeoKey
//	static barmap<int, CoordProjection> map;
//
//	static CoordProjection getCoors(int a)
//	{
//		return map.at(a);
//	}
//};

struct bytebuffer
{
	uchar* buffer = nullptr;
	size_t capasity = 0;
	void allocate(size_t newCap)
	{
		if (newCap > capasity)
		{
			if (buffer != nullptr)
				delete[] buffer;
			capasity = newCap;
			buffer = new uchar[capasity];
		}
	}

	void setLast3ToSero()
	{
		buffer[capasity - 3] = 0;
		buffer[capasity - 2] = 0;
		buffer[capasity - 1] = 0;
	}
	~bytebuffer()
	{
		if (buffer != nullptr)
			delete buffer;

		capasity = 0;
	}
};
