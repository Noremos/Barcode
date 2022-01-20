#include "barcodeCreator.h"
#include <stack>
#include <string>
#include <assert.h>
#include <algorithm>

using namespace bc;

#pragma warning(disable : 4996)

template<class T>
void BarcodeCreator<T>::draw(std::string name)
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
			Hole<T>* hd = dynamic_cast<Hole<T>*>(comp);
			if (hd && hd->getIsOutside())
				col = cv::Vec3b(0, 0, 0);
			else
				col = colors[(size_t)comp % size];

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

template<class T>
inline COMPP BarcodeCreator<T>::attach(COMPP main, COMPP second)
{
	//second->kill();
	if (second->getStart() == main->getStart() && main->getStart() == curbright)
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
		if (sortedArr != nullptr)
		{
			for (size_t rind = second->startIndex; rind <= curIndexInSortedArr; ++rind)
			{
				poidex& p = sortedArr[rind];
				//assert(workingImg->get(p.x, p.y) == curbright);
				if (getInclude(p) == second)
				{
					// Перебираем предыдущие элементы
					main->add(p, getPoint(p));
				}
			}
		}
		else
		{
			for (size_t rind = second->startIndex; rind <= curIndexInSortedArr; ++rind)
			{
				auto val = geometrySortedArr.get()[rind];
				poidex& p = val.offset;
				auto curpoint = getPoint(p);

				if (getInclude(p) == second)
				{
					// Перебираем предыдущие элементы
					main->add(p, curpoint);
				}

				// NEXT
				bc::point NextPoint = val.getNextPoint(curpoint);
				poidex NextPindex = NextPoint.getLiner(workingImg->wid());

				if (getInclude(NextPindex) == second)
				{
					// Перебираем предыдущие элементы
					main->add(NextPindex, NextPoint);
				}
			}
		}
		main->startIndex = MIN(second->startIndex, main->startIndex);
#endif // POINTS_ARE_AVAILABLE
		delete second->resline;
		second->resline = nullptr;

		return main;
	}

	switch (settings.attachMode)
	{
	case AttachMode::dontTouch:
		return main;

	case AttachMode::secondEatFirst:
		if (main->getStart() < second->getStart())
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
		COMPP newOne = new Component<T>(this, true);
		main->setParent(newOne);
		second->setParent(newOne);
		//main->kill();
		//second->kill();
		return newOne;
	}
	// else pass down

	case AttachMode::firstEatSecond:
	default:
		if (main->getStart() > second->getStart())
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		second->setParent(main);
		break;
	}
	//возращаем единую компоненту.
	return main;
}

#include <iostream>
//****************************************B0**************************************
template<class T>
inline bool BarcodeCreator<T>::checkCloserB0()
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
				if (curbright - first->getStart() > settings.maxLen.getOrDefault(0))
				{
					//qDebug() << first->num << " " << curbright << " " << settings.maxLen.getOrDefault(0);
					if (settings.killOnMaxLen)
						if (settings.killOnMaxLen)
						{
							first->kill(); //Интересный результат
						}
					first = nullptr;
				}
				else
					first->add(curpoindex);
				//setInclude(midP, first);//n--nt обяз нужно
			}
			else
			{
				if (first->isContain(IcurPindex))//если в найденном уже есть этот элемент
					continue;

				//lastB -= 1;
				if (first != connected)
					first = attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
			}
		}
	}
	if (first == nullptr)
	{
		//lastB += 1;

		first = new Component<T>(curpoindex, this);
		return true;
	}
	return false;
}
//********************************************************************************



template<class T>
COMPP BarcodeCreator<T>::getPorogComp(const point& p, poidex index)
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

template<class T>
COMPP BarcodeCreator<T>::getInclude(const size_t pos)
{
	assert(pos < totalSize);
	return included[pos] ? included[pos]->getMaxparent() : nullptr;
}


template<class T>
HOLEP BarcodeCreator<T>::getHole(uint x, uint y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

	auto itr = included[wid * y + x];
	return dynamic_cast<HOLEP>(itr ? itr->getMaxparent() : nullptr);
}

template<class T>
HOLEP BarcodeCreator<T>::getHole(const point& p)
{
	auto itr = included[wid * p.y + p.x];
	return dynamic_cast<HOLEP>(itr ? itr->getMaxparent() : nullptr);
}


template<class T>
HOLEP BarcodeCreator<T>::tryAttach(HOLEP main, HOLEP add, point p)
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

