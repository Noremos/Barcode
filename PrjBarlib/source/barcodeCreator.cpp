#include <cstddef>
#ifndef SKIP_M_INC


#include "../include/barcodeCreator.h"

#include <stack>
#include <string>
#include <assert.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iostream>
#include <map>

#include "../include/barImg.h"
#include <cmath>


#ifdef __linux
#include <climits>
#endif // __linux

#endif

using namespace bc;

#pragma warning(disable : 4996)



// static inline void split(const DatagridProvider& src, std::vector<BarImg*>& bgr)
// {
// 	size_t step = static_cast<size_t>(src.channels()) * src.typeSize();
// 	for (int k = 0; k < src.channels(); k++)
// 	{
// 		BarImg* ib = new BarImg(src.wid(), src.hei());
// 		bgr.push_back(ib);

// 		for (size_t i = 0; i < static_cast<unsigned long long>(src.length()) * src.typeSize(); i += step)
// 		{
// 			ib->setLiner(i, src.getLiner(i));
// 		}
// 	}
// }

enum class BarConvert
{
	BGR2GRAY,
	GRAY2BGR,
};


inline void cvtColorU1C2V3B(const bc::DatagridProvider& source, bc::BarImg& dest)
{
	assert(source.channels() == 1);

	dest.resize(source.wid(), source.hei());

	for (size_t i = 0; i < source.length(); ++i)
	{
		Barscalar u = source.getLiner(i);
		u.data.b3[0] = u.data.b1;
		u.data.b3[1] = u.data.b1;
		u.data.b3[2] = u.data.b1;
		u.type = BarType::BYTE8_3;
		dest.setLiner(i, u);
	}
}

inline void cvtColorV3B2U1C(const bc::DatagridProvider& source, bc::BarImg& dest)
{
	assert(dest.channels() == 1);
	dest.resize(source.wid(), source.hei());

	for (size_t i = 0; i < source.length(); ++i)
	{
		float accum = source.getLiner(i).getAvgFloat();
		dest.setLiner(i, (uchar)accum);
	}
}


struct Connect
{
	std::unordered_map<size_t, size_t> conters;
};

std::unordered_map<size_t, Connect> connections;


//****************************************B0**************************************

bool BarcodeCreator::checkAvg(const point curpix) const
{
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	float avgv = 0; short r = 0;
	for (int i = 0; i < 8; ++i)
	{
		point p(curpix + poss[i]);
		if (IS_OUT_OF_REG(p.x, p.y))
			continue;

		++r;
		avgv += workingImg->get(p.x, p.y).getAvgFloat();
	}

	float m = workingImg->get(curpix.x, curpix.y).getAvgFloat();

	return avgv / r >= m;
}

//********************************************************************************


COMPP BarcodeCreator::getPorogComp(const point& p, poidex index)
{
	auto* itr = included[index];
	if (itr && GETDIFF(curbright, workingImg->get(p.x, p.y)))
	{
		COMPP val = itr->getMaxparent();
		return (val != nullptr && val->isAlive() ? val : nullptr);
	}
	else
		return nullptr;
}


COMPP BarcodeCreator::getInclude(const size_t pos)
{
	assert(pos < totalSize);
	return included[pos] ? included[pos]->getMaxparent() : nullptr;
}


HOLEP BarcodeCreator::getHole(uint x, uint y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

	auto itr = included[wid * y + x];
	return dynamic_cast<HOLEP>(itr ? itr->getMaxparent() : nullptr);
}


HOLEP BarcodeCreator::getHole(const point& p)
{
	auto itr = included[wid * p.y + p.x];
	return dynamic_cast<HOLEP>(itr ? itr->getMaxparent() : nullptr);
}


HOLEP BarcodeCreator::tryAttach(HOLEP main, HOLEP add, point p)
{
	if (main != add && main->findCross(p, add))
	{
		//если хотя бы одна из дыр аутсайд - одна дыра умрет. Если они обе не аутсайд - соединятся в одну и одна дыра умрет.
		//еси они уба уже аутсайды, то ничего не произйдет, получается, что они живут только если обе isOut =true, т.е.
		//умирают, если хотя бы один из них false
		//flase - жива, true - мертва
		//if (main->getIsOutside() == false || add->getIsOutside() == false)
		//	--lastB;

		//как будет после соединения
		//main->setShadowOutside(main->getIsOutside() || add->getIsOutside());
		//add->setShadowOutside(main->getIsOutside());

		HOLEP ret = nullptr;// dynamic_cast<HOLEP>(attach(main, add));
		//if (ret->getIsOutside() && curbright != ret->end)
		//	ret->end = curbright;

		return ret;//все connected и first соединились в одну компоненту first
	}
	return main;
}

//****************************************B1**************************************

