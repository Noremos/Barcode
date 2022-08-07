#include "barcodeCreator.h"
#include <stack>
#include <string>
#include <assert.h>
#include <algorithm>

using namespace bc;

#pragma warning(disable : 4996)


void BarcodeCreator::draw(std::string name)
{
#ifdef USE_OPENCV

	int wd = wid * 10;
	int hi = hei * 10;
	cv::Mat img(hi, wd, CV_8UC3, cv::Scalar(255, 255, 255));
	cv::Vec3b v(100, 100, 100);
	size_t size = colors.size();
	//если 2 занимают одну клтку

	for (int i = 0; i < hi; i += 10)
		for (int j = 0; j < wd; j++) {
			img.at<cv::Vec3b>(i, j) = v;
			img.at<cv::Vec3b>(i + 1, j) = v;
		}


	for (int j = 0; j < wd; j += 10)
		for (int i = 0; i < hi; i++) {
			img.at<cv::Vec3b>(i, j) = v;
			img.at<cv::Vec3b>(i, j + 1) = v;
		}

	for (size_t i = 0; i < totalSize; i++) {
		//		Hole *phole = dynamic_cast<Hole *>(included[i]);
		//		if (phole == nullptr)
		//			continue;
		COMPP comp = getInclude(i);
		if (comp == nullptr || !comp->isAlive())
			continue;
		int x = static_cast<int>(i % wid);
		int y = static_cast<int>(i / wid);
		int tic = 1;
		int marc = cv::MARKER_TILTED_CROSS;
		cv::Vec3b col;

		cv::Point p(x, y);
		//		if (!phole->isValid)
		//		{
		//			marc = cv::MARKER_DIAMOND;
		//			col = cv::Vec3b(0, 0, 255);
		//		}
		//		else if (phole->getIsOutside())
		//		{
		//			col = cv::Vec3b(0, 0, 10);
		//			tic = 2;
		//			marc = cv::MARKER_CROSS;
		//		}
		//		else
		{
			Hole* hd = dynamic_cast<Hole*>(comp);
			if (hd && hd->getIsOutside())
				col = cv::Vec3b(0, 0, 0);
			else
				col = colors[(size_t)comp->startIndex % size];

			marc = cv::MARKER_TILTED_CROSS;
		}

		p.x = p.x * 10 + 5;
		p.y = p.y * 10 + 5;
		cv::drawMarker(img, p, col, marc, 10, tic, cv::LINE_4);

	}
	cv::namedWindow(name, cv::WINDOW_GUI_EXPANDED);
	cv::imshow(name, img);

	const int corWin = 600;
	const int corHei = 500;
	if (wd > hi)
	{
		float ad = (float)corWin / wd;
		cv::resizeWindow(name, corWin, (int)(hi * ad));
	}
	else
	{
		float ad = (float)corHei / hi;
		cv::resizeWindow(name, (int)(wd * ad), corHei);
	}
#endif // USE_OPENCV
}


inline COMPP BarcodeCreator::attach(COMPP main, COMPP second)
{
	if (main->liveLen() == 0 && second->liveLen() == 0)
	{
#ifdef POINTS_ARE_AVAILABLE
		for (const auto& val : second->resline->matr)
		{
			assert(workingImg->get(val.getX(wid), val.getY(wid)) == curbright);
			assert(included[val.getIndex()] == second);

			main->add(val.getIndex());
		}
#else
		//Чем больше startIndex, тем меньше перебирать. Надо, чтобы second была с большим
		if (second->startIndex < main->startIndex)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}

		for (size_t rind = 0, totalm = second->resline->matr.size(); rind < totalm; ++rind)
		{
			const barvalue& val = second->resline->matr[rind];
			const bc::point p = val.getPoint();
			main->add(p.getLiner(wid), p, true);
		}

		main->startIndex = MIN(second->startIndex, main->startIndex);
#endif // POINTS_ARE_AVAILABLE
		delete second->resline;
		second->resline = nullptr;

		return main;
	}

	//float bottom = (float)main->getStart();
	//float top = (float)main->getLast();
	//if (bottom > top)
	//	std::swap(bottom, top);

	//float bottm2 = (float)second->getStart();
	//float top2 = (float)second->getLast();
	//if (bottm2 > top2)
	//	std::swap(bottm2, top2);


	Barscalar fs = main->getStart();
	Barscalar sc = second->getStart();
	Barscalar diff = (fs > sc) ? (fs - sc) : (sc - fs);
	if (diff > settings.getMaxLen())
	{
		return main;
	}

	/*if (!((bottom <= bottm2 && bottm2 <= top) || (bottom <= top2 && top2 <= top)))
		return main;*/

	switch (settings.attachMode)
	{
	case AttachMode::dontTouch:
		return main;

	case AttachMode::secondEatFirst:
		if (main->startIndex < second->startIndex)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);
		break;

	case AttachMode::createNew:
		//if ((double)MIN(main->getTotalSize(), second->getTotalSize()) / totalSize > 0.05)
	{
		COMPP newOne = new Component(this, true);
		main->setParent(newOne);
		second->setParent(newOne);
		//main->kill();
		//second->kill();
		return newOne;
	}
	case AttachMode::morePointsEatLow:
		if (main->getTotalSize() < second->getTotalSize())
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);
		return main;
		// else pass down

	case AttachMode::firstEatSecond:
	default:
		if (main->startIndex > second->startIndex)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);

		if (main->getLastRowSize() >= settings.colorRange)
		{
			main->kill();
			return new Component(this, true);
		}

		break;
	}
	//возращаем единую компоненту.
	return main;
}

