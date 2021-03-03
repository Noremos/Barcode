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
		COMPP comp = included[i].comp;
		if (comp == nullptr)
			continue;
		int x = i % wid;
		int y = i / wid;
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
			col = colors[comp->num % size];
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

//#define CREATE_NEW
//Образовала новый?

template<class T>
inline COMPP BarcodeCreator<T>::attach(COMPP main, COMPP second)
{
	if (settings.createNewComponentOnAttach)
	{
		COMPP newOne = new Component<T>(this, true);
		main->setParrent(newOne);
		second->setParrent(newOne);
		main->kill();
		second->kill();
		return newOne;
	}
	else
	{
		// ***************************************************
		//if (main->coords->size() < second->coords->size()) //свапаем, если у первого меньше элементов. Нужно для производиельности
		if (main->start > second->start)
		{
			COMPP temp = main;
			main = second;
			second = temp;
		}
		//	if (second->coords->size()<100)
		{
			second->setParrent(main);
			second->kill();
			//возращаем единую компоненту.
			return main;
		}
	}
}

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
		point curPoint(curpix + poss[i]);
		connected = getPorogComp(curPoint);
		if (connected != nullptr)//существует ли ребро вокруг
		{
			if (first == nullptr)
			{
				first = connected;
				first->add(curpix);
				//setInclude(midP, first);//n--nt обяз нужно
			}
			else// соединяет несколько разных компоненты
			{
				if (first->isContain(curPoint))//если в найденном уже есть этот элемент
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

		first = new Component<T>(curpix, this);
		return true;
	}

	return false;
}
//********************************************************************************


template<class T>
int BarcodeCreator<T>::GETPOFF(const point& p) const {
	return wid * p.y + p.x;
}


template<class T>
bool BarcodeCreator<T>::isContain(int x, int y) const
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return false;

	return included[wid * y + x].comp != nullptr;
}

template<class T>
bool BarcodeCreator<T>::isContain(const point& p, bool valid) const
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return false;

	return (included[wid * p.y + p.x].comp != nullptr && ((HOLEP)included[wid * p.y + p.x].comp)->isValid == valid);
}

template<class T>
bool BarcodeCreator<T>::isContain(const point& p) const
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return false;

	return included[wid * p.y + p.x].comp != nullptr;
}

template<class T>
COMPP BarcodeCreator<T>::getComp(int x, int y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

	auto itr = included[wid * y + x].comp;
	return itr ? itr->getMaxParrent() : nullptr;
}

template<class T>
COMPP BarcodeCreator<T>::getComp(const point& p)
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return nullptr;

	auto itr = included[wid * p.y + p.x].comp;
	return itr ? itr->getMaxParrent() : nullptr;
}


template<class T>
COMPP BarcodeCreator<T>::getPorogComp(const point& p)
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return nullptr;

	const size_t off = static_cast<size_t>(wid) * p.y + p.x;
	auto& itr = included[off];
	if (itr.comp && GETDIFF(curbright, itr.bright) <= settings.getMaxStepPorog())
	{
		return itr.comp->getMaxParrent();
	}
	else
		return nullptr;
}

template<class T>
bc::Include<T>* BarcodeCreator<T>::getInclude(const size_t pos)
{
	assert(pos < totalSize);
	return &included[pos];
}


template<class T>
HOLEP BarcodeCreator<T>::getHole(int x, int y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

	auto itr = included[wid * y + x].comp;
	return dynamic_cast<HOLEP>(itr ? itr->getMaxParrent() : nullptr);
}

template<class T>
HOLEP BarcodeCreator<T>::getHole(const point& p)
{
	auto itr = included[wid * p.y + p.x].comp;
	return dynamic_cast<HOLEP>(itr ? itr->getMaxParrent() : nullptr);
}

template<class T>
void BarcodeCreator<T>::setInclude(int x, int y, COMPP comp, T bright)
{
	included[wid * y + x].setValues(comp, bright);
}