inline bool BarcodeCreator::checkCloserB1()
{
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
	Hole* hr = nullptr;
	point p1;
	point p2;

	// 1. Ищем дыру или делаем её из мелких;
	//после обовления дыры  к  ней можно будет присоединить все токи вокруг нее, кроме тточ, что на противоположном углу
	for (int i = 0; i < 8; ++i)
	{
		p1 = curpix + poss[i];

		if (IS_OUT_OF_REG(p1.x, p1.y))
			continue;

		poidex pind1 = p1.getLiner(wid);
		if (isContain(pind1))
		{
			if (isContain(curpix + poss[i + 1]))
				p2 = curpix + poss[i + 1];
			else if (i % 2 == 0 && isContain(curpix + poss[i + 2]))
				p2 = curpix + poss[i + 2];
			else
				continue;//если не нашли. Проверяем только потелнциальные дыры

			Hole* h1 = getHole(p1);
			Hole* h2 = getHole(p2);
			//все проверки на out в самом конце
			//вариант 1 - они принадлежат одному объекту. Не валидные могут содержать только одну компоненту, значит, этот объект валидный
			if (h1 == h2 && h1->isValid)
			{
				h1->add(curpix.getLiner(wid), curpix, curbright, curbright);
				hr = h1;
			}
			//вариант 2 - h1 - валид, h2- не валид. Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h1->isValid && !h2->isValid)
			{
				delete h2;
				hr = new Hole(curpix, p1, p2, this);

				h1->tryAdd(curpix);
				hr = tryAttach(hr, h1, curpix);
			}
			//вариант 3 - h1 - не  валид, h2- валид.Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h2->isValid && !h1->isValid)
			{
				delete h1;
				hr = new Hole(curpix, p1, p2, this);
				h2->tryAdd(curpix);
				hr = tryAttach(hr, h2, curpix);
			}
			//вариант 4 - оба не валид
			else if (!h1->isValid && !h2->isValid)//не факт, что они не валидны
			{
				//Т.К. мы уже проверили вышле, что образуется треуготльник, можно смело создаать дыру
				delete h1;
				delete h2;
				hr = new Hole(curpix, p1, p2, this);

			}
			//вариант 5 - разные дыры и они валидны CDOC{590}
			else if (h1->isValid && h2->isValid && h1 != h2)
			{
				bool add1 = h1->tryAdd(curpix);
				bool add2 = h2->tryAdd(curpix);
				if (add1 && add2)
				{
					hr = tryAttach(h1, h2, curpix);
				}
				else if (add1 && !add2)
				{
					if (h1->tryAdd(p2))
						hr = tryAttach(h1, h2, p2);
				}
				else if (!add1 && add2)
				{
					if (h2->tryAdd(p1))
						hr = tryAttach(h2, h1, p1);
				}
			}
			//isout будет false, если одна из дыр до этого не была out. Outside может поменяться из false в true, но не наоборот.
			//Т.е. isOutDo!=isoutPosle будет true, если isOutDo=false, а isOutPosle=true.
			if (hr != nullptr)
				break;
		}
	}

	if (hr == nullptr)
	{
		hr = new Hole(curpix, this);
		return false;
	}

	bool added = false;
	// 2. Добалвяем недодыры
	for (char i = 0; i < 8; ++i)
	{
		point curp = curpix + poss[i % 8];

		if (isContain(curp))
		{
			Hole* h_t = getHole(curp);
			//получена дыра
			if (h_t == hr)
				continue;

			// Сначала добавляем все недодыры
			if (h_t->isValid)
				continue;

			added = added || hr->tryAdd(curp);
		}
	}
	if (added)
	{
		// Пытаемся объединить дыры
		for (char i = 0; i < 8; ++i)
		{
			point sidep = curpix + poss[i % 8];

			if (isContain(sidep))
			{
				Hole* h_t = getHole(sidep);
				//получена дыра
				if (h_t == hr)
					continue;

				// Пытается соединить полноценные дыры
				if (!h_t->isValid)
					continue;

				if (h_t->tryAdd(curpix))
				{
					h_t = tryAttach(hr, h_t, curpix);
				}
			}
		}
	}

	return hr->isValid;
}
//********************************************************************************


struct RadiusOffs
{
	uint x, y;
	nextPoz pos;
};



float caclRgbDistance(const Barscalar& a, const Barscalar& b)
{
	return a.val_distance(b);
}


struct hsv {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1

	hsv(int r, int g, int b)
	{
		double      min, max, delta;

		min = r < g ? r : g;
		min = min < b ? min : b;

		max = r > g ? r : g;
		max = max > b ? max : b;

		v = max;                                // v
		delta = max - min;
		if (delta < 0.00001)
		{
			s = 0;
			h = 0; // undefined, maybe nan?
		}
		if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
			s = (delta / max);                  // s

			if (r >= max)                           // > is bogus, just keeps compilor happy
				h = (g - b) / delta;        // between yellow & magenta
			else if (g >= max)
				h = 2.0 + (b - r) / delta;  // between cyan & yellow
			else
				h = 4.0 + (r - g) / delta;  // between magenta & cyan

			h *= 60.0;                              // degrees

			if (h < 0.0)
				h += 360.0;

		}
		else
		{
			// if max is 0, then r = g = b = 0
			// s = 0, h is undefined
			s = 0.0;
			h = NAN;                            // its now undefined
		}
	}
};


struct CIELAB
{
	float c, a, b;

	CIELAB(int r, int g, int b)
	{
		toLab(r, g, b);
	}

	CIELAB(const Barscalar rgb)
	{
		toLab(rgb.data.b3[0], rgb.data.b3[1], rgb.data.b3[2]);
	}

	// Convert RGB to CIELAB
	void toLab(int red, int green, int blue)
	{
		// Convert RGB to XYZ
		double r = red / 255.0;
		double g = green / 255.0;
		double b = blue / 255.0;

		r = (r > 0.04045) ? pow(((r + 0.055) / 1.055), 2.4) : (r / 12.92);
		g = (g > 0.04045) ? pow(((g + 0.055) / 1.055), 2.4) : (g / 12.92);
		b = (b > 0.04045) ? pow(((b + 0.055) / 1.055), 2.4) : (b / 12.92);

		r *= 100.0;
		g *= 100.0;
		b *= 100.0;

		double x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
		double y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
		double z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;

		// Convert XYZ to CIELAB
		x /= 95.047;
		y /= 100.000;
		z /= 108.883;

		x = (x > 0.008856) ? pow(x, 0.3333) : ((16.0 * x) / 116.0);
		y = (y > 0.008856) ? pow(y, 0.3333) : ((16.0 * y) / 116.0);
		z = (z > 0.008856) ? pow(z, 0.3333) : ((16.0 * z) / 116.0);

		c = (116.0 * y) - 16.0;
		a = x - y;
		b = y - z;
	}

	// Comparison operator for sorting based on CIELAB
	bool operator<(const CIELAB& other) const
	{
		return std::tie(c, a, b) < std::tie(other.c, other.a, other.b);
	}

	// Comparison operator for sorting based on CIELAB
	bool operator<=(const CIELAB& other) const
	{
		return std::tie(c, a, b) <= std::tie(other.c, other.a, other.b);
	}
};


float caclHsvDistance(const Barscalar& a, const Barscalar& b)
{
	hsv hsv0(a.data.b3[0], a.data.b3[1], a.data.b3[2]);
	hsv hsv1(b.data.b3[0], b.data.b3[1], b.data.b3[2]);

	float dh = std::min(abs(hsv1.h - hsv0.h), 360 - abs(hsv1.h - hsv0.h)) / 180.0;
	float ds = abs(hsv1.s - hsv0.s);
	float dv = abs(hsv1.v - hsv0.v) / 255.0;

	return std::sqrt(dh * dh + ds * ds + dv * dv);
}

