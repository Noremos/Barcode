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
	protected:
		std::unique_ptr<T[]> data;
	public:

		BarImg(int width, int height, int chnls = 1)
		{
			_channels = chnls;
			data = std::unique_ptr<T[]>(new T[width * height]);
			TSize = sizeof(T);
			memset(getData(), 0, static_cast<unsigned long long>(width) * height * TSize);
		}

		BarImg(BarImg<T>& copy)
		{
			assignCopyOf(copy);
		}

#ifdef USE_OPENCV
		BarImg(cv::Mat img, bool copy = true)
		{
			_wid = img.cols;
			_hei = img.rows;
			chnls = img.channels();
			dataSize = sizeof(T);

			if (copy)
				memcpy(data, img.data, static_cast<unsigned long long>(_wid) * _hei * sizeof(T));
			else
				data = reinterpret_cast<T>(img.data);
		}
#endif // OPENCV

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
			return getData()[y * _wid + x];
		}

		inline T get(point& p) const
		{
			return getData()[p.y * _wid + p.x];
		}

		inline void set(int x, int y, T val)
		{
			getData()[y * _wid + x] = val;
		}

		inline void set(bc::point p, T val)
		{
			getData()[p.y * _wid + p.x] = val;
		}

		inline void setLiner(size_t pos, T val)
		{
			getData()[pos] = val;
		}

		inline T getLiner(size_t pos) const
		{
			return getData()[pos];
		}

		point getPointAt(size_t iter) const
		{
			return point(iter % _wid, iter / _wid);
		}

		void minusFrom(T min)
		{
			for (size_t i = 0; i < length(); i++)
			{
				getData()[i] = min - getData()[i];
			}
		}
		T* getData() const
		{
			return data.get();
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
			data.release();
			data = std::unique_ptr<T[]>(new T[_wid * _hei]);
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
			this->_wid = copy._wid;
			this->_hei = copy._hei;
			this->_channels = copy._channels;
			memcpy(getData(), copy.getData(), copy.TSize * copy.length());
		}

		void assignInstanceOf(const BarImg& inst)
		{
			this->_wid = inst._wid;
			this->_hei = inst._hei;
			this->_channels = inst._channels;
			data = inst.data;
		}

		bar_iterator begin() { return &getData()[0]; }
		const_bar_iterator begin() const { return &getData()[0]; }
		bar_iterator end() { return &getData()[_wid * _hei]; }
		const_bar_iterator end() const { return &getData()[_wid * _hei]; }


		static inline void split(const BarImg<BarVec3b>& src, std::vector<BarImg<uchar>>& bgr)
		{
			size_t step = static_cast<unsigned long long>(src.channels()) * src.typeSize();
			for (size_t k = 0; k < src.channels(); k++)
			{
				BarImg<uchar> ib(1, 1);
				//bgr.push_back(ib);
				//BarImg<uchar>& ref= bgr[k];
				/*ref.resize(src.wid(), src.hei());

				uchar* data = reinterpret_cast<uchar*>(ref.begin());
				for (size_t i = k; i < static_cast<unsigned long long>(src.length()) * src.typeSize(); i += step)
				{
					point p = src.getPointAt(i / src.typeSize());
					ref.setLiner(i, reinterpret_cast<uchar>(data + i));
				}*/
			}
		}

		enum class BarConvert
		{
			BGR2GRAY,
			GRAY2BGR,
		};

		static inline void cvtColor(bc::BarImg<uchar>& source, bc::BarImg<bc::BarVec3b>& dest)
		{
			/*dest.resize(source.wid(), source.hei());

			for (size_t i = 0; i < source.length(); ++i)
			{
				uchar u = source.getLiner(i);
				BarVec3b bgb(u, u, u);
				dest.setLiner(i, bgb);
			}*/
		}

		static inline void cvtColor(bc::BarImg<bc::BarVec3b>& source, bc::BarImg<uchar>& dest)
		{
			dest.resize(source.wid(), source.hei());

			for (size_t i = 0; i < source.length(); ++i)
			{
				BarVec3b bgb = source.getLiner(i);
				uchar u = .2126 * bgb.r + .7152 * bgb.g + 0.0722 * bgb.b;
				dest.setLiner(i, u);
			}
		}
	};


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
