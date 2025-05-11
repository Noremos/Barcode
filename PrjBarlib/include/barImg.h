#ifndef SKIP_M_INC
#pragma once
//#define USE_OPENCV
#include "presets.h"

#include <iterator>
#include <cassert>
#include <cstring>

#include "../extra/include_cv.h"
#include "../extra/include_py.h"
#include "barstrucs.h"
#endif

MEXP namespace bc {


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

	class EXPORT BarImg : public DatagridProvider
	{
		typedef Barscalar* bar_iterator;
		typedef const Barscalar* const_bar_iterator;
	public:
		Barscalar* values = nullptr;
	protected:

	private:
		mutable CachedValue cachedMax;
		mutable CachedValue cachedMin;

		int _wid;
		int _hei;
		int _channels;
		int TSize;

		bool _deleteData = true;
	public:
		bool diagReverce = false;
		virtual int width() { return wid(); }
		virtual int height() { return hei(); }

	protected:

		void setMetadata(int width, int height, int chnls)
		{
			this->_wid = width;
			this->_hei = height;
			this->_channels = chnls;
			if (chnls == 3)
				type = BarType::BYTE8_3;
			else if (chnls == 4)
				type = BarType::BYTE8_4;
			else
				type = BarType::BYTE8_1;
			TSize = sizeof(Barscalar);
			diagReverce = false;
		}

		void valZerofy()
		{
			std::fill(values, values + this->length(), Barscalar{0, type});
		}

		//same pointer
		Barscalar* valGetShadowsCopy() const
		{
			return values;
		}

		void valInit()
		{
			valDelete();
			values = new Barscalar[this->length()];
			_deleteData = true;
			valZerofy();
		}
		//copy
		Barscalar* valGetDeepCopy() const
		{
			Barscalar* newVals = new Barscalar[this->length()];
			for (size_t i = 0; i < this->length(); ++i)
			{
				newVals[i] = values[i];
			}
			return newVals;
		}

		void valAssignCopyOf(Barscalar* newData)
		{
			valDelete();
			values = new Barscalar[this->length()];
			_deleteData = true;
			for (size_t i = 0; i < this->length(); ++i)
			{
				values[i] = newData[i];
			}
		}

		void valDelete()
		{
			if (values != nullptr && _deleteData)
				delete[] values;

			cachedMax.isCached = false;
			cachedMin.isCached = false;
		}

		void valAssignInstanceOf(Barscalar* newData, bool deleteData = true)
		{
			valDelete();
			_deleteData = deleteData;
			values = newData;
		}

		//std::unique_ptr<Barscalar*> data;



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

		BarImg* getCopy()
		{
			BarImg* nimg = new BarImg(*this, true);
			return nimg;
		}

		void assign(const BarImg& copy)
		{
			assignCopyOf(copy);
		}

		BarImg(const BarImg& copyImg)
		{
			assignCopyOf(copyImg);
		}

		BarImg(const BarImg& copyImg, bool copy)
		{
			if (copy)
				assignCopyOf(copyImg);
			else
				assignInstanceOf(copyImg);
		}

		//Перегрузка оператора присваивания
		BarImg& operator= (const BarImg& drob)
		{
			if (&drob != this)
				assignCopyOf(drob);

			return *this;
		}

		//Перегрузка оператора присваивания
		BarImg& operator= (BarImg&& drob) noexcept
		{
			if (&drob != this)
				assignCopyOf(drob);

			return *this;
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
		//				m.at(p.y, p.x) = get(p.x, p.y);
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

		Barscalar* getData() const
		{
			return values;
		}

		void maxAndMin(Barscalar& _min, Barscalar& _max) const override
		{
			_max = values[0];
			_min = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar val = values[i];
				if (val > _max)
					_max = val;
				if (val < _min)
					_min = val;
			}
			cachedMax.set(_max);
			cachedMin.set(_min);
		}

		Barscalar max() const
		{
			if (cachedMax.isCached)
				return cachedMax.val;

			Barscalar _max = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar val = values[i];
				if (val > _max)
					_max = val;
			}

			cachedMax.set(_max);
			return _max;
		}

		Barscalar min() const
		{
			if (cachedMin.isCached)
				return cachedMin.val;

			Barscalar _min = values[0];
			for (size_t i = 1; i < this->length(); i++)
			{
				if (values[i] < _min)
					_min = values[i];
			}

			cachedMin.set(_min);
			return _min;
		}

		Barscalar MaxMinMin() const
		{
			if (cachedMin.isCached && cachedMax.isCached)
			{
				return cachedMax.val - cachedMin.val;
			}

			Barscalar _min = values[0];
			Barscalar _max = values[0];
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

		void setDataU8(int width, int height, uchar* valuesData)
		{
			setMetadata(width, height, 1);
			size_t total = static_cast<size_t>(width) * height;
			Barscalar* raw = new Barscalar[total];

			for (size_t i = 0; i < total; ++i)
			{
				raw[i].type = BarType::BYTE8_1;
				raw[i].data.b1 = valuesData[i];
			}

			valAssignInstanceOf(raw);
		}

		void copyFromRawData(int width, int height, int chnls, uchar* rawData)
		{
			setMetadata(width, height, chnls);
			valAssignCopyOf(reinterpret_cast<Barscalar*>(rawData));
		}

		void assignRawData(int width, int height, int chnls, uchar* rawData, bool deleteData = true)
		{
			setMetadata(width, height, chnls);

			valAssignInstanceOf(reinterpret_cast<Barscalar*>(rawData), deleteData);
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

		inline Barscalar get(int x, int y) const override
		{
			//if (diagReverce)
			//	return values[x * _wid + y];
			//else
			return values[y * _wid + x];
		}

		inline void set(int x, int y, Barscalar val)
		{
			//if (diagReverce)
			//	values[x * _wid + y] = val;
			//else
			values[y * _wid + x] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void set(bc::point p, Barscalar val)
		{
			//if (diagReverce)
			//	values[p.x * _wid + p.y] = val;
			//else
			values[p.y * _wid + p.x] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void add(int x, int y, Barscalar val)
		{
			//if (diagReverce)
			//	values[x * _wid + y] += val;
			//else
			values[y * _wid + x] += val;

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void minus(bc::point p, Barscalar val)
		{
			minus(p.x, p.y, val);
		}

		inline void minus(int x, int y, Barscalar val)
		{
			//assert(values[y * _wid + x] >= val);
			//if (diagReverce)
			//	values[x * _wid + y] -= val;
			//else
			values[y * _wid + x] -= val;

			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		inline void add(bc::point p, const Barscalar& val)
		{
			add(p.x, p.y, val);
		}


		inline void setLiner(size_t pos, const Barscalar& val)
		{
			values[pos] = val;
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		virtual Barscalar getLiner(size_t pos) const override
		{
			return values[pos];
		}

		void minusFrom(Barscalar min)
		{
			for (size_t i = 0; i < this->length(); i++)
			{
				values[i] = min - values[i];
			}
			cachedMin.isCached = false;
			cachedMax.isCached = false;
		}

		//void addToMat(Barscalar value)
		//{
		//	for (size_t i = 0; i < this->length(); i++)
		//	{
		//		Barscalar sval = this->getLiner(i);
		//		sval += value;
		//		assert(this->getLiner(i) == sval);
		//	}

		//	cachedMin.isCached = false;
		//	cachedMax.isCached = false;
		//}

		void resize(int nwid, int nhei)
		{
			if (nwid == _wid && nhei == _hei)
				return;

			_wid = nwid;
			_hei = nhei;
			valAssignInstanceOf(new Barscalar[this->length()], true);
		}

		inline bc::BarImg& operator+(const Barscalar& v)
		{
			BarImg* box = this->getCopy();

			//box->addToMat(v);
			assert(false);

			return *box;
		}

		//// Overload + operator to add two Box objects.
		inline bc::BarImg& operator-(const Barscalar& v)
		{
			bc::BarImg* box = this->getCopy();

			assert(false);
			for (size_t i = 0; i < box->length(); ++i)
			{
				Barscalar val = box->getLiner(i);
				val -= v;
				assert(box->getLiner(i) == val);
			}
			return *box;
		}
		//
		//bc::BarImg bc::BarImg::operator+(const Barscalar& c1, BarImg& c2)
		//{
		//	// use the Cents constructor and operator+(int, int)
		//	// we can access m_cents directly because this is a friend function
		//	BarImg nimg = c2;
		//	nimg.addToMat(c1);
		//	return nimg;
		//}
		//
		//
		//bc::BarImg bc::BarImg::operator-(const Barscalar& c1, const BarImg& c2)
		//{
		//	// use the Cents constructor and operator+(int, int)
		//	// we can access m_cents directly because this is a friend function
		//	BarImg ret(1, 1);
		//	ret.assignCopyOf(c2);
		//	ret.minusFrom(c1);
		//	return ret;
		//}


		void assignCopyOf(const BarImg& copy)
		{
			setMetadata(copy._wid, copy._hei, copy._channels);
			valAssignCopyOf(copy.values);

			this->diagReverce = copy.diagReverce;
			this->type = copy.type;
		}

		void assignInstanceOf(const BarImg& inst)
		{
			setMetadata(inst._wid, inst._hei, inst._channels);
			valAssignInstanceOf(inst.values);

			this->type = inst.type;
			this->diagReverce = inst.diagReverce;
		}

		bar_iterator begin() { return &values[0]; }
		const_bar_iterator begin() const { return &values[0]; }
		bar_iterator end() { return &values[this->length()]; }
		const_bar_iterator end() const { return &values[this->length()]; }
	};



#ifdef USE_OPENCV
	class BarMat : public DatagridProvider
	{
		mutable CachedValue cachedMax;
		mutable CachedValue cachedMin;
	public:
		Mat& mat;
		BarMat(Mat& _mat, BarType type = BarType::NONE) : mat(_mat)
		{
			if (type != BarType::NONE)
			{
				this->type = type;
			}
			else
			{
				switch (mat.type())
				{
				case CV_8UC1:
					this->type = BarType::BYTE8_1;
					break;

				case CV_8UC3:
					this->type = BarType::BYTE8_3;
					break;

					//case CV_32FC1:
					//	this->type = BarType::FLOAT;
					//	break;
				default:
					assert(false);
				}
			}
		}

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

		Barscalar get(int x, int y) const;

		//Перегрузка оператора присваивания
		BarMat& operator= (const BarMat& drob)
		{
			mat = drob.mat;
			type = drob.type;
			cachedMax = drob.cachedMax;
			cachedMin = drob.cachedMin;
			return *this;
		}


		void maxAndMin(Barscalar& min, Barscalar& max) const override
		{
			if (cachedMin.isCached && cachedMax.isCached)
			{
				min = cachedMin.val;
				max = cachedMax.val;
			}

			double amin, amax;
			cv::minMaxLoc(mat, &amin, &amax);
			min = static_cast<float>(amin);
			max = static_cast<float>(amax);
		}

		Barscalar max() const
		{
			if (cachedMax.isCached)
			{
				return cachedMax.val;
			}
			double min, max;
			cv::minMaxLoc(mat, &min, &max);

			cachedMax.set(static_cast<uchar>(max));
			cachedMin.set(static_cast<uchar>(min));
			return max;
		}

		inline size_t typeSize() const
		{
			return mat.depth();
		}
	};
#endif // USE_OPENCV



#ifdef _PYD
	class BarNdarray : public DatagridProvider
	{
	public:
		bn::array& mat;
		Py_intptr_t const* strides;

		BarNdarray(bn::array& _mat) : mat(_mat), strides(mat.strides())
		{
			std::puts("DEBUG: BarNdarray ctor");
			const auto mtype = mat.dtype();

			if (mat.ndim() == 3)
			{
				if (!mtype.is(pybind11::dtype::of<int8_t>()))
					throw pybind11::type_error("Unsupported numpy type");

				type = BarType::BYTE8_3;
				return;
			}

			if (mat.ndim() != 2)
			{
				throw pybind11::type_error("The array should be 2-dimensional");
			}

			if (mtype.is(pybind11::dtype::of<float>()))
				type = BarType::FLOAT32_1;
			else if (mtype.is(pybind11::dtype::of<int32_t>()))
				type = BarType::INT32_1;
			else if (mtype.is(pybind11::dtype::of<int8_t>()))
				type = BarType::BYTE8_1;
			else
				throw pybind11::type_error("Unsupported numpy type");

			std::cout<< "Size wid: " << mat.shape(1) << std::endl;
			std::cout<< "Size hei: " << mat.shape(0) << std::endl;
			std::cout<< "The first value: " << get(0,0).getAvgFloat() << std::endl;
		}

		~BarNdarray()
		{
			std::puts("DEBUG: BarNdarray ~dtro");
		}

		int wid() const override
		{
			return mat.shape(1);
		}

		int hei() const override
		{
			return mat.shape(0);
		}

		int channels() const override
		{
			return mat.ndim() <= 2 ? 1 : mat.shape(2);
		}

		template<class T>
		T readMatValue(int x, int y) const
		{
			return *reinterpret_cast<const T*>(mat.data(y, x));
		}

		Barscalar get(int x, int y) const override
		{
			switch (type)
			{
			case BarType::BYTE8_3:
			{
				const char* off = (const char*)mat.data() + y * strides[0] + x * strides[1];
				return Barscalar(off[0], off[1], off[2]);
			}
			case BarType::BYTE8_1:
			default:
				return Barscalar(readMatValue<uchar>(x, y), type);
			case BarType::INT32_1:
				return Barscalar(readMatValue<int32_t>(x, y), type);
			case BarType::FLOAT32_1:
				return Barscalar(readMatValue<float>(x, y), type);
			}
		}

		Barscalar max() const
		{
			if (this->length() == 0)
				return Barscalar(0, type);

			Barscalar max = this->getLiner(0);
			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar t = this->getLiner(i);
				if (t > max)
					max = t;
			}
			/*printf("max for nd: %i" + mat.attr("max"));
			return mat.attr("max"));*/
			return max;
		}

		void maxAndMin(Barscalar& min, Barscalar& max) const override
		{
			if (this->length() == 0)
				return;

			for (size_t i = 1; i < this->length(); i++)
			{
				Barscalar t = this->getLiner(i);
				if (t > max)
					max = t;
				if (t < min)
					min = t;
			}
		}

		size_t typeSize() const override
		{
			return mat.dtype().itemsize();
		}
	};

#endif // USE_OPENCV

}
//split
//convert