#include <iostream>
//****************************************B0**************************************

inline bool BarcodeCreator::checkCloserB0()
{
	COMPP first = nullptr;
	COMPP connected;// = new rebro(x, y); //included[{(int)i, (int)j}];
	//TODO выделять паять заранее
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	//first = getComp(curpix);
	// FIXME first always will be nill
	for (int i = 0; i < 8; ++i)
	{
		point IcurPoint(curpix + poss[i]);

		if (IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
			continue;


		poidex IcurPindex = IcurPoint.getLiner(wid);

		connected = getPorogComp(IcurPoint, IcurPindex);
		if (connected != nullptr)//существует ли ребро вокруг
		{
			if (first == nullptr)
			{
				first = connected;
				// if len more then maxlen, kill the component
				bool more = settings.maxLen.isCached && curbright.absDiff(first->getStart()) > settings.maxLen.getOrDefault(0);
				if (more)
				{
					//qDebug() << first->num << " " << curbright << " " << settings.maxLen.getOrDefault(0);
					if (settings.killOnMaxLen)
					{
						first->kill(); //Интересный результат
					}
					first = nullptr;
				}
				else if (!first->add(curpoindex, curpix))
					first = nullptr;
				//setInclude(midP, first);//n--nt обяз нужно
			}
			else
			{
				if (first->isContain(IcurPindex))//если в найденном уже есть этот элемент
					continue;

				//lastB -= 1;
				if (first != connected && first->canBeConnected(IcurPoint))
					first = attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
			}
		}
	}

	if (first == nullptr)
	{
		//lastB += 1;

		first = new Component(curpoindex, this);
		return true;
	}
	return false;
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
		main->setShadowOutside(main->getIsOutside() || add->getIsOutside());
		add->setShadowOutside(main->getIsOutside());

		HOLEP ret = dynamic_cast<HOLEP>(attach(main, add));
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
				h1->add(curpix);
				hr = h1;
			}
			//вариант 2 - h1 - валид, h2- не валид. Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h1->isValid && !h2->isValid)
			{
				hr = new Hole(curpix, p1, p2, this);

				delete h2;
				h1->tryAdd(curpix);
				hr = tryAttach(hr, h1, curpix);
			}
			//вариант 3 - h1 - не  валид, h2- валид.Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h2->isValid && !h1->isValid)
			{
				hr = new Hole(curpix, p1, p2, this);
				delete h1;
				h2->tryAdd(curpix);
				hr = tryAttach(hr, h2, curpix);
			}
			//вариант 4 - оба не валид
			else if (!h1->isValid && !h2->isValid)//не факт, что они не валидны
			{
				//Т.К. мы уже проверили вышле, что образуется треуготльник, можно смело создаать дыру
				hr = new Hole(curpix, p1, p2, this);
				delete h1;
				delete h2;

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

	for (int i = 0; i < 16; ++i)
	{
		point curp = curpix + poss[i % 8];

		if (isContain(curp))
		{
			Hole* h_t = getHole(curp);
			//получена дыра
			if (h_t == hr)
				continue;

			Hole* h2 = nullptr;

			point next = curpix + poss[(i % 8) + 1];
			if (isContain(next))
				h2 = getHole(next);
			if (h2 == hr || h2 == h_t)
				h2 = nullptr;

			if (!h_t->isValid)
			{
				if (h2 != nullptr && !h2->isValid)
				{
					delete h_t;
					delete h2;
					new Hole(curpix, curp, next, this);
				}
				else if (hr->tryAdd(curp))
					delete h_t;
			}

			//вариант 2 - она валидна
			else
			{
				bool add_t = h_t->tryAdd(curpix);
				bool add_r = hr->tryAdd(curp);
				if (h2 != nullptr)
				{
					bool added = h_t->tryAdd(next);
					if (h2->isValid)
					{
						h_t = tryAttach(h_t, h2, next);
					}
					else
					{
						if (added)
							delete h2;
					}
				}
				if (add_t && add_r)
					hr = tryAttach(hr, h_t, curpix);
				else if (add_t && !add_r)
				{
					hr = tryAttach(hr, h_t, curpix);
				}
				else if (!add_t && add_r)
				{
					hr = tryAttach(hr, h_t, curp);
				}

			}
		}
	}

	return hr->isValid;
}
//********************************************************************************



bc::indexCov* sortPixelsByRadius(const bc::DatagridProvider* workingImg, size_t& totalSize, bc::ProcType type, float maxRadius, size_t& toProcess)
{
	int wid = workingImg->wid();
	int hei = workingImg->hei();

	totalSize = 4 * static_cast<size_t>(wid) * hei + wid + hei;
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

			// bottom rigth
			// c 0
			// 0 n
			next = workingImg->get(w + 1, h + 1);
			dist = cur.val_distance(next);

			data[k++] = indexCov(offset, dist, bottomRight);


			// 0 c
			// n 0
			cur = workingImg->get(w + 1, h);
			next = workingImg->get(w, h + 1);
			dist = cur.val_distance(next);
			offset = wid * h + w + 1;

			data[k++] = indexCov(offset, dist, bottomLeft);
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
	// Тип не имеет занчение потому что соединяем не по яркости
	toProcess = k;
	std::sort(data, data + toProcess, [](const indexCov& a, const indexCov& b) {
		return a.dist < b.dist;
		});

	return data;
}


inline void BarcodeCreator::sortPixels(bc::ProcType type)
{
	switch (workingImg->getType())
	{
	case BarType::BYTE8_1:
	{
		uint hist[256];//256
		uint offs[256];//256
		std::fill_n(hist, 256, 0);
		std::fill_n(offs, 256, 0);

		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			for (int i = 0; i < workingImg->wid(); ++i)//wid
			{
				auto p = (int)workingImg->get(i, j);
				++hist[p];//можно vector, но хз
			}
		}

		for (size_t i = 1; i < 256; ++i)
		{
			hist[i] += hist[i - 1];
			offs[i] = hist[i - 1];
		}


		poidex* data = new poidex[totalSize + 1];//256
		for (size_t i = 0; i < totalSize; i++)
		{
			uchar p = workingImg->getLiner(i).data.b1;
			data[offs[p]++] = i;
		}

		if (type == ProcType::f255t0)
		{
			std::reverse(data, data + totalSize);
		}
		this->sortedArr = data;
		break;
	}
	case BarType::BYTE8_3:
	{
		assert(false);
		break;
	}
	//case BarType::FLOAT:
	//{
		//// do this hack to skip constructor calling for every point
//poidex* data = new poidex[totalSize + 1];//256

////point * data = new point[total];

//for (size_t i = 0; i < totalSize; ++i)//wid
//	data[i] = i;

//myclassFromMin cmp;
//cmp.workingImg = workingImg;
//std::sort(data, &data[totalSize], cmp);
//if (type == ProcType::f255t0)
//{
//	for (size_t i = 1; i < totalSize - 1; ++i)//wid
//	{
//		float v0 = static_cast<float>(workingImg->getLiner(data[i - 1]));
//		float v2 = static_cast<float>(workingImg->getLiner(data[i]));
//		assert(v0 >= v2);
//	}
//}

//this->sortedArr = data;
	//	break;
	//}

	default:
		break;
	}
}