template<class T>
inline bool BarcodeCreator<T>::checkCloserB1()
{
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
	Hole<T>* hr = nullptr;
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

			Hole<T>* h1 = getHole(p1);
			Hole<T>* h2 = getHole(p2);
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
				hr = new Hole<T>(curpix, p1, p2, this);

				delete h2;
				h1->tryAdd(curpix);
				hr = tryAttach(hr, h1, curpix);
			}
			//вариант 3 - h1 - не  валид, h2- валид.Мы уже проверили, что треугольник p-p1-p2 есть
			//cod 402
			else if (h2->isValid && !h1->isValid)
			{
				hr = new Hole<T>(curpix, p1, p2, this);
				delete h1;
				h2->tryAdd(curpix);
				hr = tryAttach(hr, h2, curpix);
			}
			//вариант 4 - оба не валид
			else if (!h1->isValid && !h2->isValid)//не факт, что они не валидны
			{
				//Т.К. мы уже проверили вышле, что образуется треуготльник, можно смело создаать дыру
				hr = new Hole<T>(curpix, p1, p2, this);
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
		hr = new Hole<T>(curpix, this);
		return false;
	}

	for (int i = 0; i < 16; ++i)
	{
		point curp = curpix + poss[i % 8];

		if (isContain(curp))
		{
			Hole<T>* h_t = getHole(curp);
			//получена дыра
			if (h_t == hr)
				continue;

			Hole<T>* h2 = nullptr;

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
					new Hole<T>(curpix, curp, next, this);
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


template<class T>
bc::indexCov* sortPixelsByRadius(const bc::DatagridProvider<T>* workingImg, size_t& totalSize, bc::ProcType type)
{
	int wid = workingImg->wid();
	int lastW = wid - 1;
	int lastH = workingImg->hei() - 1;

	totalSize = lastW * (lastH - 1) * 4 + lastW * 3 + lastW * 2 + lastH;

	bc::indexCov* data = new bc::indexCov[totalSize];//256
	// Сичтаем расстояние между всеми соседними пикселями для каждого пикселя.
	// Чтобы не считать повтороно, от текущего проверяем только уникальные - в форме перевёрнутой "г"
	//
	int k = 0;
	for (int h = 1; h < lastH; ++h)
	{
		for (int w = 0; w < lastW; ++w)
		{
			int offset = wid * h + w;
			T cur = workingImg->get(w, h);
			T next;

			next = workingImg->get(w + 1, h - 1);
			data[k++] = indexCov(offset, indexCov::val_distance(cur, next), topRight);
			next = workingImg->get(w + 1, h);
			data[k++] = indexCov(offset, indexCov::val_distance(cur, next), middleRight);
			next = workingImg->get(w + 1, h + 1);
			data[k++] = indexCov(offset, indexCov::val_distance(cur, next), downRight);
			next = workingImg->get(w, h + 1);
			data[k++] = indexCov(offset, indexCov::val_distance(cur, next), downCur);
		}
	}


	//first line
	for (int w = 0; w < lastW; ++w)
	{
		int offset = w;
		T cur = workingImg->get(w, 0);
		T next;

		next = workingImg->get(w + 1, 0);
		data[k++] = indexCov(offset, indexCov::val_distance(cur, next), middleRight);
		next = workingImg->get(w + 1, 1);
		data[k++] = indexCov(offset, indexCov::val_distance(cur, next), downRight);
		next = workingImg->get(w, 1);
		data[k++] = indexCov(offset, indexCov::val_distance(cur, next), downCur);
	}

	//last line (row)
	for (int w = 0; w < lastW; ++w)
	{
		int offset = wid * lastH + w;
		T cur = workingImg->get(w, lastH);
		T next;

		next = workingImg->get(w + 1, lastH - 1);
		data[k++] = indexCov(offset, indexCov::val_distance(cur, next), topRight);
		next = workingImg->get(w + 1, lastH);
		data[k++] = indexCov(offset, indexCov::val_distance(cur, next), middleRight);
	}

	//last column
	for (int h = 0; h < lastH; ++h)
	{
		int offset = wid * h + lastW;
		T cur = workingImg->get(lastW, h);
		T nextH = workingImg->get(lastW, h + 1);
		data[k++] = indexCov(offset, indexCov::val_distance(cur, nextH), nextPoz::downCur);
	}

	// Тип не имеет занчение потому что соединяем не по яркости
	//if (type == ProcType::f0t255)
	//{
	std::sort(data, data + totalSize, [](const indexCov& a, const indexCov& b) {
		return a.dist < b.dist;
		});
	//}
	//else // type == ProcType::f255t0
	//{
	//	std::sort(data, data + totalSize, [](const indexCov& a, const indexCov& b) {
	//		return a.dist > b.dist;
	//		});
	//}
	return data;
}


template<>
inline void BarcodeCreator<uchar>::sortPixels(bc::ProcType type)
{
	uint hist[256];//256
	uint offs[256];//256
	memset(hist, 0, 256 * sizeof(int));
	memset(offs, 0, 256 * sizeof(int));

	for (int j = 0; j < workingImg->hei(); ++j)//hei
	{
		for (int i = 0; i < workingImg->wid(); ++i)//wid
		{
			auto p = workingImg->get(i, j);
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
		uchar p = workingImg->getLiner(i);
		data[offs[p]++] = i;
	}

	if (type == ProcType::f255t0)
	{
		std::reverse(data, data + totalSize);
	}
	this->sortedArr = data;
}



template<>
void BarcodeCreator<bc::barVector<uchar, 3>>::sortPixels(bc::ProcType type)
{
	this->geometrySortedArr.reset(sortPixelsByRadius(workingImg, totalSize, type));
}

template<class T>
struct myclassFromMin {
	const bc::DatagridProvider<T>* workingImg;
	bool operator() (poidex& a, poidex& b)
	{
		return workingImg->getLiner(a) < workingImg->getLiner(b);
	}
};

template<class T>
struct myclassFromMax {
	const bc::DatagridProvider<T>* workingImg;
	bool operator() (poidex& a, poidex& b)
	{
		return workingImg->getLiner(a) > workingImg->getLiner(b);
	}
};

template<class T>
void BarcodeCreator<T>::sortPixels(bc::ProcType type)
{
	// do this hack to skip constructor calling for every point
	poidex* data = new poidex[totalSize + 1];//256

	//point * data = new point[total];

	for (size_t i = 0; i < totalSize; ++i)//wid
		data[i] = i;

	myclassFromMin<T> cmp;
	cmp.workingImg = workingImg;
	std::sort(data, &data[totalSize], cmp);
	if (type == ProcType::f255t0)
	{
		for (size_t i = 1; i < totalSize - 1; ++i)//wid
		{
			float v0 = static_cast<float>(workingImg->getLiner(data[i - 1]));
			float v2 = static_cast<float>(workingImg->getLiner(data[i]));
			assert(v0 >= v2);
		}
	}

	this->sortedArr = data;
}
#include<map>

//template<class T>
//inline point* BarcodeCreator<T>::sort()
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

//	T prev;
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
//            T p = workingImg->get(i, j);
//			data[offs[p]++] = point(i, j);
//		}
//	}
//	return data;
//}


template<class T>
void BarcodeCreator<T>::init(const bc::DatagridProvider<T>* src, ProcType& type, ComponentType& comp)
{
	wid = src->wid();
	hei = src->hei();

	needDelImg = false;

	if (type == ProcType::invertf0)
	{
		T mmin = 0, mmax = 0;
		src->maxAndMin(mmin, mmax);
		bc::BarImg<T>* newone = new BarImg<T>(src->wid(), src->hei());
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
		geometrySortedArr.reset(sortPixelsByRadius(workingImg, totalSize, type));
	}
	else
	{
		totalSize = workingImg->length();
		sortPixels(type);
	}
	// lastB = 0;

	included = new Include<T>[totalSize];
	memset(included, 0, totalSize * sizeof(Include<T>));

#ifdef USE_OPENCV

	if (colors.size() == 0 && settings.visualize)
	{
		for (int b = 0; b < 255; b += 20)
			for (int g = 255; g > 20; g -= 20)
				for (int r = 0; r < 255; r += 50)
					colors.push_back(cv::Vec3b(b, g, r));
}
#endif // USE_OPENCV
}
//#include <QDebug>

template<class T>
void BarcodeCreator<T>::processHole(Barcontainer<T>* item)
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

template<class T>
void BarcodeCreator<T>::processComp(Barcontainer<T>* item)
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

template<class T>
void BarcodeCreator<T>::addItemToCont(Barcontainer<T>* container)
{
	if (container != nullptr)
	{
		Baritem<T>* lines = new Baritem<T>(workingImg->wid());

		switch (settings.returnType)
		{
		case ReturnType::barcode2d:
			computeNdBarcode(lines, 2);
			break;
		case ReturnType::barcode3d:
		case ReturnType::barcode3dold:
			computeNdBarcode(lines, 3);
			break;


			break;
		default:
			break;
		}
		container->addItem(lines);
	}
}

template<class T>
void BarcodeCreator<T>::VISUAL_DEBUG()
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

template<class T>
void BarcodeCreator<T>::VISUAL_DEBUG_COMP()
{
	checkCloserB0();
#ifdef USE_OPENCV
	if (settings.visualize)
	{
		draw("main");
		cv::waitKey(1);
	}
#endif // DEBUG
}

template<class T>
void BarcodeCreator<T>::clearIncluded()
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
		//memset(included, 0, totalSize * sizeof(Include<T>));
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


template<class T>
bool compareLines(const barline<T>* i1, const barline<T>* i2)
{
	if (i1->len == i2->len)
		return i1->start > i2->start;

	return (i1->len > i2->len);
}


template<class T>
void BarcodeCreator<T>::computeNdBarcode(Baritem<T>* lines, int n)
{
	assert(n == 2 || n == 3);

	// якорная линия
	auto* rootNode = new BarRoot<T>(0, 0, workingImg->wid(), nullptr, 0);

	for (COMPP c : components)
	{
		if (c == nullptr || c->resline == nullptr)
			continue;

		T len = c->resline->len();

		if (c->parent == nullptr)
		{
			assert(c->isAlive() || settings.killOnMaxLen);
			c->kill();
			if (settings.createGraph)
				c->resline->setparent(rootNode);
		}
		//else
			//assert(len != 0);

		assert(!c->isAlive());


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

template<class T>
void BarcodeCreator<T>::processTypeF(barstruct& str, const bc::DatagridProvider<T>* src, Barcontainer<T>* item)
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

template<class T>
void BarcodeCreator<T>::processFULL(barstruct& str, const bc::DatagridProvider<T>* img, Barcontainer<T>* item)
{
	bool rgb = (img->channels() != 1);
	originalImg = true;
	if (str.coltype == ColorType::rgb || (str.coltype == ColorType::native && rgb))
	{
		assert(img->channels() == 3);
		processTypeF(str, img, item);
	}
	else
	{
		if (img->channels() != 1)
		{
			bc::BarImg<T> res(1, 1);
			cvtColorV3B2U1C(*img, res);
			originalImg = false;
			//const DatagridProvider<T>* temoing = dynamic_cast<const DatagridProvider<T>*>(res);
			processTypeF(str, &res, item);
		}
		else
		{
			processTypeF(str, img, item);
		}
	}
}

template<class T>
bc::Barcontainer<T>* BarcodeCreator<T>::createBarcode(const bc::DatagridProvider<T>* img, const BarConstructor<T>& structure)
{
	this->settings = structure;
	settings.checkCorrect();

	Barcontainer<T>* cont = new Barcontainer<T>();

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

//template<class T>
//void BarcodeCreator<T>::Prepair()
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
//				T val = mat.get(p);
//
//				if (dif(val, curbright) <= settings.getStep())
//				{
//					if (subC == nullptr && mainC == nullptr)
//					{
//						mainC = new Component<T>(this->curpix, this);
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
//					mainC = new Component<T>(this->curpix, this);
//				}
//			}
//		}
//	}
//}
//
//template<class T>
//void BarcodeCreator<T>::ProcessFullPrepair(int* retBty, Barcontainer<T>* item)
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
//template<class T>
//void BarcodeCreator<T>::ProcessPrepComp(int* retBty, Barcontainer<T>* item)
//{
//	Prepair();
//	processComp(retBty, item);
//}

template<>
Barcontainer<float>* BarcodeCreator<float>::searchHoles(float* img, int wid, int hei, float nullVal)
{
	settings.createBinaryMasks = true;
	settings.createGraph = true;
	settings.returnType = ReturnType::barcode2d;
	skipAddPointsToParent = true;
	//[y * _wid + x]
	img[hei * wid - 1] = 9999.f;//
	workingImg = new BarImg<float>(wid, hei, 1, reinterpret_cast<uchar*>(img), false, false);
	this->needDelImg = true;

	if (nullVal > -999)
	{
		for (size_t i = 0; i < workingImg->length(); ++i)
		{
			workingImg->getLiner(i) = -9999;
		}
		nullVal = -9999;
}
	//	float maxs, mins;
	//	workingImg->maxAndMin(mins, maxs);
	//	settings.setMaxLen((maxs - mins) / 2);
	auto prec = ProcType::f255t0;
	auto comp = ComponentType::Component;
	init(workingImg, prec, comp);

	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		curpoindex = sortedArr[curIndexInSortedArr];
		curpix = getPoint(curpoindex);
		curbright = workingImg->get(curpix);

#ifdef VDEBUG
		VISUAL_DEBUG_COMP();
#else
		checkCloserB0();
#endif
	}


	Barcontainer<float>* item = new Barcontainer<float>();

	addItemToCont(item);
	clearIncluded();

	return item;
}


template<class T>
Barcontainer<T>* BarcodeCreator<T>::searchHoles(float* /*img*/, int /*wid*/, int /*hei*/, float/* nullVal*/)
{
	return nullptr;
}

#ifdef _PYD

template<>
inline bc::Barcontainer<barvec3b>* bc::BarcodeCreator<barvec3b>::createBarcode(bn::ndarray& img, bc::BarConstructor<barvec3b>& structure)
{
	//auto shape = img.get_shape();

	/*	int type = cv_8uc1;
	if (img.get_nd() == 3 && img.shape[2] == 3)
	image = &barimg<bcvec3b>(shape[0], shape[1], img.get_data());
	else if float
	barimg image(shape[0], shape[1], img.get_data());*/

	//cv::imshow("test", image);
	//cv::waitkey(0);
	bc::BarNdarray<barvec3b> image(img);
	//try
	//{
	return createBarcode(&image, structure);
	//}
	//catch (const std::exception& ex)
	//{
	//printf("ERROR");
	//printf(ex.what());
	//}

	//bc::BarImg<T> image(img.shape(1), img.shape(0), img.get_nd(), (uchar*)img.get_data(), false, false);
	//return createBarcode(&image, structure);
}

template<class T>
inline bc::Barcontainer<T>* bc::BarcodeCreator<T>::createBarcode(bn::ndarray& img, bc::BarConstructor<T>& structure)
{
	//auto shape = img.get_shape();

	/*	int type = cv_8uc1;
	if (img.get_nd() == 3 && img.shape[2] == 3)
	image = &barimg<bcvec3b>(shape[0], shape[1], img.get_data());
	else if float
	barimg image(shape[0], shape[1], img.get_data());*/

	//cv::imshow("test", image);
	//cv::waitkey(0);
	bc::BarNdarray<T> image(img);
	//try
	//{
	return createBarcode(&image, structure);
	//}
	//catch (const std::exception& ex)
	//{
	//printf("ERROR");
	//printf(ex.what());
	//}

	//bc::BarImg<T> image(img.shape(1), img.shape(0), img.get_nd(), (uchar*)img.get_data(), false, false);
	//return createBarcode(&image, structure);
	}
#endif
// GEOMERTY


template<class T>
void BarcodeCreator<T>::processCompByRadius(Barcontainer<T>* item)
{
	for (curIndexInSortedArr = 0; curIndexInSortedArr < totalSize; ++curIndexInSortedArr)
	{
		const indexCov& val = geometrySortedArr.get()[curIndexInSortedArr];
		curpoindex = val.offset;
		curpix = getPoint(curpoindex);
		curbright = workingImg->get(curpix.x, curpix.y);

		bc::point NextPoint = val.getNextPoint(curpix);
		poidex NextPindex = NextPoint.getLiner(workingImg->wid());

		auto first = getComp(curpoindex);
		auto connected = getComp(NextPindex);
		if (first != nullptr)
		{
			if (connected != nullptr)//существует ли ребро вокруг
			{
				if (first != connected)//если в найденном уже есть этот элемент
					attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
			}
			else
			{
				first->add(NextPindex, NextPoint);
			}
		}
		else
		{
			if (connected != nullptr)//существует ли ребро вокруг
			{
				connected->add(curpoindex);
			}
			else
			{
				first = new Component<T>(curpoindex, this);
				first->add(NextPindex, NextPoint);
			}
		}
#ifdef USE_OPENCV
		if (settings.visualize)
		{
			const BarMat<T>* mat = reinterpret_cast<const BarMat<T>*>(this->workingImg);

			if (mat)
			{
				Mat wimg;
				bool is3d = typeid(T) != typeid(uchar);
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
	//assert(((void)"ALARM! B0 is not one", lastB == 1));
	addItemToCont(item);
	clearIncluded();
}

INIT_TEMPLATE_TYPE(bc::BarcodeCreator)

//template bc::Barcontainer<ushort>* BarcodeCreator<ushort>::createBarcode(const bc::DatagridProvider<ushort>*, const BarConstructor<ushort>&)

