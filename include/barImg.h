#pragma once
//#define USE_OPENCV
#include "presets.h"
#include "point.h"

#include <memory>
#include <iterator>

INCLUDE_CV



namespace bc {

	struct EXPORT BarVec3b
	{
	public:
		uchar r, g, b;

		inline BarVec3b() : r(0), g(0), b(0)
		{ }

		inline BarVec3b(uchar _r, uchar _g, uchar _b) : r(_r), g(_g), b(_b)
		{ }
	};
	/*struct BarVec3f
	{
	public:
		float r, g, b;
	};

	struct BarVec3i
	{
	public:
		int r, g, b;
	};*/


	template<class T>
	class EXPORT BarImg
	{
		typedef T* bar_iterator;
		typedef const T* const_bar_iterator;

		int _wid;
		int _hei;
		int _channels;
		int TSize;
		bool _deleteData = true;
		CachedValue<T> cachedMax;
		CachedValue<T> cachedMin;


		void setMetadata(int width, int height, int chnls)
		{
			this->_wid = width;
			this->_hei = height;
			this->_channels = chnls;
			TSize = sizeof(T);
		}

	protected:
		T* values = nullptr;

		void valZerofy()
		{
			memset(values, 0, length() * TSize);
		}
		//same pointer
		T* valGetShadowsCopy() const
		{
			return values;
		}

		void valInit()
		{
			valDelete();
			values = new T[length()];
			_deleteData = true;
			valZerofy();
		}
		//copy
		T* valGetDeepCopy() const
		{
			T* newVals = new T[length()];
			memcpy(newVals, values, length() * TSize);
			return newVals;
		}

		void valAssignCopyOf(T* newData)
		{
			valDelete();
			values = new T[length()];
			_deleteData = true;
			
			memcpy(values, newData, length() * TSize);
		}

		void valDelete()
		{
			if (values != nullptr && _deleteData)
				delete[] values;

			cachedMax.isCached = false;
			cachedMin.isCached = false;
		}

		void valAssignInstanceOf(T* newData , bool deleteData = true)
		{
			valDelete();
			_deleteData = deleteData;
			values = newData;
		}

		//std::unique_ptr<T*> data;
	public:

		BarImg(int width, int height, int chnls = 1)
		{
			setMetadata(width, height, chnls);
			valInit();
		}


		BarImg(int width, int height, int chnls, uchar* _data, bool copy = true, bool deleteData = true)
		{
			if (copy)
			{
				copyFromData(width, height, chnls, _data);
			}
			else
				assignData(width, height, chnls, _data, deleteData);
		}

		BarImg<T>* getCopy()
		{
			BarImg<T>* nimg = new BarImg<T>(_wid, _hei, _channels, values, true, true);
			return nimg;
		}

		void assign(const BarImg<T>& copy)
		{
			assignCopyOf(copy);
		}

		BarImg(const BarImg<T>& copyImg, bool copy = true, bool deleteData = true)
		{
			if (copy)
				assignCopyOf(copyImg);
			else
				assignInstanceOf(copyImg);
		}

#ifdef USE_OPENCV
		BarImg(cv::Mat img, bool copy = true)
		{
			if (copy)
				copyFromData(img.cols, img.rows, img.channels(), img.data);
			else
				assignData(img.cols, img.rows, img.channels(), img.data);
		}
#endif // OPENCV
		~BarImg()
		{
			valDelete();
		}

		T* getData() const
		{
			return values;
		}

		T max() const
		{
			if (cachedMax.isCached)
				return cachedMax.val;

			T _max = values[0];
			for (size_t i = 1; i < length(); i++)
			{
				if (values[i] > _max)
					_max = values[i];
			}
			//cachedMax.set(_max);
			return _max;
		}

		T min() const
		{
			if (cachedMin.isCached)
				return cachedMin.val;

			T _min = values[0];
			for (size_t i = 1; i < length(); i++)
			{
				if (values[i] < _min)
					_min = values[i];
			}
			//cachedMin.set(_min);
			return _min;
		}

		void copyFromData(int width, int height, int chnls, uchar* rawData)
		{
			setMetadata(width, height, chnls);
			valAssignCopyOf(reinterpret_cast<T*>(rawData));
		}

		void assignData(int width, int height, int chnls, uchar* rawData, bool deleteData = true)
		{
			setMetadata(width, height, chnls);

			valAssignInstanceOf(reinterpret_cast<T*>(rawData), deleteData);
		}

		inline int wid() const
		{
			return _wid;
		}

		inline int hei() const
		{
			return _hei;
		}
		inline int channels() const
		{
			return _channels;
		}
		inline size_t length() const
		{
			return (size_t)(_wid)*_hei;
		}
		inline int typeSize() const
		{
			return TSize;
		}

		inline T get(int x, int y) const
		{
			return values[y * _wid + x];
		}