struct myclassFromMin {
	const bc::DatagridProvider* workingImg;
	bool operator() (poidex& a, poidex& b)
	{
		return workingImg->getLiner(a) < workingImg->getLiner(b);
	}
};


struct myclassFromMax {
	const bc::DatagridProvider* workingImg;
	bool operator() (poidex& a, poidex& b)
	{
		return workingImg->getLiner(a) > workingImg->getLiner(b);
	}
};

#include<map>

//
//inline point* BarcodeCreator::sort()
//{
//	std::map<T, int> hist;
//	std::unordered_map<T, int> offs;

//	for (int i = 0; i < workingImg->wid(); ++i)//wid
//	{
//		for (int j = 0; j < workingImg->hei(); ++j)//hei
//		{
//			T& p = workingImg->get(i, j);
//			if (hist.find(p) != hist.end())
//			{
//				++hist[p];
//			}
//			else
//				hist.insert(std::pair<T, int>(p, 1));
//		}
//	}

//	Barscalar prev;
//	bool f = false;
//	//auto const& [key, val]
//	for (auto const& iter : hist)
//	{
//		auto key = iter.first;
//		if (!f)
//		{
//			prev = key;
//			f = true;
//			continue;
//		}
//		hist[key] += hist[prev];
//		offs[key] = hist[prev];
//		prev = key;
//	}
//	hist.clear();