bc::indexCov* sortPixelsByRadius(const bc::DatagridProvider* workingImg, bc::barstruct sets, float maxRadius, size_t& toProcess)
{
	int wid = workingImg->wid();
	int hei = workingImg->hei();
	std::function<float(const Barscalar& a, const Barscalar& b)> calcDistnace;
	if (sets.trueSort && workingImg->getType() == BarType::BYTE8_3)
		calcDistnace = caclHsvDistance;
	else
		calcDistnace = caclRgbDistance;


	int totalSize = 4 * static_cast<size_t>(wid) * hei + wid + hei;
	float dist;
	bc::indexCov* data = new bc::indexCov[totalSize];

	static const RadiusOffs offs[] = {
		// rigth
		// c n
		// 0 0
		{1, 0, middleRight},

		// bottom
		// c 0
		// n 0
		{0,1, bottomCenter},


		// bottom rigth
		// c 0
		// 0 n
		{1,1, bottomRight},
	};

	// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
	// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"
	int k = 0;
	if (sets.mask)
	{
		for (int h = 0; h < hei - 1; ++h)
		{
			for (int w = 0; w < wid - 1; ++w)
			{
				int offset = wid * h + w;
				const Barscalar cur = workingImg->get(w, h);

				if (sets.mask->get(w, h) != sets.maskId)
				{
					continue;
				}

				std::optional<Barscalar> nextd[3];
				for (size_t l = 0; l < 3; l++)
				{
					const auto& off = offs[l];
					if (sets.mask->get(w + off.x, h + off.y) != sets.maskId)
					{
						nextd[l] = workingImg->get(w + off.x, h + off.y);
						dist = calcDistnace(cur, nextd[l].value());
						data[k++] = indexCov(offset, dist, off.pos);
					}
				}

				// 0 c
				// n 0
				if (nextd[0].has_value() && nextd[1].has_value())
				{
					dist = calcDistnace(nextd[0].value(), nextd[1].value());
					offset = wid * h + w + 1;
					data[k++] = indexCov(offset, dist, bottomLeft);
				}
			}
		}

		const int wd = wid - 1;
		for (int h = 0; h < hei - 1; ++h)
		{
			if (sets.mask->get(wd, h) != sets.maskId || sets.mask->get(wd, h + 1) != sets.maskId)
				continue;

			int offset = wid * h + wd;
			Barscalar cur = workingImg->get(wd, h);
			Barscalar next = workingImg->get(wd, h + 1);
			dist = calcDistnace(cur, next);
			data[k++] = indexCov(offset, dist, bottomCenter);
		}

		int hd = hei - 1;
		for (int w = 0; w < wid - 1; ++w)
		{
			if (sets.mask->get(w, hd) != sets.maskId || sets.mask->get(w + 1, hd) != sets.maskId)
				continue;

			int offset = wid * hd + w;
			const Barscalar cur = workingImg->get(w, hd);
			const Barscalar next = workingImg->get(w + 1, hd);
			dist = calcDistnace(cur, next);
			data[k++] = indexCov(offset, dist, middleRight);
		}
	}
	else
	{
		for (int h = 0; h < hei - 1; ++h)
		{
			for (int w = 0; w < wid - 1; ++w)
			{
				int offset = wid * h + w;
				const Barscalar cur = workingImg->get(w, h);

				Barscalar nextd[3];
				for (size_t l = 0; l < 3; l++)
				{
					const auto& off = offs[l];

					nextd[l] = workingImg->get(w + off.x, h + off.y);
					dist = calcDistnace(cur, nextd[l]);
					data[k++] = indexCov(offset, dist, off.pos);
				}

				// 0 c
				// n 0

				dist = calcDistnace(nextd[0], nextd[1]);
				offset = wid * h + w + 1;

				data[k++] = indexCov(offset, dist, bottomLeft);
			}
		}

		int wd = wid - 1;
		for (int h = 0; h < hei - 1; ++h)
		{
			int offset = wid * h + wd;
			const Barscalar cur = workingImg->get(wd, h);
			const Barscalar next = workingImg->get(wd, h + 1);
			dist = calcDistnace(cur, next);
			data[k++] = indexCov(offset, dist, bottomCenter);
		}

		int hd = hei - 1;
		for (int w = 0; w < wid - 1; ++w)
		{
			int offset = wid * hd + w;
			const Barscalar cur = workingImg->get(w, hd);
			const Barscalar next = workingImg->get(w + 1, hd);
			dist = calcDistnace(cur, next);
			data[k++] = indexCov(offset, dist, middleRight);
		}
	}

	assert(k < totalSize);

	// Тип не имеет занчение потому что соединяем не по яркости
	toProcess = k;
	std::sort(data, data + toProcess, [](const indexCov& a, const indexCov& b) {
		return a.dist < b.dist;
		});

	return data;
}


bc::indexCov* sortPixelsByRadius4(const bc::DatagridProvider* workingImg, bc::ProcType type, float maxRadius, size_t& toProcess)
{
	int wid = workingImg->wid();
	int hei = workingImg->hei();

	int totalSize = 2 * static_cast<size_t>(wid) * hei;
	float dist;
	bc::indexCov* data = new bc::indexCov[totalSize];
	// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
	// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме отражённой по вертикали буквы "L"

	int k = 0;
	for (int h = 0; h < hei - 1; ++h)
	{
		for (int w = 0; w < wid - 1; ++w)
		{
			int offset = wid * h + w;
			Barscalar cur = workingImg->get(w, h);
			Barscalar next;

			// rigth
			// c n
			// 0 0
			next = workingImg->get(w + 1, h);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, middleRight);

			// bottom
			// c 0
			// n 0
			next = workingImg->get(w, h + 1);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, bottomCenter);
		}
	}

	int wd = wid - 1;
	for (int h = 0; h < hei - 1; ++h)
	{
		int offset = wid * h + wd;
		Barscalar cur = workingImg->get(wd, h);
		Barscalar next;
		next = workingImg->get(wd, h + 1);
		dist = cur.val_distance(next);
		data[k++] = indexCov(offset, dist, bottomCenter);
	}

	int hd = hei - 1;
	for (int w = 0; w < wid - 1; ++w)
	{
		int offset = wid * hd + w;
		Barscalar cur = workingImg->get(w, hd);
		Barscalar next;
		next = workingImg->get(w + 1, hd);
		dist = cur.val_distance(next);
		data[k++] = indexCov(offset, dist, middleRight);
	}
	assert(k < totalSize);

	// Тип не имеет занчение потому что соединяем не по яркости
	toProcess = k;
	std::sort(data, data + toProcess, [](const indexCov& a, const indexCov& b) {
		return a.dist < b.dist;
		});

	return data;
}


