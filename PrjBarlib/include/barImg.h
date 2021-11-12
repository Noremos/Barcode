#pragma once
//#define USE_OPENCV
#include "presets.h"

#include <memory>
#include <iterator>
#include <cassert>
#include <cstring>

#include "include_cv.h"
#include "include_py.h"
#include "barstrucs.h"



namespace bc {


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
	class EXPORT DatagridProvider
	{
	public:
		virtual int wid() const = 0;
		virtual int hei() const = 0;
		virtual int channels() const = 0;

		virtual void maxAndMin(T& min, T&max) const = 0;
		virtual size_t typeSize() const = 0;


		virtual T& get(int x, int y) const = 0;

		virtual T& get(point& p) const
		{
			return get(p.x, p.y);
		}

		virtual size_t length() const
		{
			return static_cast<size_t>(wid()) * hei();
		}

		// wid * y + x;
		virtual T& getLiner(size_t pos) const
		{
			return get((int)(pos % wid()), (int)(pos / wid()));
		}

		point getPointAt(size_t iter) const
		{
			return point((int)(iter % wid()), (int)(iter / wid()));
		}
		virtual ~DatagridProvider()
		{ }

	};

	template<class T>
	class EXPORT BarImg : public DatagridProvider<T>
	{
		typedef T* bar_iterator;
		typedef const T* const_bar_iterator;
	public:
		T* values = nullptr;
	protected:

	private:
		mutable CachedValue<T> cachedMax;
		mutable CachedValue<T> cachedMin;

		int _wid;
		int _hei;
		int _channels;
		int TSize;

		bool _deleteData = true;
	public:
		bool diagReverce = false;
		virtual int width(){ return wid();}
		virtual int height(){ return hei();}

	protected:

		void setMetadata(int width, int height, int chnls)
		{
			this->_wid = width;
			this->_hei = height;
			this->_channels = chnls;
			TSize = sizeof(T);
			diagReverce = false;
		}


		void valZerofy()
		{
			memset(values, 0, this->length() * TSize);
		}
		//same pointer
		T* valGetShadowsCopy() const
		{
			return values;
		}

		void valInit()
		{
			valDelete();
			values = new T[this->length()];
			_deleteData = true;
			valZerofy();
		}
		//copy
		T* valGetDeepCopy() const
		{
			T* newVals = new T[this->length()];
			memcpy(newVals, values, this->length() * TSize);
			return newVals;
		}

		void valAssignCopyOf(T* newData)
		{
			valDelete();
			values = new T[this->length()];
			_deleteData = true;
			memcpy(values, newData, this->length() * TSize);
		}

		void valDelete()
		{
			if (values != nullptr && _deleteData)
				delete[] values;

			cachedMax.isCached = false;
			cachedMin.isCached = false;
		}

		void valAssignInstanceOf(T* newData, bool deleteData = true)
		{
			valDelete();
			_deleteData = deleteData;
			values = newData;
		}

		//std::unique_ptr<T*> data;



	public:
		BarImg(int width = 1, int height = 1, int chnls = 1)
		{
			setMetadata(width, height, chnls);
			valInit();
		}


		BarImg(int width, int height, int chnls, uchar* _data, bool copy = true, bool deleteData = true)
		{
			if (copy)
			{
				copyFromRawData(width, height, chnls, _data);
			}
			else
				assignRawData(width, height, chnls, _data, deleteData);
		}

		BarImg<T>* getCopy()
		{
			BarImg<T>* nimg = new BarImg<T>(*this, true);
			return nimg;
		}

		void assign(const BarImg<T>& copy)
		{
			assignCopyOf(copy);
		}

		BarImg(const BarImg<T>& copyImg, bool copy = true)
		{
			if (copy)
				assignCopyOf(copyImg);
			else
				assignInstanceOf(copyImg);
		}