//	size_t total = workingImg->length();

//	point* data = new point[total];//256
//	for (int i = 0; i < workingImg->wid(); ++i)//wid
//	{
//		for (int j = 0; j < workingImg->hei(); ++j)//hei
//		{
//            Barscalar p = workingImg->get(i, j);
//			data[offs[p]++] = point(i, j);
//		}
//	}
//	return data;
//}



void BarcodeCreator::init(const bc::DatagridProvider* src, ProcType& type, ComponentType& comp)
{
	wid = src->wid();
	hei = src->hei();

	needDelImg = false;

	if (type == ProcType::invertf0)
	{
		Barscalar mmin = 0, mmax = 0;
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


	//от 255 до 0
	if (comp == ComponentType::RadiusComp)
	{
		geometrySortedArr.reset(sortPixelsByRadius(workingImg, totalSize, type, settings.maxRadius, this->processCount));
		sortedArr = nullptr;
	}
	else
	{
		totalSize = workingImg->length();
		sortPixels(type);
	}
	// lastB = 0;

	included = new Include[totalSize];
	memset(included, 0, totalSize * sizeof(Include));

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
	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		curpix = getPoint(sortedArr[curIndexInSortedArr]);
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
	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		curpoindex = sortedArr[curIndexInSortedArr];
		curpix = getPoint(curpoindex);
		assert(curpoindex == wid * curpix.y + curpix.x);

		curbright = workingImg->get(curpix.x, curpix.y);

#ifdef VDEBUG
		VISUAL_DEBUG_COMP();
#else
		checkCloserB0();

#endif
		assert(included[wid * curpix.y + curpix.x]);

	}

	//assert(((void)"ALARM! B0 is not one", lastB == 1));

	addItemToCont(item);
	clearIncluded();
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
		Baritem* lines = new Baritem(workingImg->wid(), type);

		switch (settings.returnType)
		{
		case ReturnType::barcode2d:
			computeNdBarcode(lines, 2);
			break;
		case ReturnType::barcode3d:
		case ReturnType::barcode3dold:
			computeNdBarcode(lines, 3);
			break;
		default:
			break;
		}
		container->addItem(lines);
	}
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
	checkCloserB0();
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
	if (sortedArr != nullptr)
	{
		delete[] sortedArr;
		sortedArr = nullptr;
	}
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
	auto* rootNode = new BarRoot(0, 0, workingImg->wid(), nullptr, 0);

	for (COMPP c : components)
	{
		if (c == nullptr || c->resline == nullptr)
			continue;

		if (c->parent == nullptr)
		{
			//assert(c->isAlive() || settings.killOnMaxLen);
			c->kill();
			if (settings.createGraph)
				c->resline->setparent(rootNode);
		}

		assert(!c->isAlive());

		Barscalar len = c->resline->len();
		//else
		//{
		//	if (len == 0)
		//	{
		//		assert(c->resline->children.size() == 0);

		//		for (size_t i = 0; i < c->resline->matr.size(); i++)
		//		{
		//			c->parent->resline->matr.push_back(c->resline->matr[i]);
		//		}
		//		continue;
		//	}
		//}
		//else
			//assert(len != 0);



		if (static_cast<int>(len) == INFINITY)
			continue;

		lines->add(c->resline);
	}

	if (settings.createGraph)
	{
		lines->setRootNode(rootNode);
	}
	else
	{
		delete rootNode;
	}
}