int BarcodeCreator::sortOrtoPixels(bc::ProcType type, int rtoe, int off, int offDop) //
{
	int ret = 0;
	uint hist[256];//256
	uint offs[256];//256
	std::fill_n(hist, 256, 0);
	std::fill_n(offs, 256, 0);

	int offB = off;

	switch (rtoe)
	{
	case 0:
		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			//assert(drawimg.get(off, j).type == BarType::NONE);
//			drawimg.set(off, j, Barscalar(255));
			auto p = (int)workingImg->get(off, j);
			++hist[p]; //можно vector, но хз
			++ret;
		}
		break;
	case 1:
		for (int i = 0; i < workingImg->wid(); ++i)//wid
		{
			//assert(drawimg.get(i, off).type == BarType::NONE);
//			drawimg.set(i, off, Barscalar(255));
			auto p = (int)workingImg->get(i, off);
			++hist[p];//можно vector, но хз
			++ret;
		}
		break;
	case 2:
		for (int j = offDop; j < workingImg->hei() && off < workingImg->wid(); ++j)//hei
		{
			//assert(drawimg.get(off, j).type == BarType::NONE);
//			drawimg.set(off, j, Barscalar(255));
			auto p = (int)workingImg->get(off++, j);
			++hist[p];//можно vector, но хз
			++ret;
		}
		break;
	case 3:
		for (int j = offDop; j < workingImg->hei() && off >= 0; ++j)//hei
		{
			//assert(drawimg.get(off, j).type == BarType::NONE);
//			drawimg.set(off, j, Barscalar(255));
			auto p = (int)workingImg->get(off--, j);
			++hist[p];//можно vector, но хз
			++ret;
		}
		break;
	default:
		break;
	}

	off = offB;
	for (size_t i = 1; i < 256; ++i)
	{
		hist[i] += hist[i - 1];
		offs[i] = hist[i - 1];
	}


	poidex* data = new poidex[ret + 1];
	std::fill_n(data, ret + 1, poidex(UINT_MAX));

	switch (rtoe)
	{
	case 0:
		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			auto p = (int)workingImg->get(off, j);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(off, j);
		}
		break;
	case 1:
		for (int i = 0; i < workingImg->wid(); ++i)//wid
		{
			auto p = (int)workingImg->get(i, off);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(i, off);
		}
		break;
	case 2:
		for (int j = offDop; j < workingImg->hei() && off < workingImg->wid(); ++j)//hei
		{
			auto p = (int)workingImg->get(off, j);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(off++, j);
		}
		break;
	case 3:
		for (int j = offDop; j < workingImg->hei() && off >= 0; ++j)//hei
		{
			auto p = (int)workingImg->get(off, j);
			assert(offs[p] < ret);
			data[offs[p]++] = GETOFF(off--, j);
		}
		break;
	default:
		break;
	}

	this->sortedArr.reset(data);
	return ret;
}

struct myclassFromMin {
	const bc::DatagridProvider* workingImg;
	bool operator() (poidex a, poidex b)
	{
		return workingImg->getLiner(a) < workingImg->getLiner(b);
	}
};


struct myclassFromMax {
	const bc::DatagridProvider* workingImg;
	bool operator() (poidex a, poidex b)
	{
		return workingImg->getLiner(a) > workingImg->getLiner(b);
	}
};


void BarcodeCreator::sortPixels(ProcType type)
{
	switch (workingImg->getType())
	{
	case BarType::BYTE8_1:
	case BarType::BYTE8_3:
	case BarType::BYTE8_4:
	{
		if (settings.trueSort)
		{
			poidex* data = new poidex[totalSize + 1];//256

			for (size_t i = 0; i < totalSize; ++i)//wid
				data[i] = i;

			std::sort(data, &data[totalSize], [this](poidex a, poidex b) {
				return CIELAB(workingImg->getLiner(a)) < CIELAB(workingImg->getLiner(b));
				});
			delete[] data;
			break;
		}
		uint hist[256];//256
		uint offs[256];//256
		std::fill_n(hist, 256, 0);
		std::fill_n(offs, 256, 0);

		if (settings.mask)
		{
			processCount = 0;
			for (int j = 0; j < workingImg->hei(); ++j)//hei
			{
				for (int i = 0; i < workingImg->wid(); ++i)//wid
				{
					if (settings.mask->get(i, j) != settings.maskId)
						continue;

					auto p = (int)workingImg->get(i, j);
					++hist[p];//можно vector, но хз
					++processCount;
				}
			}
		}
		else
		{
			for (int j = 0; j < workingImg->hei(); ++j)//hei
			{
				for (int i = 0; i < workingImg->wid(); ++i)//wid
				{
					auto p = (int)workingImg->get(i, j).getAvgUchar();
					++hist[p];//можно vector, но хз
				}
			}
			processCount = totalSize;
		}

		for (size_t i = 1; i < 256; ++i)
		{
			hist[i] += hist[i - 1];
			offs[i] = hist[i - 1];
		}


		poidex* data = new poidex[processCount + 1];
		memset(data, 0, (processCount + 1) * sizeof(poidex));

		if (settings.mask)
		{
			for (size_t i = 0; i < totalSize; i++)
			{
				if (settings.mask->getLiner(i) != settings.maskId)
					continue;

				uchar p = workingImg->getLiner(i).getAvgUchar();
				assert(offs[p] < processCount || processCount == 0);
				data[offs[p]++] = i;
			}
		}
		else
		{
			for (size_t i = 0; i < totalSize; i++)
			{
				uchar p = workingImg->getLiner(i).getAvgUchar();
				data[offs[p]++] = i;
			}
		}

		if (type == ProcType::f255t0)
		{
			std::reverse(data, data + processCount);
		}
		this->sortedArr.reset(data);
		break;
	}
	case BarType::FLOAT32_1:
	{
		poidex* data = new poidex[totalSize + 1];//256

		for (size_t i = 0; i < totalSize; ++i)//wid
			data[i] = i;

		switch (type)
		{
		case bc::ProcType::f0t255:
		{
			myclassFromMin cmp;
			cmp.workingImg = workingImg;
			std::sort(data, &data[totalSize], cmp);
			break;
		}
		case bc::ProcType::f255t0:
		{
			myclassFromMax cmp;
			cmp.workingImg = workingImg;
			std::sort(data, &data[totalSize], cmp);
			break;
		}
		default:
			assert(false);
			break;
		}

		//for (size_t i = 1; i < totalSize - 1; ++i)//wid
		//{
		//	float v0 = static_cast<float>(workingImg->getLiner(data[i - 1]));
		//	float v2 = static_cast<float>(workingImg->getLiner(data[i]));
		//	assert(v0 >= v2);
		//}

		processCount = totalSize;
		this->sortedArr.reset(data);
		break;
	}

	default:
		assert(false);
		break;
	}
}