		//#ifdef USE_OPENCV
		//		BarImg(cv::Mat img, bool copy = true)
		//		{
		//			initFromMat(img, copy);
		//		}
		//
		//		void initFromMat(cv::Mat img, bool copy = true)
		//		{
		//			if (copy)
		//				copyFromRawData(img.cols, img.rows, img.channels(), img.data);
		//			else
		//				assignRawData(img.cols, img.rows, img.channels(), img.data, false);
		//			diagReverce = false;
		//		}
		//
		//		cv::Mat getCvMat()
		//		{
		//			bool re = diagReverce;
		//			diagReverce = false;
		//			cv::Mat m = cv::Mat::zeros(_hei, _wid, CV_8UC1);
		//			for (size_t i = 0; i < this->length(); i++)
		//			{
		//				auto p = getPointAt(i);
		//				m.at<T>(p.y, p.x) = get(p.x, p.y);
		//			}
		//			diagReverce = re;
		//
		//			return m;
		//		}
		//#endif // OPENCV

        virtual ~BarImg()
		{
			valDelete();
		}

		T* getData() const
		{
			return values;
		}

		void maxAndMin(T& _min, T& _max) const override
		{
			_max = values[0];
			_min = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				T val = values[i];
				if (val > _max)
					_max = val;
				if (val < _min)
					_min = val;
			}
			cachedMax.set(_max);
			cachedMin.set(_min);
		}

		T max() const
		{
			if (cachedMax.isCached)
				return cachedMax.val;

			T _max = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				T val = values[i];
				if (val > _max)
					_max = val;
			}

