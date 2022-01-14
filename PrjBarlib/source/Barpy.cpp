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

template<class T>
T sqr(T A)
{
	return A * A;
}

#define xtype double
const int N = 256;
struct simpleMatrix
{
	xtype data[N * N];
	int wid, hei;
	void init()
	{
		memset(data, 0, wid * hei * sizeof(xtype));
	}

	void divine(double val)
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

	xtype get(int x, int y)
	{
		return data[y * wid + x];
	}

	void set(int x, int y, xtype val)
	{
		data[y * wid + x] = val;
	}

	void increment(int y, int x)
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


class calc
{
public:
	simpleMatrix getGLSM(const bn::ndarray& gray, int angle)
	{
		int not_zero_coun = 0;
		simpleMatrix P;
		Py_intptr_t const* strides = gray.get_strides();

		int imgWid = gray.shape(1);
		int imgHei = gray.shape(0);
		P.wid = 256;
		P.hei = 256;
		P.init();

		if (angle == 0)
		{
			for (int x = 0; x < imgWid; ++x)
			{
				for (int y = 0; y < imgHei; y++)
				{
					if (x + 1 < imgWid)
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

			for (int x = 0; x < imgWid; ++x)
			{
				for (int y = 0; y < imgHei; y++)
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
			for (int x = 0; x < imgWid; ++x)
			{
				for (int y = 0; y < imgHei; y++)
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
			for (int x = 0; x < imgWid; ++x)
			{
				for (int y = 0; y < imgHei; y++)
				{
					if (y - 1 >= 0 && x - 1 >= 0)
					{
						uchar I = *(gray.get_data() + y * strides[0] + x * strides[1]);
						uchar In = *(gray.get_data() + (y - 1) * strides[0] + (x - 1) * strides[1]);

						P.increment(I, In);
						if (I != 0)
							not_zero_coun += 1;
					}
				}
			}


		}

		P.divine((double)not_zero_coun);
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


	double mlog(double val)
	{
		return val <= 0 ? 0 : log(val);
	}

	double mlog2(double val)
	{
		return val <= 0 ? 0 : log2(val);
	}

	double msqrt(double val)
	{
		return val <= 0 ? 0 : sqrt(val);
	}


	bp::list getFitchesFAST(const bn::ndarray& gray)
	{
		Py_intptr_t const* strides = gray.get_strides();
		double ui = 0;
		double oi = 0;
		for (int i = 0; i < gray.shape(1); i++)
		{
			double sm = 0;
			for (int j = 0; j < gray.shape(0); j++)
			{
				uchar val = *(gray.get_data() + j * strides[0] + i * strides[1]);
				sm += val;
				oi += (val * i) * (val * i);
			}
			ui += sm * i;
		}

		double res[100];
		int resInt = 0;
		int angle = 0;

		int u = 0;
		for (uchar an = 0; an < 4; ++an)
		{
			simpleMatrix P = getGLSM(gray, angle);
			angle += 45;

			const int pplusSize = 2 * N - 1;
			double Pplus[pplusSize];
			memset(Pplus, 0, pplusSize * sizeof(double));
			double Pmin[N];
			memset(Pmin, 0, N * sizeof(double));

			double Px[N];
			double Py[N];
			double Q = 0;
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

			double HX = 0;
			double HY = 0;
			for (int ai = 0; ai < N; ai++)
			{
				double v_y = Py[ai];
				HY -= v_y * mlog2(v_y);

				double v_x = Px[ai];
				HX -= v_x * mlog2(v_x);
			}

			double HXY1 = 0;
			double HXY2 = 0;
			for (size_t it = 0; it < N * N; it++)
			{
				int i = it % P.wid;
				int j = it / P.wid;
				xtype Pij = P.data[it];

				HXY1 -= Pij * mlog(Px[i] * Py[j]);
				HXY2 -= Px[i] * Py[j] * mlog(Px[i] * Py[j]);
				Pplus[i + j] += Pij;
				Pmin[abs(i - j)] += Pij;
			}

			double v = 0;
			double ksum = 0.0;
			for (int k = 0; k < N; k++)
			{
				ksum += P.get(k, k) * P.get(k, k);
			}


			double f[15];
			memset(f, 0, 15 * sizeof(double));
			double g[18];
			memset(g, 0, 18 * sizeof(double));

			double rm = 0;
			for (size_t it = 0; it < N * N; it++)
			{
				int x = it % P.wid;
				int y = it / P.wid;
				xtype Pij = P.data[it];
				// 1.Угловой момент второго порядка(мера гладкости изображения
					// f1 += sqr(Pij)
					// 3.Корреляция Харалика
				f[3] += sqr(x - y) * Pij;
				// 4.Дисперсия Харалика :
				f[4] += sqr(x - u) * Pij;

				//разностный момент
				rm += Pij * (1 + sqr(y - u));
				//5.Обратный разностный момент(имеет большое значение для низко - контрастных изображений) :
				f[5] += Pij / (1 + sqr(y - u));
				// 7.Энтропя

				//9. Энтропия(мера равномерности) :
				f[9] -= Pij * mlog(Pij);

				// 1.втрогой угловой момент
				g[1] += sqr(Pij);
				// 3.Разностный момент
				g[3] += sqr(x - y) * Pij;
				// 4.Обратный разностный момент
				g[4] += Pij / (1 + sqr(x - y));
				// 7.Энтропя
				g[8] -= Pij * mlog(Pij);
				// 13.Диагональный момент
				g[14] += (Pij >= 0 ? msqrt(0.5 * abs(x - y) * Pij) : -msqrt(0.5 * abs(x - y) * (-Pij)));
				// 14.Второй диагональный момент
				g[15] += (0.5 * abs(x - y) * Pij);

				// 15.Момент произведения 1
				g[16] += (x - v) * (y - v) * Pij;
				// 16.Момент произведения 2
				g[17] += Pij - ksum / N;
			}

			double ASM = 0;
			for (int n = 0; n < N; n++)
			{
				ASM += sqr(P.get(n, n));
				//////////////////////////////////
				// 2.Контраст
				g[2] += n * n;
			}

			g[2] *= P.sum();
			double ENERGY = msqrt(ASM);

			for (int i = 1; i < pplusSize; i++)
			{
				// 8.Суммарная энтропия
				f[8] -= Pplus[i] * mlog(Pplus[i]);
				//////##############################33
				// 5Суммарное среднее
				g[5] += (i + 1) * Pplus[i];
				// 6.Суммарная энтропя
				g[6] -= Pplus[i] * mlog(Pplus[i]);

			}

			for (int i = 1; i < pplusSize; i++)
			{
				// 6.Суммарное среднее
				f[6] += i * double(Pplus[i]);

				// 7.Суммарная дисперсия :
				f[7] += (i - f[8]) * (i - f[8]) * Pplus[i];
			}

			// [1,2*n-1] Надо делать -1, т.к. индексация у массивов с 0
			for (int i = 0; i < N; i++)
			{
				f[10] += i * i * Pmin[i];
				f[11] -= i * i * double(Pmin[i]) * mlog(Pmin[i]);
				// #########################################
					// 8.Разностная энтропя
				g[9] -= Pmin[i] * mlog(Pmin[i]);
				// 9.Разностная вариация
				g[10] -= Pmin[i] * (i - g[9]) * (i - g[9]);
			}
			// 10.Корреляция 1
			g[11] = (g[8] - HXY1) / (MAX(HX, HY));

			// 11.Корреляция 2
			g[12] = msqrt(1 - exp(-2 * (HXY2 - HXY1)));


			//f[12] = (f[9] - HXY1) / MAX(HX, HY);
			f[13] = msqrt(1 - exp(-2 * HXY2 - f[9]));

			f[14] = msqrt(Q);

			// Корреляция Харалика - 2
			// Суммарная Дисперсия - 7
			// Вторая информационная мера - 13
			// Энергия
			// Разностный момент rm(5)
			// Обратный разностный момент 5
			// Разностная энтропия(расчитывается чуть по - другому) - 11
			// Коэффициент вариации 14(14 - Максимальный коэффициент корреляции)
			// среднее значение яркости - 6
			res[resInt++] = (g[1]);//0
			res[resInt++] = (g[2]);//1 ? 
			res[resInt++] = (g[3]);//2
			res[resInt++] = (g[4]);//3
			res[resInt++] = (g[5]);//4
			res[resInt++] = (g[6]);
			res[resInt++] = (g[8]);
			res[resInt++] = (g[9]);
			res[resInt++] = (g[10]);
			res[resInt++] = (g[11]);
			res[resInt++] = (g[12]);//10
			res[resInt++] = (g[13]);
			res[resInt++] = (g[14]);
			res[resInt++] = (g[15]);
			res[resInt++] = (g[16]);
			res[resInt++] = (g[17]);//15
			res[resInt++] = (f[3]);
			res[resInt++] = (f[7]);
			res[resInt++] = (f[13]);
			res[resInt++] = (ENERGY);
			res[resInt++] = (rm);//20 ?
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
};

BOOST_PYTHON_MODULE(matrix)
{
	bn::initialize();
	Py_Initialize();

	class_<calc>("calc")
		.def("getFitchesFAST", &calc::getFitchesFAST, args("grayImage"));
}

#endif // _PYD