void BarcodeCreator::init(const bc::DatagridProvider* src, ProcType& type)
{
	wid = src->wid();
	hei = src->hei();

	needDelImg = false;
	sameStart = 0;

	if (type == ProcType::invertf0)
	{
		Barscalar mmin, mmax;
		src->maxAndMin(mmin, mmax);
		bc::BarImg* newone = new BarImg(src->wid(), src->hei());
		for (size_t i = 0; i < src->length(); ++i)
		{
			newone->setLiner(i, mmax - src->getLiner(i));
		}

		if (!originalImg)
		{
			delete src;
			src = nullptr;
		}

		originalImg = false;
		type = ProcType::f0t255;
		setWorkingImg(newone);
	}
	else
		setWorkingImg(src);

	totalSize = workingImg->length();

	//от 255 до 0
	switch (type)
	{
	case ProcType::Radius:
		geometrySortedArr.reset(sortPixelsByRadius(workingImg, settings, settings.maxRadius, this->processCount));
		sortedArr = nullptr;
		break;
	case ProcType::StepRadius:
		geometrySortedArr.reset(sortPixelsByRadius4(workingImg, type, settings.maxRadius, this->processCount));
		sortedArr = nullptr;
		break;
	case ProcType::ValueRadius:
		break;
	case ProcType::f0t255:
	case ProcType::f255t0:
	case ProcType::invertf0:
	case ProcType::experiment:
		sortPixels(type);
		break;
	default:
		assert(false);
	}
	// lastB = 0;


#ifdef ENABLE_ENERGY
	energy.reset(new int[totalSize]);
	memset(energy.get(), 0, totalSize * sizeof(int));

#endif
	included = new Include[totalSize];
	std::fill(included, included + totalSize, nullptr);

#ifdef USE_OPENCV
	if (colors.size() == 0 && settings.visualize)
	{
		for (int b = 0; b < 255; b += 20)
			for (int g = 255; g > 20; g -= 20)
				for (int r = 0; r < 255; r += 100)
					colors.push_back(cv::Vec3b(b, g, r));
	}
#endif // USE_OPENCV
}
//#include <QDebug>


void BarcodeCreator::processHole(Barcontainer* item)
{
	poidex* indexarr = sortedArr.get();
	for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
	{
		curpix = getPoint(indexarr[curIndexInSortedArr]);
		curbright = workingImg->get(curpix);

		/*	if (i == 25)
				qDebug() << "";*/
#ifdef VDEBUG
		VISUAL_DEBUG();
#else
		checkCloserB1();
#endif
	}
	// assert(((void)"ALARM! B1 is not zero", lastB == 1));

	addItemToCont(item);
	clearIncluded();
	// lastB = 0;
}


void BarcodeCreator::processComp(Barcontainer* item)
{
	poidex* indexarr = sortedArr.get();
	Barscalar prev = workingImg->get(getPoint(indexarr[0]));
	int state = 0;
	sameStart = 0;
	for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
	{
		curpoindex = indexarr[curIndexInSortedArr];
		//if (curpoindex == UINT_MAX)
		//	continue;

		curpix = getPoint(curpoindex);
		assert(curpoindex == wid * curpix.y + curpix.x);

		curbright = workingImg->get(curpix.x, curpix.y);

		if (prev.getAvgFloat() != curbright.getAvgFloat())
		{
			switch (state)
			{
			case 0:
				state = 2;
				curIndexInSortedArr = sameStart - 1;
				continue;
			case 2:
				state = 0;
				sameStart = curIndexInSortedArr;
				break;
			default:
				assert(false);
			}
			prev = curbright;
		}


		switch (state)
		{
		case 0:
			Component::passSame(this);
			if (curIndexInSortedArr == processCount - 1)
			{
				curIndexInSortedArr = sameStart - 1;
				state = 2;
			}
			break;
		case 2:
			Component::passConnections(this);
			break;
		default:
			assert(false);
		}

		//Component::process(this);

		assert(included[wid * curpix.y + curpix.x]);
	}

	//assert(((void)"ALARM! B0 is not one", lastB == 1));
	if (item)
	{
		addItemToCont(item);
		clearIncluded();
	}
	// lastB = 0;
}

// Parallel execution with function object.
struct Operator
{
	void operator()(short& pixel, const int* /*position*/) const
	{
		// Perform a simple threshold operation
		assert(pixel != 256);
		if (pixel == -1)
			pixel = 0;
		else
			pixel = 256 - pixel;
	}
};


void BarcodeCreator::addItemToCont(Barcontainer* container)
{
	if (container != nullptr)
	{
		switch (settings.returnType)
		{
		case ReturnType::barcode2d:
			computeNdBarcode(root, 2);
			break;
		case ReturnType::barcode3d:
		case ReturnType::barcode3dold:
			computeNdBarcode(root, 3);
			break;
		default:
			assert(false);
		}
	}

	root = nullptr;
}


void BarcodeCreator::VISUAL_DEBUG()
{
	checkCloserB1();
#ifdef USE_OPENCV
	if (settings.visualize)
	{
		draw("main");
		cv::waitKey(0);
	}
#endif // DEBUG
}


void BarcodeCreator::VISUAL_DEBUG_COMP()
{
	Component::process(this);
#ifdef USE_OPENCV
	if (settings.visualize)
	{
		draw("main");
		cv::waitKey(settings.waitK);
	}
#endif // DEBUG
}


void BarcodeCreator::clearIncluded()
{
	for (COMPP c : components)
	{
		//assert(c != nullptr);
		if (c != nullptr)
			delete c;
	}
	components.clear();

	if (included != nullptr)
	{
		//memset(included, 0, totalSize * sizeof(Include));
		delete[] included;
		included = nullptr;
	}


	if (needDelImg)
	{
		delete workingImg;
	}
	workingImg = nullptr;

	geometrySortedArr.reset(nullptr);
	sortedArr.reset();
}



bool compareLines(const barline* i1, const barline* i2)
{
	if (i1->len() == i2->len())
		return i1->start > i2->start;

	return (i1->len() > i2->len());
}



void BarcodeCreator::computeNdBarcode(Baritem* lines, int n)
{
	assert(n == 2 || n == 3);

	// якорная линия
	BarRoot* rootNode = nullptr;
	if (settings.createGraph)
	{
		rootNode = new BarRoot();
		rootNode->initRoot(lines);
	}

	for (COMPP c : components)
	{
		if (c == nullptr || c->resline == nullptr)
			continue;


		// {
		// 	auto& cs = connections[reinterpret_cast<size_t>(c)].conters;
		// 	size_t m = 0;
		// 	Component* mcs = nullptr;
		// 	for(auto& c : cs)
		// 	{
		// 		if (c.second > 0)
		// 			mcs = reinterpret_cast<Component*>(c.first);
		// 	}
		// 	if (mcs == nullptr)
		// 	{
		// 		mcs = reinterpret_cast<Component*>(cs.begin()->first);
		// 	}
		// 	attach(c, mcs);
		// }

		if (c->parent == nullptr)
		{
			//assert(c->isAlive() || settings.killOnMaxLen);
			c->kill(curbright);
			if (settings.createGraph)
				rootNode->addChild(c->resline);
		}

		assert(!c->isAlive());
	}
}