			cachedMax.set(_max);
			return _max;
		}

		T min() const
		{
			if (cachedMin.isCached)
				return cachedMin.val;

			T _min = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				if (values[i] < _min)
					_min = values[i];
			}

			cachedMin.set(_min);
			return _min;
		}

		T MaxMinMin() const
		{
			if (cachedMin.isCached && cachedMax.isCached)
			{
				return cachedMax.val - cachedMin.val;
			}

			T _min = values[0];
			T _max = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				if (values[i] < _min)
					_min = values[i];

				if (values[i] > _max)
					_max = values[i];
			}
			cachedMin.set(_min);
			cachedMax.set(_max);

			return _max - _min;
		}

		void copyFromRawData(int width, int height, int chnls, uchar* rawData)
		{
			setMetadata(width, height, chnls);
			valAssignCopyOf(reinterpret_cast<T*>(rawData));
		}

		void assignRawData(int width, int height, int chnls, uchar* rawData, bool deleteData = true)
		{
			setMetadata(width, height, chnls);

			valAssignInstanceOf(reinterpret_cast<T*>(rawData), deleteData);
		}

		inline int wid() const override
		{
			return _wid;
		}

		inline int hei() const override
		{
			return _hei;
		}
		inline int channels() const override
		{
			return _channels;
		}

		inline size_t typeSize() const override
		{
			return TSize;
		}

		inline T& get(int x, int y) const override
		{
			//if (diagReverce)
			//	return values[x * _wid + y];
			//else
			return values[y * _wid + x];
		}

		inline void set(int x, int y, T val)
		{
			//if (diagReverce)
			//	values[x * _wid + y] = val;
			//else
			values[y * _wid + x] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void set(bc::point p, T val)
		{
			//if (diagReverce)
			//	values[p.x * _wid + p.y] = val;
			//else
			values[p.y * _wid + p.x] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void add(int x, int y, T val)
		{
			//if (diagReverce)
			//	values[x * _wid + y] += val;
			//else
			values[y * _wid + x] += val;

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void minus(bc::point p, T val)
		{
			minus(p.x, p.y, val);
		}

		inline void minus(int x, int y, T val)
		{
			//assert(values[y * _wid + x] >= val);
			//if (diagReverce)
			//	values[x * _wid + y] -= val;
			//else
			values[y * _wid + x] -= val;

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void add(bc::point p, T val)
		{
			add(p.x, p.y, val);
		}


		inline void setLiner(size_t pos, T val)
		{
			values[pos] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		virtual T& getLiner(size_t pos) const override
		{
			return values[pos];
		}

		void minusFrom(T min)
		{
			for (size_t i = 0; i < this->length(); i++)
			{
				values[i] = min - values[i];
			}
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		void addToMat(T value)
		{
			for (size_t i = 0; i < this->length(); i++)
			{
				T& sval = this->getLiner(i);
				sval += value;
				assert(this->getLiner(i) == sval);
			}

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		void resize(int nwid, int nhei)
		{
			if (nwid == _wid && nhei == _hei)
				return;

			_wid = nwid;
			_hei = nhei;
			valAssignInstanceOf(new T[this->length()], true);
		}
		//Перегрузка оператора присваивания
		BarImg<T>& operator= (const BarImg<T>& drob)
		{
			if (&drob != this)
				assignCopyOf(drob);

			return *this;
		}

		//Overload + operator to add two Box objects.
		BarImg<T>& operator+(const T& v)
		{
			BarImg<T>* box = this->getCopy();

			box->addToMat(v);

			return *box;
		}

		//// Overload + operator to add two Box objects.
		BarImg<T>& operator-(const T& v)
		{
			BarImg<T>* box = this->getCopy();

			for (size_t i = 0; i < box->length(); ++i)
			{
				T& val = box->getLiner(i);
				val -= v;
				assert(box->getLiner(i) == val);
			}
			return *box;
		}

		void assignCopyOf(const BarImg<T>& copy)
		{
			setMetadata(copy._wid, copy._hei, copy._channels);
			valAssignCopyOf(copy.values);

			this->diagReverce = copy.diagReverce;
		}

		void assignInstanceOf(const BarImg<T>& inst)
		{
			setMetadata(inst._wid, inst._hei, inst._channels);
			valAssignInstanceOf(inst.values);

			this->diagReverce = inst.diagReverce;
		}

		bar_iterator begin() { return &values[0]; }
		const_bar_iterator begin() const { return &values[0]; }
		bar_iterator end() { return &values[this->length()]; }
		const_bar_iterator end() const { return &values[this->length()]; }
	};



#ifdef USE_OPENCV
	template <class T>
	class BarMat : public DatagridProvider<T>
	{
		mutable CachedValue<T> cachedMax;
		mutable CachedValue<T> cachedMin;
	public:
		Mat& mat;
		BarMat(Mat& _mat) : mat(_mat)
		{ }

		int wid() const
		{
			return mat.cols;
		}

		int hei() const
		{
			return mat.rows;
		}

		int channels() const
		{
			return mat.channels();
		}

		T& get(int x, int y) const;

		
		void maxAndMin(T& min, T& max) const override
		{
			if (cachedMin.isCached && cachedMax.isCached)
			{
				min = cachedMin.val;
				max = cachedMax.val;
			}

			double amin, amax;
			cv::minMaxLoc(mat, &amin, &amax);
			min = static_cast<T>(amin);
			max = static_cast<T>(amax);
		}

		T max() const
		{
			if (cachedMax.isCached)
			{
				return cachedMax.val;
			}
			double min, max;
			cv::minMaxLoc(mat, &min, &max);

			cachedMax.set(static_cast<T>(max));
			cachedMin.set(static_cast<T>(min));
			return static_cast<T>(max);
		}

		inline size_t typeSize() const
		{
			return mat.depth();
		}
	};
#endif // USE_OPENCV 



#ifdef _PYD
	template <class T>
	class BarNdarray : public DatagridProvider<T>
	{
	public:
		Py_intptr_t const* strides;

		bn::ndarray& mat;

		BarNdarray(bn::ndarray& _mat) : mat(_mat)
		{ 
			strides = _mat.get_strides();
		}
		int wid() const
		{
			return mat.shape(1);
		}

		int hei() const
		{
			return mat.shape(0);
		}

		int channels() const
		{
			return mat.get_nd() <= 2 ? 1 : mat.shape(2);
		}

		T& get(int x, int y) const
		{
			return *reinterpret_cast<T*>(mat.get_data() + y * strides[0] + x * strides[1]);
		}

		T max() const
		{
			if (this->length() == 0)
				return 0;

			T max = this->getLiner(0);
			for (size_t i = 1; i < this->length(); i++)
			{
				T t = this->getLiner(i);
				if (t > max)
					max = t;
			}
			/*printf("max for nd: %i" + mat.attr("max"));
			return mat.attr("max"));*/
			return max;
		}

		void maxAndMin(T& min, T& max) const override
		{
			if (this->length() == 0)
				return;

			for (size_t i = 1; i < this->length(); i++)
			{
				T t = this->getLiner(i);
				if (t > max)
					max = t;
				if (t < min)
					min = t;
			}
		}

		size_t typeSize() const 
		{
			return mat.get_dtype().get_itemsize();
		}
	};

	INIT_TEMPLATE_TYPE(BarNdarray)
#endif // USE_OPENCV 

	template<class T>
	static inline void split(const DatagridProvider<T>& src, std::vector<BarImg<T>*>& bgr)
	{
		size_t step = static_cast<size_t>(src.channels()) * src.typeSize();
		for (int k = 0; k < src.channels(); k++)
		{
			BarImg<T>* ib = new BarImg<T>(src.wid(), src.hei());
			bgr.push_back(ib);

			for (size_t i = 0; i < static_cast<unsigned long long>(src.length()) * src.typeSize(); i += step)
			{
				ib->setLiner(i, src.getLiner(i));
			}
		}
	}

	//template<class T, class U>
	//static inline void split(const DatagridProvider<BarVec3b>& src, std::vector<DatagridProvider<U>*>& bgr)
	//{
	//}

	enum class BarConvert
	{
		BGR2GRAY,
		GRAY2BGR,
	};

	template<class T>
	static inline void cvtColorU1C2V3B(const bc::DatagridProvider<T>& source, bc::BarImg<T>& dest)
	{
		assert(source.channels() == 1);

		dest.resize(source.wid(), source.hei());

		for (size_t i = 0; i < source.length(); ++i)
		{
			T u = source.getLiner(i);
			for (size_t c = 0; c < source.channels(); c++)
			{
				dest.setLiner(i + c, u);
			}
		}
	}
	template<class T>
	static inline void cvtColorV3B2U1C(const bc::DatagridProvider<T>& source, bc::BarImg<T>& dest)
	{
		assert(dest.channels() == 1);
		dest.resize(source.wid(), source.hei());
		int chnls = source.channels();
		double coof = 1.0 /chnls;
		for (size_t i = 0; i < source.length(); ++i)
		{
			float accum = 0;
			accum += static_cast<float>(source.getLiner(i));
			dest.setLiner(i, accum);
		}
	}
	//template<class T, class U>
	//static inline void cvtColor(const bc::DatagridProvider<T>& source, bc::DatagridProvider<U>& dest)
	//{

	//}

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

#ifdef USE_OPENCV
	template<class T>
	static cv::Mat convertProvider2Mat(DatagridProvider<T>* img)
	{
		cv::Mat m = cv::Mat::zeros(img->hei(), img->wid(), CV_8UC1);
		for (size_t i = 0; i < img->length(); i++)
		{
			auto p = img->getPointAt(i);
			m.at<T>(p.y, p.x) = img->get(p.x, p.y);
		}
		return m;
	}
	INIT_TEMPLATE_TYPE(BarMat)

#endif // USE_OPENCV

	INIT_TEMPLATE_TYPE(DatagridProvider)
	INIT_TEMPLATE_TYPE(BarImg)
}
//split
//convert