void BarcodeCreator::processTypeF(barstruct& str, const bc::DatagridProvider* src, Barcontainer* item)
{
	init(src, str.proctype, str.comtype);

	switch (str.comtype)
	{
	case  ComponentType::Component:
		processComp(item);
		break;
	case  ComponentType::RadiusComp:
		processCompByRadius(item);
		break;
	case  ComponentType::Hole:
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


void BarcodeCreator::processFULL(barstruct& str, const bc::DatagridProvider* img, Barcontainer* item)
{
	bool rgb = (img->channels() != 1);


	if (str.comtype == ComponentType::Component && rgb)
	{
		BarImg* res = new BarImg();
		cvtColorV3B2U1C(*img, *res);
		originalImg = false;
		needDelImg = true;
		type = BarType::BYTE8_1;
		processTypeF(str, res, item);
		return;
	}

	switch (str.coltype)
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
		processTypeF(str, res, item);
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
		processTypeF(str, res, item);
		return;
	}
	default:
		break;
	}

	type = img->type;
	originalImg = true;
	needDelImg = false;
	processTypeF(str, img, item);
}


bc::Barcontainer* BarcodeCreator::createBarcode(const bc::DatagridProvider* img, const BarConstructor& structure)
{
	this->settings = structure;
	this->settings.createBinaryMasks = true;

	settings.checkCorrect();
	Barcontainer* cont = new Barcontainer();

	for (auto& it : settings.structure)
	{
		processFULL(it, img, cont);
	}
	return cont;
}

// ***************************************************


uchar dif(uchar a, uchar b)
{
	if (a > b)
		return a - b;
	else
		return b - a;
}

//
//void BarcodeCreator::Prepair()
//{
//	const bcBarImg& mat = *workingImg;
//	int N = 4;
//	//	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
//	static char poss[5][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, 0} }; //эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
//	for (int c = 0; c < mat.wid(); ++c)//x //wid
//	{
//		for (int r = 0; r < mat.hei(); ++r)//y //hei
//		{
//			this->curbright = mat.get(r, c);
//			this->curpix = point(c, r);
//			COMPP mainC = getComp(c, r);
//			for (int off = 0; off < N; ++off)
//			{
//				point p(c + poss[off][0], r + poss[off][1]);
//
//				if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
//					continue;
//
//				COMPP subC = getComp(p);
//				if (subC == mainC && mainC != nullptr)
//					continue;
//
//				Barscalar val = mat.get(p);
//
//				if (dif(val, curbright) <= settings.getStep())
//				{
//					if (subC == nullptr && mainC == nullptr)
//					{
//						mainC = new Component(this->curpix, this);
//						mainC->add(p);
//					}
//					else if (mainC != nullptr && subC == nullptr)
//					{
//						mainC->add(p);
//					}
//					else if (mainC == nullptr && subC != nullptr)
//					{
//						mainC = subC;
//						mainC->add(curpix);
//					}
//					else //if (mainC != nullptr && subC != nullptr)
//					{
//						mainC = attach(mainC, subC);
//					}
//				}
//				if (mainC == nullptr)
//				{
//					mainC = new Component(this->curpix, this);
//				}
//			}
//		}
//	}
//}
//
//
//void BarcodeCreator::ProcessFullPrepair(int* retBty, Barcontainer* item)
//{
//	for (int i = 0; i < 256; ++i)
//	{
//		settings.setStep(i);
//		Prepair();
//		retBty[i] = this->lastB;
//	}
//
//	for (auto* c : components)
//	{
//		if (c->isAlive())
//		{
//			c->kill();
//			break;
//		}
//	}
//	addItemToCont(item);
//	clearIncluded();
//	lastB = 0;
//}
//
//
//void BarcodeCreator::ProcessPrepComp(int* retBty, Barcontainer* item)
//{
//	Prepair();
//	processComp(retBty, item);
//}

//
//Barcontainer<float>* BarcodeCreator<float>::searchHoles(float* img, int wid, int hei, float nullVal)
//{
//	settings.createBinaryMasks = true;
//	settings.createGraph = true;
//	settings.returnType = ReturnType::barcode2d;
//	skipAddPointsToParent = true;
//	//[y * _wid + x]
//	img[hei * wid - 1] = 9999.f;//
//	workingImg = new BarImg<float>(wid, hei, 1, reinterpret_cast<uchar*>(img), false, false);
//	this->needDelImg = true;
//
//	if (nullVal > -999)
//	{
//		for (size_t i = 0; i < workingImg->length(); ++i)
//		{
//			workingImg->getLiner(i) = -9999;
//		}
//		nullVal = -9999;
//	}
//	//	float maxs, mins;
//	//	workingImg->maxAndMin(mins, maxs);
//	//	settings.setMaxLen((maxs - mins) / 2);
//	auto prec = ProcType::f255t0;
//	auto comp = ComponentType::Component;
//	init(workingImg, prec, comp);
//
//	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
//	{
//		curpoindex = sortedArr[curIndexInSortedArr];
//		curpix = getPoint(curpoindex);
//		curbright = workingImg->get(curpix);
//
//#ifdef VDEBUG
//		VISUAL_DEBUG_COMP();
//#else
//		checkCloserB0();
//#endif
//	}
//
//
//	Barcontainer<float>* item = new Barcontainer<float>();
//
//	addItemToCont(item);
//	clearIncluded();
//
//	return item;
//}