void BarcodeCreator::processTypeF(const bc::DatagridProvider* src, Barcontainer* item)
{
	auto p = settings.proctype;
	init(src, p);
	root = new Baritem(workingImg->wid(), type);
	item->addItem(root);
	switch (settings.comtype)
	{
	case ComponentType::Component:
	{
		switch (settings.proctype)
		{
		case ProcType::Radius:
			processCompByRadius(item);
			break;
		case ProcType::experiment:
		{
			//			cv::namedWindow("drawimg", cv::WINDOW_NORMAL);

			//			drawimg = BarImg(workingImg->wid(), workingImg->hei(), 1);
			totalSize = sortOrtoPixels(settings.proctype, 2, 0); // go down-right
			processComp();
			sortedArr.reset();

			int swid = workingImg->wid();
			int hwid = swid / 2 + swid % 2;
			for (int i = 0; i < swid; ++i)//wid Идям по диагонали
			{
				totalSize = sortOrtoPixels(settings.proctype, 0, i); // go down
				processComp();
				sortedArr.reset();

				if (i < workingImg->hei())
				{
					totalSize = sortOrtoPixels(settings.proctype, 1, i); // go rigth
					processComp();
					sortedArr.reset();
				}

				//sortOrtoPixels(settings.proctype, 2, i); // go down-right
				//processComp(item);
				if (i < hwid)
				{
					totalSize = sortOrtoPixels(settings.proctype, 3, i * 2);// go down-left
					processComp();
					sortedArr.reset();
				}
				else
				{
					totalSize = sortOrtoPixels(settings.proctype, 3, swid - 1, i - hwid);// go down-left
					processComp();
					sortedArr.reset();
				}

				// Нинии паралелные остновной, ихсодят в разыне стороны (одна по ширине, другая по высоте)

				totalSize = sortOrtoPixels(settings.proctype, 2, i);// go down-right
				processComp();
				sortedArr.reset();

				totalSize = sortOrtoPixels(settings.proctype, 2, 0, i);// go down-right
				processComp();
				sortedArr.reset();

				//				cv::imshow("drawimg", bc::convertProvider2Mat(&drawimg));
				//				cv::waitKey(0);
			}

			addItemToCont(item);
			clearIncluded();

			//			cv::waitKey(0);
		}
		break;
		case ProcType::StepRadius:
			processCompByStepRadius(item);
			break;
		case ProcType::ValueRadius:
			processByValueRadius(item);
			break;
		default:
			processComp(item);
			break;
		}
		break;
	}
	case ComponentType::Hole:
		if (settings.proctype == ProcType::Radius)
			processHoleByRadius(item);
		else
			processHole(item);
		break;
		//case  ComponentType::FullPrepair:
		//	ProcessFullPrepair(b, item);
		//	break;
		//case  ComponentType::PrepairComp:
		//	ProcessPrepComp(b, item);
		//	break;
	default:
		break;
	}

}


void BarcodeCreator::processFULL(const bc::DatagridProvider* img, Barcontainer* item)
{
	// bool rgb = (img->channels() != 1);


	//	if (settings.comtype == ComponentType::Component && rgb)
	//	{
	//		BarImg* res = new BarImg();
	//		cvtColorV3B2U1C(*img, *res);
	//		originalImg = false;
	//		needDelImg = true;
	//		type = BarType::BYTE8_1;
	//		processTypeF(settings, res, item);
	//		return;
	//	}

	switch (settings.coltype)
	{
		// To RGB
	case ColorType::rgb:
	{
		if (img->type == BarType::BYTE8_3)
			break;

		BarImg* res = new BarImg(-1, -1, 3);
		cvtColorU1C2V3B(*img, *res);
		originalImg = false;
		needDelImg = true;
		type = BarType::BYTE8_3;
		processTypeF(res, item);
		return;
	}
	case ColorType::gray:
	{
		if (img->type == BarType::BYTE8_1)
			break;

		BarImg* res = new BarImg();
		cvtColorV3B2U1C(*img, *res);
		originalImg = false;
		needDelImg = true;
		type = BarType::BYTE8_1;
		processTypeF(res, item);
		return;
	}
	default:
		break;
	}

	type = img->getType();
	originalImg = true;
	needDelImg = false;
	processTypeF(img, item);
}


bc::Barcontainer* BarcodeCreator::createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure)
{
	this->settings.createBinaryMasks = true;

	settings.checkCorrect();
	Barcontainer* cont = new Barcontainer();

	for (auto& it : structure.structs)
	{
		this->settings = it;
		processFULL(img, cont);
	}
	return cont;
}

bc::Baritem* BarcodeCreator::createBarcode(const bc::DatagridProvider* img, const barstruct& structure)
{
	settings = structure;
	Barcontainer cont;

	processFULL(img, &cont);
	return cont.exractItem(0);
}

std::unique_ptr<bc::Baritem> BarcodeCreator::create(const bc::DatagridProvider& img, const barstruct& structure)
{
	BarcodeCreator bc;
	return std::unique_ptr<bc::Baritem>(bc.createBarcode(&img, structure));
}

// ***************************************************


uchar dif(uchar a, uchar b)
{
	if (a > b)
		return a - b;
	else
		return b - a;
}



//Barcontainer* BarcodeCreator::searchHoles(float* /*img*/, int /*wid*/, int /*hei*/, float/* nullVal*/)
//{
//	return nullptr;
//}

#ifdef _PYD

bc::Barcontainer* bc::BarcodeCreator::createPysBarcode(bn::array& img, bc::BarConstructor& structure)
{
	//auto shape = img.get_shape();

	/*	int type = cv_8uc1;
	if (img.get_nd() == 3 && img.shape[2] == 3)
	image = &barimg<bcvec3b>(shape[0], shape[1], img.get_data());
	else if float
	barimg image(shape[0], shape[1], img.get_data());*/

	//cv::imshow("test", image);
	//cv::waitkey(0);
	bc::BarNdarray image(img);
	//try
	//{
	return createBarcode(&image, structure);
	//}
	//catch (const std::exception& ex)
	//{
	//printf("ERROR");
	//printf(ex.what());
	//}

	//bc::BarImg image(img.shape(1), img.shape(0), img.get_nd(), (uchar*)img.get_data(), false, false);
	//return createBarcode(&image, structure);
}
#endif
// GEOMERTY