template<class T>
void BarcodeCreator<T>::setInclude(const point& p, COMPP comp, T bright)
{
	included[wid * p.y + p.x].setValues(comp, bright);
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
		if (isContain(p1))
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
				//                if (!hr->getIsOutside())   ++lastB;
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
			//получена дыра
			Hole<T>* h_t = getHole(curp);
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
					Hole<T>* nh = new Hole<T>(curpix, curp, next, this);
					delete h_t;
					delete h2;
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

template<>
inline point* BarcodeCreator<uchar>::sort(const ProcType& type)
{
	int hist[256];//256
	int offs[256];//256
	memset(hist, 0, 256 * sizeof(int));
	memset(offs, 0, 256 * sizeof(int));

	for (int i = 0; i < workingImg->wid(); ++i)//wid
	{
		for (int j = 0; j < workingImg->hei(); ++j)//hei
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

	size_t total = workingImg->length();

	point* data = new point[total];//256
	for (int i = 0; i < workingImg->wid(); ++i)//wid
	{
		for (int j = 0; j < workingImg->hei(); ++j)//hei
		{
			auto p = (int)workingImg->get(i, j);
			data[offs[p]++] = point(i, j);
		}
	}

	if (type == ProcType::f255t0)
	{
		for (size_t i = 0, ts = total / 2; i < ts; ++i)
		{
			point temp = data[i];
			data[i] = data[total - i - 1];
			data[total - i - 1] = temp;
		}
	}
	return data;
}

template<class T>
inline point* BarcodeCreator<T>::sort(const ProcType& type)
{
	size_t total = workingImg->length();
	point* data = new point[total];//256

	size_t k = 0;
	for (int i = 0; i < total; ++i)//wid
		data[i] = workingImg->getPointAt(i);

	std::sort(data, data + total, [ptr = workingImg](point& a, point& b) {
		return ptr->get(a.x, a.y) > ptr->get(b.x, b.y);
		});

	if (type == ProcType::f255t0)
	{
		std::reverse(data, data + total);
	}

	return data;
}

template<class T>
void BarcodeCreator<T>::init(const bc::DatagridProvider<T>* src, const  ProcType& type)
{
	needDelImg = false;

	setWorkingImg(src);

	wid = src->wid();
	hei = src->hei();
	totalSize = src->length();
	included = new Include<T>[totalSize];
	memset(included, 0, totalSize * sizeof(Include<T>));


	//от 255 до 0
	sortedArr = sort(type);
	memset(b, 0, 256 * sizeof(T));

	lastB = 0;

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
void BarcodeCreator<T>::processHole(int* retBty, Barcontainer<T>* item)
{
	size_t len = totalSize - 1;
	reverse = false;

	for (size_t i = 0; i < totalSize; ++i)
	{
		curpix = sortedArr[i];
		curbright = workingImg->get(curpix);

		/*	if (i == 25)
				qDebug() << "";*/
#ifdef VDEBUG
		VISULA_DEBUG(totalSize, i);
#else
		checkCloserB1();
#endif

		if (i != len)
		{
			T scnd = workingImg->get(sortedArr[i + 1]);
			if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
			{
				for (T k = curbright; k < scnd; k += settings.getMaxStepPorog()) {
					retBty[(int)k] = lastB;
				}
			}
		}
		else
		{
			retBty[(int)curbright] = lastB;
		}
	}
	//if (lastB != 1)
	//	qDebug() << lastB;
	assert(((void)"ALARM! B1 is not zero", lastB == 1));

	addItemToCont(item);
	clearIncluded();
	lastB = 0;
}

template<class T>
void BarcodeCreator<T>::processComp(int* retBty, Barcontainer<T>* item)
{
	size_t len = totalSize - 1;

	for (size_t i = 0; i < totalSize; ++i)
	{
		curpix = sortedArr[i];
		curbright = workingImg->get(curpix.x, curpix.y);
#ifdef VDEBUG
		VISULA_DEBUG_COMP(totalSize, i);
#else
		checkCloserB0();
#endif
		if (settings.returnType == ReturnType::betty)
		{
			if (i != len)
			{
				T scnd = workingImg->get(sortedArr[i + 1]);
				if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
				{
					for (T k = curbright; k < scnd; k += settings.getMaxStepPorog())
						retBty[(int)k] = lastB;
				}
			}
			else {
				retBty[(int)curbright] = lastB;
			}
		}
	}

	//    curbright = 255;
	//assert(((void)"ALARM! B0 is not one", lastB == 1));

	addItemToCont(item);
	clearIncluded();
	lastB = 0;
}

// Parallel execution with function object.
struct Operator
{
	void operator()(short& pixel, const int* position) const
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
		container->addItem(getBarcode());
	}
}

template<class T>
void BarcodeCreator<T>::VISULA_DEBUG(int y, int i)
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
void BarcodeCreator<T>::VISULA_DEBUG_COMP(int y, int i)
{
	checkCloserB0();
#ifdef USE_OPENCV
	if (settings.visualize)
	{
		draw("main");
		cv::waitKey(0);
	}
#endif // DEBUG
}

template<class T>
void BarcodeCreator<T>::clearIncluded()
{
	for (COMPP c : components)
	{
		if (c != nullptr)
		{
			delete c;
		}
	}
	components.clear();

	if (included != nullptr)
	{
		memset(included, 0, totalSize * sizeof(Include<T>));
		delete[] included;
		included = nullptr;
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
void BarcodeCreator<T>::reverseCom()
{
	for (size_t i = 0; i < totalSize; i++)
	{
		Include<T>* incl = getInclude(i);
		barline<T>* bline;
		auto& ccod = incl->bright;// начало конкретно в этом пикселе

		if (settings.createBinayMasks)
		{
			// надо добавить заничя каому потомку
			COMPP prev = incl->comp;
			while (prev->parent)
			{
				barline<T>* blineParrent = prev->parent->resline;
				bline = prev->resline;

				if (settings.createBinayMasks)
					blineParrent->addCoord(getPoint(i), blineParrent->end() - bline->end());//нам нужно только то время, которое было у съевшего.

				prev = prev->parent;
			}
		}
		//blineParrent->end() - ccod = общее время
		//item->end() - ccod + blineParrent->end() - item->end() = общее время
		//220 - 10
		bline = incl->comp->resline;
		if (settings.createBinayMasks)
			bline->addCoord(getPoint(i), bline->end() - ccod);

		// parent always will be (rootNode for root elements
		if (settings.createGraph && incl->comp->parent != nullptr)
			bline->setParrent(incl->comp->parent->resline);
	}
}

template<class T>
void BarcodeCreator<T>::computeBettyBarcode(Baritem<T>* lines)
{
	throw std::exception();
}

template<>
void BarcodeCreator<uchar>::computeBettyBarcode(Baritem<uchar>* lines)
{
	std::stack<uchar> tempStack;

	for (short i = 0; i < 256; ++i)
	{
		int p = b[i];
		if (i > 0)
		{
			int pred = b[i - 1];
			if (pred == p)// || (tempStack!=0 && tempStack[tempStack.length-1][1]==p))
				continue;

			for (int j = pred + 1; j <= p; ++j)
				tempStack.push((uchar)i);

			for (int j = pred; j > p; --j)
			{
				uchar t = tempStack.top();
				lines->add(t, (uchar)i - t);
				tempStack.pop();
			}
		}
		else if (p > 0)
		{
			for (int j = 1; j <= p; j++)
				tempStack.push((uchar)i);
		}
	}

	while (!tempStack.empty())
	{
		uchar t = tempStack.top();
		lines->add((uchar)t, (uchar)MIN(255, 255 - t));
		tempStack.pop();
	}
	std::vector<bc::barline<uchar>*>& vec = lines->barlines;
	std::sort(vec.begin(), vec.end(), compareLines<uchar>);
}

template<class T>
void BarcodeCreator<T>::computeNdBarcode(Baritem<T>* lines, int n)
{
	assert(n == 2 || n == 3);

	// якорная линия
	auto* rootNode = new BarRoot<T>(0, 0);

	for (COMPP c : components)
	{
		if (c == nullptr)
			continue;

		if (c->isAlive())
			c->kill();

		pmap<T>* coords = nullptr;
		size_t size = settings.createBinayMasks ? c->getTotalSize() : 0;

		auto* bar3d = (n == 3) ? c->bar3d : nullptr;
		barline<T>* line = new barline<T>(c->start, c->end - c->start, bar3d, size);
		c->resline = line;

		if (settings.createBinayMasks)
		{
			if (c->parent == nullptr && settings.createGraph)
			{
				line->setParrent(rootNode);
			}
		}
		lines->add(line);
	}

	if (settings.createBinayMasks)
	{
		reverseCom();
	}
	if (settings.createGraph)
		lines->setRootNode(rootNode);
	else
		delete rootNode;
}

template<class T>
Baritem<T>* BarcodeCreator<T>::getBarcode()
{
	Baritem<T>* lines = new Baritem<T>();

	switch (settings.returnType)
	{

	case ReturnType::betty:
		computeBettyBarcode(lines);
		break;
	case ReturnType::barcode2d:
		computeNdBarcode(lines, 2);
		break;
	case ReturnType::barcode3d:
		computeNdBarcode(lines, 3);
		break;


		break;
	default:
		break;
	}

	return lines;
}

template<class T>
void BarcodeCreator<T>::processTypeF(const barstruct& str, const bc::DatagridProvider<T>* src, Barcontainer<T>* item)
{
	init(src, str.proctype);

	switch (str.comtype)
	{
	case  ComponentType::Component:
		processComp(b, item);
		break;
	case  ComponentType::Hole:
		processHole(b, item);
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

	delete[] sortedArr;
}

template<class T>
void BarcodeCreator<T>::processFULL(const barstruct& str, const bc::DatagridProvider<T>* img, Barcontainer<T>* item)
{
	bool rgb = (img->channels() != 1);

	if (str.coltype == ColorType::rgb || (str.coltype == ColorType::native && rgb))
	{
		if (img->channels() != 1)
		{
			std::vector<BarImg<T>*> bgr;
			split(*img, bgr); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

			for (size_t i = 0; i < bgr.size(); i++)
				processTypeF(str, bgr[i], item);
		}
		else
		{
			const DatagridProvider<T>* temoing = dynamic_cast<const DatagridProvider<T>*>(img);

			processTypeF(str, temoing, item);
			Baritem<T>* last = item->lastItem();
			item->addItem(new Baritem<T>(*last));
			item->addItem(new Baritem<T>(*last));
		}
	}
	else
	{
		if (img->channels() != 1)
		{
			bc::BarImg<T> res(1, 1);
			cvtColorV3B2U1C(*img, res);

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

	for (const auto& it : settings.structure)
	{
		processFULL(it, img, cont);
	}
	return cont;
}

template<class T>
Barcontainer<T>* BarcodeCreator<T>::createBarcode(bcBarImg* img, const std::vector<barstruct>& structure)
{
	Barcontainer<T>* cont = new Barcontainer<T>();
	settings.checkCorrect();

	for (const auto& it : structure)
	{
		processFULL(it, img, cont);
	}
	return cont;
}

template<class T>
Barcontainer<T>* BarcodeCreator<T>::createBarcode(bcBarImg* src, const barstruct* structure, int size)
{
	Barcontainer<T>* cont = new Barcontainer<T>();
	settings.checkCorrect();

	for (size_t i = 0; i < size; i++)
	{
		processFULL(structure[i], src, cont);
	}
	return cont;
}



template<class T>
Barcontainer<T>* BarcodeCreator<T>::createSLbarcode(const bcBarImg* src, T foneStart, T foneEnd, Barcontainer<T>* cont)
{
	if (cont == nullptr)
		cont = new Barcontainer<T>();

	init(src, ProcType::f0t255);

	const bcBarImg& img = *workingImg;

	const size_t len = totalSize - 1;
	size_t foneStartI = 0;
	foneStart = MAX(foneStart, img.get(sortedArr[0]));
	foneStart = MIN(foneStart, img.get(sortedArr[len]));

	size_t foneEndI = 0;
	foneEnd = MAX(foneEnd, img.get(sortedArr[0]));
	foneEnd = MIN(foneEnd, img.get(sortedArr[len]));

	for (size_t i = 0; i < totalSize; ++i)
	{
		curbright = img.get(sortedArr[i]);
		if (foneStart == 0 && curbright >= foneStart) {
			foneStartI = i;
			break;
		}
		if (foneEndI == 0) {
			if (curbright > foneEnd) {
				foneEndI = i - 1;
				break;
			}
			if (curbright == foneEnd) {
				foneEndI = i;
				break;
			}
		}
	}
	size_t off = len;
	for (size_t i = foneEndI; i < off; ++i, --off) {
		auto temp = sortedArr[i];
		sortedArr[i] = sortedArr[off];
		sortedArr[off] = temp;
	}
	off = len;
	processHole(b, cont);

	delete[] sortedArr;
	delete[] included;

	return cont;
}

template<class T>
Barcontainer<T>* BarcodeCreator<T>::createSLbarcode(const bcBarImg* src, T foneStart, T foneEnd, bool createRGBbar)
{
	Barcontainer<T>* cont = new Barcontainer<T>();
	if (!createRGBbar)
		return createSLbarcode(src, foneStart, foneEnd, cont);

	if (src->channels() == 3)
	{
		std::vector<bcBarImg*>  bgr;
		//bc::BarImg<BarVec3b>* ds = dynamic_cast<bc::BarImg<BarVec3b>*>(src);
		//split(ds, bgr);
		Barbase<T>* b = createSLbarcode(bgr[0], foneStart, foneEnd, cont);
		Barbase<T>* g = createSLbarcode(bgr[1], foneStart, foneEnd, cont);
		Barbase<T>* r = createSLbarcode(bgr[2], foneStart, foneEnd, cont);

		return cont;
	}
	else
	{
		createSLbarcode(src, foneStart, foneEnd, cont);
		cont->addItem(cont->lastItem()->clone());
		cont->addItem(cont->lastItem()->clone());

		return cont;
	}
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
Barcontainer<float>* BarcodeCreator<float>::searchHoles(float* img, int wid, int hei)
{
	this->wid = wid;
	this->hei = hei;
	totalSize = static_cast<size_t>(wid) * hei;
	included = new Include<float>[totalSize];
	memset(included, 0, totalSize * sizeof(Include<float>));

	settings.createBinayMasks = false;
	settings.createGraph = false;
	settings.returnType = ReturnType::barcode2d;
	workingImg = new BarImg<float>(wid, hei, 1, reinterpret_cast<uchar*>(img), false, false);
	auto* arr = sort(bc::ProcType::f0t255);

	for (size_t i = 0; i < totalSize; ++i)
	{
		auto& val = arr[i];
		curpix = point(val.x, val.y);
		curbright = workingImg->get(curpix);
#ifdef VDEBUG
		VISULA_DEBUG_COMP(totalSize, i);
#else
		checkCloserB0();
#endif
	}

	delete[] arr;



	Barcontainer<float>* item = new Barcontainer<float>();

	addItemToCont(item);
	clearIncluded();
	return item;

	return nullptr;
}


template<class T>
Barcontainer<T>* BarcodeCreator<T>::searchHoles(float* img, int wid, int hei)
{
	return nullptr;
}

INIT_TEMPLATE_TYPE(bc::BarcodeCreator)

//template bc::Barcontainer<uchar>* BarcodeCreator<uchar>::createBarcode(const bc::DatagridProvider<uchar>*, const BarConstructor<uchar>&)