Barcontainer* BarcodeCreator::searchHoles(float* /*img*/, int /*wid*/, int /*hei*/, float/* nullVal*/)
{
	return nullptr;
}

#ifdef _PYD

bc::Barcontainer* bc::BarcodeCreator::createBarcode(bn::ndarray& img, bc::BarConstructor& structure)
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
	for (curIndexInSortedArr = 0; curIndexInSortedArr < processCount; ++curIndexInSortedArr)
	{
		const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];

		//if (val.poz == nextPoz::middleRight)
		//	continue;
		curpoindex = val.offset;
		curpix = getPoint(curpoindex);
		curbright = workingImg->get(curpix.x, curpix.y);


		bc::point NextPoint = val.getNextPoint(curpix);
		poidex NextPindex = NextPoint.getLiner(workingImg->wid());

		Component* first = getComp(curpoindex);
		Component* connected = getComp(NextPindex);

		/*if (val.dist > settings.maxRadius)
		{
			break;
			new Component(curpoindex, this);
			new Component(NextPindex, this);
		}*/

		if (first != nullptr)
		{
			//если в найденном уже есть этот элемент
			//существует ли ребро вокруг
			if (connected != nullptr && first != connected)
			{
				attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
			}
			else if (connected == nullptr)
			{
				if (!first->add(NextPindex, NextPoint))
				{
					connected = new Component(NextPindex, this);
				}
			}
		}
		else
		{
			// Ребро не создано или не получилось присоединить
			if (connected == nullptr)
			{
				first = new Component(curpoindex, this);
				if (!first->add(NextPindex, NextPoint))
				{
					connected = new Component(NextPindex, this);
				}
			}
			else if (!connected->add(curpoindex, curpix))
			{
				first = new Component(curpoindex, this);
			}
		}

#ifdef USE_OPENCV
		if (settings.visualize)
		{
			const BarMat* mat = reinterpret_cast<const BarMat*>(this->workingImg);

			if (mat)
			{
				Mat wimg;
				bool is3d = this->type == BarType::BYTE8_3;
				if (!is3d)
					cv::cvtColor(mat->mat, wimg, cv::COLOR_GRAY2BGR);
				else
					mat->mat.copyTo(wimg);

				int size = colors.size();

				for (size_t i = 0; i < totalSize; i++)
				{
					COMPP comp = getInclude(i);
					if (comp == nullptr || !comp->isAlive())
						continue;
					int x = static_cast<int>(i % wid);
					int y = static_cast<int>(i / wid);
					int tic = 1;
					int marc = cv::MARKER_TILTED_CROSS;

					cv::Point p(x, y);
					cv::Vec3b col = colors[(size_t)comp->startIndex % size];

					wimg.at<cv::Vec3b>(y, x) = col;

				}
				//wimg.at<cv::Vec3b>(curpix.y, curpix.x) = cv::Vec3b(255, 0, 0);
				//wimg.at<cv::Vec3b>(NextPoint.y, NextPoint.x) = cv::Vec3b(0,0,255);

				cv::namedWindow("img", cv::WINDOW_NORMAL);
				cv::imshow("img", wimg);
			}
			else
			{
				draw("radius");
			}
			cv::waitKey(settings.waitK);
		}
#endif // USE_OPENCV
	}


	//totalSize = workingImg->length();
	//bc::ProcType type = ProcType::f0t255;
	//sortPixels(type);
	//processComp(item);

	//assert(((void)"ALARM! B0 is not one", lastB == 1));
	addItemToCont(item);
	clearIncluded();
}


//template bc::Barcontainer<ushort>* BarcodeCreator<ushort>::createBarcode(const bc::DatagridProvider<ushort>*, const BarConstructor<ushort>&)