void BarcodeCreator::processCompByRadius(Barcontainer* item)
{
	sameStart = 0;
	auto* get = geometrySortedArr.get();
	float last = get[0].dist;
	for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
	{
		const indexCov& val = get[curIndexInSortedArr];
		if (val.dist > settings.maxRadius)
		{
			break;
		}
		if (last != val.dist)
		{
			sameStart = curIndexInSortedArr;
			last = val.dist;
		}

		processRadius(val, true);
	}
	curbright = get[curIndexInSortedArr - 1].dist;



	//totalSize = workingImg->length();
	//bc::ProcType type = ProcType::f0t255;
	//sortPixels(type);
	//processComp(item);

	//assert(((void)"ALARM! B0 is not one", lastB == 1));
	addItemToCont(item);
	clearIncluded();
}


//template bc::Barcontainer<ushort>* BarcodeCreator<ushort>::createBarcode(const bc::DatagridProvider<ushort>*, const BarConstructor<ushort>&)

template<class CP>
static CP* radiusAttach(CP* first, Barscalar valueToFirst, CP* connected, Barscalar valueToSecond, BarcodeCreator* factory, float distance)
{
	switch (factory->settings.attachMode)
	{
	case AttachMode::dontTouch:
		return nullptr;

	case AttachMode::firstEatSecond:
		if (first->startIndex > connected->startIndex)
		{
			std::swap(first, connected);
			std::swap(valueToFirst, valueToSecond);
		}
		break;
	case AttachMode::secondEatFirst:
		if (first->startIndex < connected->startIndex)
		{
			std::swap(first, connected);
			std::swap(valueToFirst, valueToSecond);
		}
		break;
	case AttachMode::morePointsEatLow:
		if (first->getTotalSize() < connected->getTotalSize())
		{
			std::swap(first, connected);
			std::swap(valueToFirst, valueToSecond);
		}
		break;
	case AttachMode::createNew:
	{
		if (first->justCreated())
		{
			connected->merge(first);
			return connected;
		}
		else if (connected->justCreated())
		{
			first->merge(connected);
			return first;
		}
		else
		{
			if (distance < factory->settings.minAttachRadius)
			{
				if (first->resline->getMatrSize() > connected->resline->getMatrSize())
				{
					first->merge(connected);
				}
				else
					connected->merge(first);
			}
			else
			{
				CP* newOne = new CP(factory, distance);
				newOne->addChild(first, valueToFirst, false);
				newOne->addChild(connected, valueToSecond, false);
				return newOne;
			}
		}
	}
	default: throw;
	}

	//if (distance > 0)
	//{
	//	first->markNotSame();
	//	connected->markNotSame();
	//}

	first->addChild(connected, valueToSecond, true);
	if (first->resline)
	{
		return first;
	}
	else
		return connected;
}

struct conhash
{
	poidex p1;
	poidex p2;
	uint holeId;
};

class HoleRadius : public Component
{
public:
	HoleRadius(BarcodeCreator* root, const Barscalar& distance) : Component(root, distance)
	{ }

	void addConnection(const poidex p1, const poidex p2, const Barscalar& distance)
	{
		Barscalar value = factory->getValue(p1);
		add(p1, factory->getPoint(p1), value, distance);

		value = factory->getValue(p2);
		add(p2, factory->getPoint(p2), value, distance);
	}

	bool isSingle() const
	{
		return resline->matr.size() == 2;
	}
};


void BarcodeCreator::processCompByStepRadius(Barcontainer* item)
{
	Barscalar samax, sadym;
	workingImg->maxAndMin(sadym, samax);
	int smax = (int)samax.getAvgUchar();
	int foundSt = 0;
	settings.maxRadius = 0.f;
	for (float& i = settings.maxRadius; i <= smax; ++i)
	{
		curIndexInSortedArr = foundSt;
		foundSt = 0;
		for (; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
		{
			const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];

			if (val.dist > settings.maxRadius && foundSt == 0)
				foundSt = curIndexInSortedArr;

			processRadius(val, foundSt == 0);
		}
	}
	addItemToCont(item);
	clearIncluded();
}


void BarcodeCreator::processRadius(const indexCov& val, bool allowAttach)
{
	curpoindex = val.offset;
	curpix = getPoint(curpoindex);

	bc::point NextPoint = val.getNextPoint(curpix);
	poidex NextPindex = NextPoint.getLiner(wid);

	RadiusComponent* first = dynamic_cast<RadiusComponent*>(getComp(curpoindex));
	RadiusComponent* connected = dynamic_cast<RadiusComponent*>(getComp(NextPindex));
	curbright = val.dist;

	if (first != nullptr)
	{
		Barscalar nextColor = workingImg->get(NextPoint.x, NextPoint.y);
		//curpoindex = NextPindex;
		//curpix = NextPoint;

		//если в найденном уже есть этот элемент
		//существует ли ребро вокруг
		if (connected != nullptr)
		{
			if (first != connected)
			{
				if (!allowAttach)
					return;

				//if (val.dist > 0)
				//{
				//	first->markNotSame();
				//	connected->markNotSame();
				//}

				curbright = val.dist;
				Barscalar currentColor = workingImg->get(curpix.x, curpix.y);
				radiusAttach<RadiusComponent>(first, nextColor, connected, currentColor, this, val.dist);
			}
		}
		else
		{
			first->add(NextPindex, NextPoint, nextColor, curbright);
		}
	}
	else
	{
		Barscalar currentColor = workingImg->get(curpix.x, curpix.y);
		// Ребро не создано или не получилось присоединить
		if (connected == nullptr)
		{
			first = new RadiusComponent(this, curbright);
			Barscalar nextColor = workingImg->get(NextPoint.x, NextPoint.y);
			first->addInit(curpoindex, curpix, currentColor, NextPindex, NextPoint, nextColor);
		}
		else
		{
			connected->add(curpoindex, curpix, currentColor, curbright);
			//first = new RadiusComponent(curpoindex, currentColor, curbright, this);
		}
	}
}

