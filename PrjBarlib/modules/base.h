#ifndef BASE_H
#define BASE_H
#define LOG
#include <vector>
#include <assert.h>
#include <string.h>
#define MEXP
//#define DISABLE_GUI
//#define RELEASE_AV

#define ENABLE_SHAPE

#ifndef DISABLE_GUI
#define ENABLE_MARKERS
#define PAINTER
#endif


typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;

//typedef unsigned long long offu64;
typedef unsigned int offu64;

typedef unsigned int objoff;


#ifndef MAX
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#endif


enum class ProcessType
{
	Otsu = 0,
	NaburPorog,
	Manuly,
	HistFromMI,
	Waterfall,
	Canny,
	Smoof,
	AvgSdvig
};
enum class AvgType
{
	AvgNsum = 512,
	AvgNsumFromMI = 1024,
	Hist = 2048,
	Manuly = 4096,
	MaxMinusMin = 8192
};


enum class glColor {
	Green = 1, // vec3(0.0, 1.0, 0.0);
	Purpure = 2, // vec3(0.54, 0.0, 0.5);
	Blue = 3, // vec3(0.0, 0.3, 0.7);
	Red = 4, //vec3(1.0, 0.0, 0.0);
	Brown = 5, //vec3(0.58, 0.29, 0.0);
	Oragne = 6, //vec3(1.0, 0.5, 0.0);
	null = 7
};


template<class T>
struct StaticArray
{
private:
	T* m_buffer = nullptr;
	uint m_size = 0;


public:
	StaticArray() {}
	StaticArray(const StaticArray& other) /*: s(other.s)*/
	{
		allocate(other.m_size);
		std::copy(other.m_buffer, other.m_buffer + other.m_size, m_buffer);
	}
	/*std::cout << "move failed!\n";*/
	StaticArray(StaticArray&& other) /*: s(std::move(o.s))*/
	{
		m_buffer = std::exchange(other.m_buffer, nullptr); // leave other in valid state
		m_size = std::exchange(other.m_size, 0);
	}

	T* data()
	{
		return m_buffer;
	}

	T* extract()
	{
		T* temp = std::exchange(m_buffer, nullptr);
		m_size = 0;

		return temp;
	}

	void allocate(uint nsize)
	{
		//		assert(size != 0);

		release();
		this->m_size = nsize;
		m_buffer = new T[nsize];
	}

	uint size() { return m_size; }

	void setData(T* newData, size_t size)
	{
		release();
		m_size = size;
		m_buffer = newData;
	}

	void setToZero()
	{
		memset(m_buffer, 0, m_size * sizeof(T));
	}

	T& operator[](std::size_t idx)
	{
		assert(idx < m_size);
		return m_buffer[idx];
	}

	// copy assignment
	StaticArray& operator=(const StaticArray& other)
	{
		// Guard self assignment
		if (this == &other)
			return *this;

		allocate(other.m_size);

		std::copy(other.m_buffer, other.m_buffer + other.m_size, m_buffer);
		return *this;
	}

	// move assignment
	StaticArray& operator=(StaticArray&& other) noexcept
	{
		// Guard self assignment
		if (this == &other)
			return *this; // delete[]/size=0 would also be ok

		m_buffer = std::exchange(other.m_buffer, nullptr); // leave other in valid state
		m_size = std::exchange(other.m_size, 0);
		return *this;
	}

	void release()
	{
		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = nullptr;
		}
		m_size = 0;
	}

	~StaticArray()
	{
		release();
	}
};

using vbuffer = StaticArray<uchar>;

#endif // BASE_H
