#ifdef _PYD

#include "boost/python.hpp"
#include "boost/python/numpy.hpp"
using namespace boost::python;
namespace bp = boost::python;
namespace bn = boost::python::numpy;

#include <cstring>

typedef unsigned char uchar;

#define PY_SILENS
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#define xtype float
const int N = 256;
struct simpleMatrix
{
	xtype data[N * N];
	int wid, hei;
	void init()
	{
		memset(data, 0, wid * hei);
	}

	void divine(float val)
	{
		for (size_t i = 0; i < wid * hei; i++)
		{
			data[i] /= val;
		}
	}

	xtype sum()
	{
		xtype sum = 0;
		for (size_t i = 0; i < wid * hei; i++)
		{
			sum += data[i];
		}
		return sum;
	}

	uchar get(int x, int y)
	{
		return data[y * wid + x];
	}

	void set(int x, int y, xtype val)
	{
		data[y * wid + x] = val;
	}

	void increment(int x, int y)
	{
		xtype& val = data[y * wid + x];
		++val;
	}

	xtype sumWid(int rowIndex)
	{
		xtype* raw = data;
		xtype sum = 0;
		int index = rowIndex * wid;
		for (size_t x = 0; x < 256; x++)
		{
			sum += data[index + x];
		}
		return sum;
	}

	xtype sumHei(int colIndex)
	{
		xtype* raw = data;
		xtype sum = 0;
		for (size_t y = 0; y < 256; y++)
		{
			sum += data[y * wid + colIndex];
		}
		return sum;
	}
};

simpleMatrix getGLSM(const bn::ndarray& gray, int angle)
{
	int not_zero_coun = 0;
	simpleMatrix P;
	Py_intptr_t const* strides = gray.get_strides();

	P.wid = 256;
	P.hei = 256;
	if (angle == 0)
	{
		for (int x = 0; x < P.wid; ++x)
		{
			for (size_t y = 0; y < P.hei; y++)
			{
				if (x + 1 < P.wid)
				{
					uchar I = *(gray.get_data() + y * strides[0] + x * strides[1]);
					uchar In = *(gray.get_data() + y * strides[0] + (x + 1) * strides[1]);

					P.increment(I, In);
					if (I != 0)
						not_zero_coun += 1;
				}
			}
		}
	}

	else if (angle == 45)
	{

		for (int x = 0; x < P.wid; ++x)
		{
			for (size_t y = 0; y < P.hei; y++)
			{
				if (y - 1 >= 0 && x + 1 < P.wid)
				{
					uchar I = *(gray.get_data() + y * strides[0] + x * strides[1]);
					uchar In = *(gray.get_data() + (y - 1) * strides[0] + (x + 1) * strides[1]);

					P.increment(I, In);
					if (I != 0)
						not_zero_coun += 1;
				}
			}
		}

	}
	else if (angle == 90)
	{
		for (int x = 0; x < P.wid; ++x)
		{
			for (size_t y = 0; y < P.hei; y++)
			{
				if (y - 1 >= 0)
				{
					uchar I = *(gray.get_data() + y * strides[0] + x * strides[1]);
					uchar In = *(gray.get_data() + (y - 1) * strides[0] + x * strides[1]);

					P.increment(I, In);
					if (I != 0)
						not_zero_coun += 1;
				}
			}
		}
	}
	else
	{
		for (int x = 0; x < P.wid; ++x)
		{
			for (size_t y = 0; y < P.hei; y++)
			{
				if (y - 1 >= 0 && x - 1 >= 0)
				{
					uchar I = *(gray.get_data() + y * strides[0] + x * strides[1]);
					uchar In = *(gray.get_data() + (y - 1) * strides[0] + (x + 1) * strides[1]);

					P.increment(I, In);
					if (I != 0)
						not_zero_coun += 1;
				}
			}
		}


	}

	P.divine(not_zero_coun);
	return P;
}
//
//bp::list getChrcs(const bn::ndarray& gray)
//{
//
//	int maxLen = 256;
//	int* hist = new int[maxLen];
//	memset(hist, 0, maxLen * sizeof(int));
//
//	for (size_t i = 0; i < barlines.size(); i++)
//		++hist[static_cast<int>(barlines[i]->len())];
//
//	bp::list pyhist;
//	for (size_t i = 0; i < maxLen; i++)
//		pyhist.append(hist[i]);
//
//	delete[] hist;
//
//	return pyhist;
//}