void BarcodeCreator::processByValueRadius(Barcontainer* item)
{
	Barscalar samax, sadym;
	workingImg->maxAndMin(sadym, samax);
	float smax = (int)samax.getAvgFloat();

	settings.maxRadius = 0.f;
	BarImg img(workingImg->wid(), workingImg->hei(), workingImg->channels());
	for (size_t i = 0; i < workingImg->length(); i++)
		img.setLiner(i, workingImg->getLiner(i));

	for (float& i = settings.maxRadius; i <= smax; ++i)
	{
		for (auto* comp : components)
		{
			if (comp->resline == nullptr)
				continue;

			float avg[3]{ 0, 0, 0 };
			auto& matr = comp->resline->matr;
			size_t msize = matr.size();
			for (size_t i = 0; i < msize; i++)
			{
				auto sc = workingImg->get(matr[i].getPoint());
				avg[0] += sc.data.b3[0];
				avg[1] += sc.data.b3[1];
				avg[2] += sc.data.b3[2];
			}

			avg[0] /= msize;
			avg[1] /= msize;
			avg[2] /= msize;

			Barscalar avgSCl(avg[0], avg[1], avg[2]);

			for (size_t i = 0; i < msize; i++)
			{
				img.set(matr[i].getPoint(), avgSCl);
			}
		}

		geometrySortedArr.reset(sortPixelsByRadius4(&img, ProcType::ValueRadius, settings.maxRadius, processCount));
		for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
		{
			const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];

			if (val.dist > settings.maxRadius)
				break;

			processRadius(val, true);
		}

		if (curIndexInSortedArr == processCount)
			break;
	}

	addItemToCont(item);
	clearIncluded();
}


void BarcodeCreator::processHoleByRadius(Barcontainer* item)
{
	connections.clear();

	auto* get = geometrySortedArr.get();
	for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
	{
		const indexCov& val = get[curIndexInSortedArr];
		if (val.dist > settings.maxRadius)
			break;

		processHoleRadius(val);
	}
	curbright = get[curIndexInSortedArr - 1].dist;
	addItemToCont(item);
	clearIncluded();
}

constexpr size_t chash(poidex a, poidex b)
{
	if (a < b)
		std::swap(a, b);
	return (static_cast<size_t>(a) << 8) | b;
}

void BarcodeCreator::processHoleRadius(const indexCov& val)
{
	curpoindex = val.offset;
	curpix = getPoint(curpoindex);

	bc::point NextPoint = val.getNextPoint(curpix);
	poidex NextPindex = NextPoint.getLiner(wid);

	curbright = val.dist;

	const auto offsets = val.getOffsets(curpix, wid);
	bool fouund = false;
	size_t rebHash = chash(curpoindex, NextPindex);
	auto& conv = connections[rebHash];
	//assert(conv.size() < 2);

	for (auto& i : offsets)
	{
		auto holes1 = connections.find(chash(curpoindex, i));
		if (holes1 == connections.end())
			continue;

		auto holes2 = connections.find(chash(NextPindex, i));
		if (holes2 == connections.end())
			continue;

		Barscalar curColor = workingImg->get(curpix.x, curpix.y);
		Barscalar nextColor = workingImg->get(NextPoint.x, NextPoint.y);


		auto& h1co = holes1->second;
		//assert(h1co.size() <= 2);
		int sa = 0;

		for (auto& h1 : h1co.cons)
		{
			h1 = h1->getMaxparent();
			if (h1->resline == nullptr)
				continue;

			bool f = false;

			auto& h2co = holes2->second;
			//assert(h2co.size() <= 2);
			for (auto& h2 : h2co.cons)
			{
				h2 = h2->getMaxparent();
				if (h2->resline == nullptr)
					continue;

				if (h1 == h2)
				{
					conv.add(h1);
				}
				else if (h1->resline->matr.size() == 2)
				{
					h2->add(curpoindex, curpix, curColor, curbright);
					assert(h1->resline->root == nullptr);
					delete h1->resline;
					h1->resline = nullptr;

					h1 = h2;
					sa |= 1;

					conv.add(h2);
				}
				else if (h2->resline->matr.size() == 2)
				{
					h1->add(NextPindex, NextPoint, nextColor, curbright);
					assert(h2->resline->root == nullptr);
					delete h2->resline;
					h2->resline = nullptr;

					sa |= 2;
					h2 = h1;
					conv.add(h1);
				}
				else if (h1co.exists(h2) && h2co.exists(h1))
				{
					auto* hsd = radiusAttach<HoleRadius>(dynamic_cast<HoleRadius*>(h1), nextColor, dynamic_cast<HoleRadius*>(h2), curColor, this, val.dist);
					conv.add(hsd);

					h1 = h1->getMaxparent();

					f = true;
				}
				else
					continue;

				fouund = true;
			}

			if ((sa & 2) != 0)
			{
				sa &= ~2;
				h2co.clearDeleted();
			}
			//if (f)
			//	break;
		}

		if ((sa & 1) != 0)
		{
			h1co.clearDeleted();
		}
	}

	if (!fouund)
	{
		HoleRadius* firsth = new HoleRadius(this, curbright);
		firsth->addConnection(curpoindex, NextPindex, curbright);
		conv.cons.push_back(firsth);
	}

	//if (first != nullptr)
	//{
	//	if (connected != nullptr && connected != first)
	//	{
	//

	//		if (fouund)
	//		{
	//			radiusAttach<HoleRadius>(first, connected, this, settings, val.dist);
	//		}
	//		else
	//		{
	//			HoleRadius* firsth = new HoleRadius(this);
	//			firsth->addConnection(curpoindex, NextPindex);
	//		}

	//		first = nullptr;
	//	}
	//}
	//else
	//{
	//	// Ребро не создано или не получилось присоединить
	//	if (connected == nullptr)
	//	{
	//		HoleRadius* firsth = new HoleRadius(this);
	//		firsth->addConnection(curpoindex, NextPindex);
	//	}
	//	else
	//	{
	//		first = connected;
	//	}
	//}

	//bool added = false;
	//while (first)
	//{
	//	const auto offsets = val.getOffsets(curpix, wid);
	//	for (auto& i : offsets)
	//	{
	//		if (connections.count({ curpoindex, NextPindex, connected->resline->id }))
	//		{
	//			Barscalar nextColor = workingImg->get(NextPoint.x, NextPoint.y);
	//			first->add(NextPindex, NextPoint, nextColor);
	//			first = nullptr;
	//			added = true;
	//			break;
	//		}
	//	}

	//	if (!first && first->next)
	//		first = first->next;
	//	else
	//		break;
	//}

	//if (!added)
	//{
	//	HoleRadius* firsth = new HoleRadius(this);
	//	firsth->addConnection(curpoindex, NextPindex);
	//	first->next = firsth;
	//}
}