		inline T get(point& p) const
		{
			return values[p.y * _wid + p.x];
		}

		inline void set(int x, int y, T val)
		{
			values[y * _wid + x] = val;
		}

		inline void set(bc::point p, T val)
		{
			values[p.y * _wid + p.x] = val;
		}

		inline void setLiner(size_t pos, T val)
		{
			values[pos] = val;
		}

		inline T getLiner(size_t pos) const
		{
			return values[pos];
		}

		point getPointAt(size_t iter) const
		{
			return point(iter % _wid, iter / _wid);
		}

		void minusFrom(T min)
		{
			for (size_t i = 0; i < length(); i++)
			{
				values[i] = min - values[i];
			}
		}

		void addToMat(T value)
		{
			for (auto& sval : this)
				sval += value;
		}

		void resize(int nwid, int nhei)
		{
			if (nwid == _wid && nhei == _hei)
				return;

			_wid = nwid;
			_hei = nhei;
			valAssignInstanceOf(new T[length()]);
		}
		//Перегрузка оператора присваивания
		BarImg<T>& operator= (const BarImg<T>& drob)
		{
			assignCopyOf(drob);
			return *this;
		}

		//Overload + operator to add two Box objects.
		BarImg<T> operator+(const T& v) {
			BarImg box(this);

			box.addToMat(v);

			return box;
		}

		//// Overload + operator to add two Box objects.
		BarImg operator-(const T& v)
		{
			BarImg box(this);

			for (auto& val : box)
				val -= v;

			return box;
		}

		void assignCopyOf(const BarImg& copy)
		{
			setMetadata(copy._wid, copy._hei, copy._channels);
			valAssignCopyOf(copy.values);
		}

		void assignInstanceOf(const BarImg& inst)
		{
			setMetadata(inst._wid, inst._hei, inst._channels);
			valAssignInstanceOf(inst.values);
		}

		bar_iterator begin() { return &values[0]; }
		const_bar_iterator begin() const { return &values[0]; }
		bar_iterator end() { return &values[length()]; }
		const_bar_iterator end() const { return &values[length()]; }
	};


	static inline void split(const BarImg<BarVec3b>& src, std::vector<BarImg<uchar>>& bgr)
	{
		size_t step = static_cast<unsigned long long>(src.channels()) * src.typeSize();
		for (size_t k = 0; k < src.channels(); k++)
		{
			BarImg<uchar> ib(1, 1);
			bgr.push_back(ib);
			BarImg<uchar>& ref = bgr[k];
			ref.resize(src.wid(), src.hei());

			uchar* data = reinterpret_cast<uchar*>(ref.begin());
			for (size_t i = k; i < static_cast<unsigned long long>(src.length()) * src.typeSize(); i += step)
			{
				point p = src.getPointAt(i / src.typeSize());
				ref.setLiner(i, reinterpret_cast<uchar>(data + i));
			}
		}
	}

	template<class T, class U>
	static inline void split(const BarImg<T>& src, std::vector<BarImg<U>>& bgr)
	{
	}

	enum class BarConvert
	{
		BGR2GRAY,
		GRAY2BGR,
	};

	static inline void cvtColor(const bc::BarImg<uchar>& source, bc::BarImg<bc::BarVec3b>& dest)
	{
		dest.resize(source.wid(), source.hei());

		for (size_t i = 0; i < source.length(); ++i)
		{
			uchar u = source.getLiner(i);
			BarVec3b bgb(u, u, u);
			dest.setLiner(i, bgb);
		}
	}

	static inline void cvtColor(const bc::BarImg<bc::BarVec3b>& source, bc::BarImg<uchar>& dest)
	{
		dest.resize(source.wid(), source.hei());

		for (size_t i = 0; i < source.length(); ++i)
		{
			BarVec3b bgb = source.getLiner(i);
			uchar u = .2126 * bgb.r + .7152 * bgb.g + 0.0722 * bgb.b;
			dest.setLiner(i, u);
		}
	}
	template<class T, class U>
	static inline void cvtColor(const bc::BarImg<T>& source, bc::BarImg<U>& dest)
	{

	}

	//// note: this function is not a member function!
	template<class T>
	BarImg<T> operator+(const T& c1, BarImg<T>& c2)
	{
		// use the Cents constructor and operator+(int, int)
		// we can access m_cents directly because this is a friend function
		BarImg<T>& nimg = c2;
		return c2.addToMat(c1);
	}

	template<class T>
	BarImg<T> operator-(const T& c1, const BarImg<T>& c2)
	{
		// use the Cents constructor and operator+(int, int)
		// we can access m_cents directly because this is a friend function
		BarImg<T> ret(1, 1);
		ret.assignCopyOf(c2);
		ret.minusFrom(c1);
		return ret;
	}
}

//split
//convert