bp::list getFitchesFAST(const bn::ndarray& gray)
{
	

	Py_intptr_t const* strides = gray.get_strides();
	float ui = 0;
	float oi = 0;
	for (int i = 0; i < gray.shape(1); i++)
	{
		float sm = 0;
		for (int j = 0; j < gray.shape(0); j++)
		{
			uchar val = *(gray.get_data() + j * strides[0] + i * strides[1]);
			sm += val;
			oi += (val * i) * (val * i);
		}
		ui += sm * i;
	}

	float res[100];
	int resInt = 0;
	int angle = 0;

	int u = 0;
	for (uchar an = 0; an < 4; ++an)
	{
		simpleMatrix P = getGLSM(gray, angle);
		angle += 45;

		const int pplusSize = 2 * N - 1;
		float Pplus[pplusSize];
		memset(Pplus, 0, pplusSize * 4);
		float Pmin[N];
		memset(Pmin, 0, N * 4);

		float Px[N];
		float Py[N];
		float Q = 0;
		int ik = 0, jk = 0;

		for (size_t i = 0; i < N; i++)
		{
			Px[i] = P.sumWid(i);
			Py[i] = P.sumHei(i);
			ik = i;
			jk = i;
			double dele = (Px[ik] * Py[i]);
			if (dele != 0.0)
			{
				Q += (P.get(i, ik) * P.get(i, jk)) / (dele);
			}
		}

		float HX = 0;
		float HY = 0;
		for (int ai = 0; ai < N; ai++)
		{
			float v_y = Py[ai];
			HY -= v_y * log2f(v_y);

			float v_x = Px[ai];
			HX -= v_x * log2(v_x);
		}

		float HXY1 = 0;
		float HXY2 = 0;
		for (size_t it = 0; it < N * N; it++)
		{
			int i = it % P.wid;
			int j = it / P.wid;
			xtype Pij = P.data[it];

			HXY1 -= Pij * log(Px[i] * Py[j]);
			HXY2 -= Px[i] * Py[j] * log(Px[i] * Py[j]);
			Pplus[i + j] += Pij;
			Pmin[abs(i - j)] += Pij;
		}

		float v = 0;
		float ksum = 0.0;
		for (int k = 0; k < N; k++)
		{
			ksum += P.get(k, k) * P.get(k, k);
		}


		float f[15];
		memset(f, 0, 15);
		float g[18];
		memset(g, 0, 18);

		float rm = 0;
		for (size_t it = 0; it < N * N; it++)
		{
			int i = it % P.wid;
			int j = it / P.wid;
			xtype Pij = P.data[it];
			// 1.Угловой момент второго порядка(мера гладкости изображения
				// f1 += sqr(Pij)
				// 3.Корреляция Харалика
			f[3] += (i - j) * (i - j) * Pij;
			// 4.Дисперсия Харалика :
			f[4] += (i - u) * (i - u) * Pij;

			//разностный момент
			rm += Pij * (1 + (i - u) * (i - u));
			//5.Обратный разностный момент(имеет большое значение для низко - контрастных изображений) :
			f[5] += Pij / (1 + (i - u) * (i - u));
			// 7.Энтропя

			//9. Энтропия(мера равномерности) :
			f[9] -= Pij * log(Pij);

			// 1.втрогой угловой момент
			g[1] += Pij * Pij;
			// 3.Разностный момент
			g[3] += (i - j) * (i - j) * Pij;
			// 4.Обратный разностный момент
			g[4] += Pij / (1 + (i - j) * (i - j));
			// 7.Энтропя
			g[8] -= Pij * log(Pij);
			// 13.Диагональный момент
			g[14] += (Pij >= 0 ? sqrt(0.5 * abs(i - j) * Pij) : -sqrt(0.5 * abs(i - j) * (-Pij)));
			// 14.Второй диагональный момент
			g[15] += (0.5 * abs(i - j) * Pij);

			// 15.Момент произведения 1
			g[16] += (i - v) * (j - v) * Pij;
			// 16.Момент произведения 2
			g[17] += Pij - ksum / N;
		}

		float ASM = 0;
		for (int n = 0; n < N; n++)
		{
			ASM += P.get(n, n) * P.get(n, n);
			//////////////////////////////////
			// 2.Контраст
			g[2] += n * n;

			g[2] *= P.sum();
		}

		float ENERGY = sqrt(ASM);

		for (int i = 1; i < pplusSize; i++)
		{
			// 8.Суммарная энтропия
			f[8] -= Pplus[i] * log(Pplus[i]);
			//////##############################33
			// 5Суммарное среднее
			g[5] += (i + 1) * Pplus[i];
			// 6.Суммарная энтропя
			g[6] -= Pplus[i] * log(Pplus[i]);

		}

		for (int i = 1; i < pplusSize; i++)
		{
			// 6.Суммарное среднее
			f[6] += i * float(Pplus[i]);

			// 7.Суммарная дисперсия :
			f[7] += (i - f[8]) * (i - f[8]) * Pplus[i];
		}

		// [1,2*n-1] Надо делать -1, т.к. индексация у массивов с 0
		for (int i = 0; i < N; i++)
		{
			f[10] += i * i * Pmin[i];
			f[11] -= i * i * float(Pmin[i]) * log(Pmin[i]);
			// #########################################
				// 8.Разностная энтропя
			g[9] -= Pmin[i] * log(Pmin[i]);
			// 9.Разностная вариация
			g[10] -= Pmin[i] * (i - g[9]) * (i - g[9]);
		}
		// 10.Корреляция 1
		g[11] = (g[8] - HXY1) / (MAX(HX, HY));

		// 11.Корреляция 2
		g[12] = sqrt(1 - exp(-2 * (HXY2 - HXY1)));


		//f[12] = (f[9] - HXY1) / MAX(HX, HY);
		f[13] = sqrt(1 - exp(-2 * HXY2 - f[9]));

		f[14] = sqrt(Q);

		// Корреляция Харалика - 2
		// Суммарная Дисперсия - 7
		// Вторая информационная мера - 13
		// Энергия
		// Разностный момент rm(5)
		// Обратный разностный момент 5
		// Разностная энтропия(расчитывается чуть по - другому) - 11
		// Коэффициент вариации 14(14 - Максимальный коэффициент корреляции)
		// среднее значение яркости - 6
		res[resInt++] = (g[1]);
		res[resInt++] = (g[2]);
		res[resInt++] = (g[3]);
		res[resInt++] = (g[4]);
		res[resInt++] = (g[5]);
		res[resInt++] = (g[6]);
		res[resInt++] = (g[8]);
		res[resInt++] = (g[9]);
		res[resInt++] = (g[10]);
		res[resInt++] = (g[11]);
		res[resInt++] = (g[12]);
		res[resInt++] = (g[13]);
		res[resInt++] = (g[14]);
		res[resInt++] = (g[15]);
		res[resInt++] = (g[16]);
		res[resInt++] = (g[17]);
		res[resInt++] = (f[3]);
		res[resInt++] = (f[7]);
		res[resInt++] = (f[13]);
		res[resInt++] = (ENERGY);
		res[resInt++] = (rm);
		res[resInt++] = (f[5]);
		res[resInt++] = (f[11]);
		res[resInt++] = (f[14]);
		res[resInt++] = (f[6]);
	}

	bp::list respy;
	for (int i = 0; i < 100; i++)
	{
		respy.append(res[i]);
	}
	return respy;
}

BOOST_PYTHON_MODULE(matrix)
{
	bn::initialize();
	Py_Initialize();

	def("getFitchesFAST", getFitchesFAST, args("grayImage"));
}

#endif // _PYD