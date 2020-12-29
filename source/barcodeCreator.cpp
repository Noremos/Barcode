#include "barcodeCreator.h"
#include <stack>
#include <string>

//#define VDEBUG

void bc::barcodeCreator::draw(std::string name)
{
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
		Component *comp = included[i];
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

		marc = cv::MARKER_TILTED_CROSS;
		for (auto& ps : (*comp->coords))
		{
			cv::Point psc = ps.first.cvPoint();
			psc.x = psc.x * 10 + 5;
			psc.y = psc.y * 10 + 5;
			cv::drawMarker(img, psc, col, marc, 10, tic, cv::LINE_4);
		}
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
}

//#define CREATE_NEW
//Образовала новый?
inline bc::Component* bc::barcodeCreator::attach(Component* main, Component* second)
{
#ifndef CREATE_NEW
	// ***************************************************
	if (main->coords->size() < second->coords->size()) //свапаем, если у первого меньше элементов. Нужно для производиельности
	{
		Component *temp = main;
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
#endif
#ifdef CREATE_NEW
//	else
	{
		//	for (auto it = second->coords->begin(); it != second->coords->end(); ++it)
		//		main->add(it->first);
		//if (!createBin)
		//	second->coords->clear();
		// ***********************************

		Component *newOne = new Component(this, true);
		main->setParrent(newOne);
		second->setParrent(newOne);
		main->kill();
		second->kill();
		return newOne;
	}
#endif
}

//****************************************B0**************************************
inline bool bc::barcodeCreator::checkCloserB0()
{
	Component* first = nullptr;
	Component* connected;// = new rebro(x, y); //included[{(int)i, (int)j}];
	//TODO выделять паять заранее
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	first = getComp(curpix);

	for (int i = 0; i < 8; ++i)
	{
		point curPoint(curpix + poss[i]);
		if (isContain(curPoint))//существует ли ребро вокруг
		{
			if (first == nullptr)
			{
				first = getComp(curPoint);
				first->add(curpix);
                //setInclude(midP, first);//n--nt обяз нужно
			}
			else// соединяет несколько разных компоненты
			{
				if (first->isContain(curPoint))//если в найденном уже есть этот элемент
					continue;

				connected = getComp(curPoint);

				//lastB -= 1;
				first = attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
			}
		}
	}
	if (first == nullptr)
	{
		//lastB += 1;

		connected = new Component(curpix, this);
        //setInclude(midP.x, midP.y, connected);
		return true;
	}

	return false;
}
//********************************************************************************


int bc::barcodeCreator::GETPOFF(const bc::point& p) const {
	return wid * p.y + p.x;
}


bool bc::barcodeCreator::isContain(int x, int y) const
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return false;

	return included[wid * y + x] != nullptr;
}


bool bc::barcodeCreator::isContain(const bc::point& p, bool valid) const
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return false;

	return (included[wid * p.y + p.x] != nullptr && ((Hole*)included[wid * p.y + p.x])->isValid == valid);
}

bool bc::barcodeCreator::isContain(const bc::point& p) const
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return false;

	return included[wid * p.y + p.x] != nullptr;
}

bc::Component* bc::barcodeCreator::getComp(int x, int y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

	auto itr = included[wid * y + x];
	return itr ? (itr->getMaxParrent() ? itr->getMaxParrent() : itr) : nullptr;
}

bc::Component* bc::barcodeCreator::getComp(const bc::point& p)
{
	if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
		return nullptr;

	auto itr = included[wid * p.y + p.x];
	return itr ? (itr->getMaxParrent() ? itr->getMaxParrent() : itr) : nullptr;
}

bc::Hole* bc::barcodeCreator::getHole(int x, int y)
{
	if (x < 0 || y < 0 || x >= wid || y >= hei)
		return nullptr;

    auto itr = included[wid * y + x];
    return dynamic_cast<Hole*>(itr->getMaxParrent() ? itr->getMaxParrent() : itr);
}

bc::Hole* bc::barcodeCreator::getHole(const bc::point& p)
{
    auto itr = included[wid * p.y + p.x];
    return dynamic_cast<Hole*>(itr->getMaxParrent() ? itr->getMaxParrent() : itr);
}

void bc::barcodeCreator::setInclude(int x, int y, bc::Component* comp)
{

    included[wid * y + x] = comp;
    if (createBin)
        matrix[wid * y + x] = curbright;
}

void bc::barcodeCreator::setInclude(const bc::point& p, bc::Component* comp)
{
    included[wid * p.y + p.x] = comp;
    if (createBin)
        matrix[wid * p.y + p.x] = curbright;
}

bc::Hole* bc::barcodeCreator::tryAttach(Hole* main, Hole* add, point p)
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

		Hole* ret = dynamic_cast<Hole*>(attach(main, add));
		//if (ret->getIsOutside() && curbright != ret->end)
		//	ret->end = curbright;

		return ret;//все connected и first соединились в одну компоненту first
	}
	return main;
}

//****************************************B1**************************************
inline bool bc::barcodeCreator::checkCloserB1()
{
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
	Hole* hr = nullptr;
	point p1;
	point p2;

	//после обовления дыры  к  ней можно будет присоединить все токи вокруг нее, кроме тточ, что на противоположном углу
	for (int i = 0; i < 8; ++i)
	{
		p1 = curpix + poss[i];
		if (isContain(p1))
		{
			if (isContain(curpix + poss[i + 1]))
				p2 = curpix+ poss[i + 1];
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
		hr = new Hole(curpix, this);
		return false;
	}

	for (int i = 0; i < 16; ++i)
	{
		point curp = curpix + poss[i % 8];
		if (isContain(curp))
		{
			//получена дыра
            Hole* h_t = getHole(curp);
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
					Hole* nh = new Hole(curpix, curp, next, this);
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


inline bc::point* bc::barcodeCreator::sort(const cv::Mat& arr,const ProcType& type)
{
	cv::Mat mask = arr;
// bc::ProcType::experement:
	//distanceTransform(nimg, nimg, cv::DIST_L2, 3);
	// nimg.convertTo(nimg, CV_8UC1);
	//	int b = mask.type();
	//	for (int i = 0; i < arr.rows; ++i) {
	//		for (int j = 0; j < arr.cols; ++j) {
	//						int cou = 0;
	//						uchar mins[4];
	//						if (i > 0)
	//							mins[cou++] = arr.at<uchar>(i - 1, j);
	//						if (i < arr.rows - 1)
	//							mins[cou++] = arr.at<uchar>(i + 1, j);
	//						if (j > 0)
	//							mins[cou++] = arr.at<uchar>(i, j - 1);
	//						if (j < arr.cols - 1)
	//							mins[cou++] = arr.at<uchar>(i, j + 1);

	//						uchar min = 255;
	//			uchar center = arr.at<uchar>(i, j);
	//						for (int i = 0; i < cou; ++i) {
	//							if (abs(center - mins[i]) < min)
	//								min = center - mins[i];
	//						}
	//						mask.at<uchar>(i, j) = (uchar)(min);

	////			if (center > 127)
	////				center = 255 - center;
	////			mask.at<uchar>(i, j) = center;
	////			//			int st0 = MAX(i - 1, 0);
	////			int st1 = MAX(j - 1, 0);
	////			int ed0 = MIN(i + 1,arr.rows - 1);
	////			int ed1 = MIN(j + 1,arr.cols - 1);
	//			//            double min, max;
	//			//            cv::Mat zone = (*arr)(cv::Range(st0, ed0), cv::Range(st1, ed1));
	//			//            cv::minMaxLoc(zone, &min, &max);
	//			//            nimg.at<uchar>(i, j) = (uchar)( 255 -  max - min);

	//		}
	//	}

	//cv::namedWindow("ret", cv::WINDOW_NORMAL);
	//cv::imshow("ret", mask);
	//cv::waitKey(0);

	int hist[256];//256
	int offs[256];//256
	for (size_t i = 0; i < 256; ++i)
	{
		hist[i] = 0;
		offs[i] = 0;
	}

	for (int i = 0; i <arr.cols; ++i)//wid
	{
		for (int j = 0; j <arr.rows; ++j)//hei
		{
			auto p = mask.at<uchar>((int)j, (int)i);
			++hist[p];//можно vector, но хз
		}
	}

    for (size_t i = 1; i < 256; ++i)
	{
		hist[i] += hist[i - 1];
        offs[i] = hist[i - 1];
    }

	point* data = new point[(size_t)mask.cols * (size_t)mask.rows];//256
	for (int i = 0; i < mask.cols; ++i)//wid
	{
		for (int j = 0; j < mask.rows; ++j)//hei
		{
//			if (mask.channels()== 3)
//			{
//				auto p = mask.at<cv::Vec3b>((int)j, (int)i);
//				data[offs[p]++] = point((int)i, (int)j);
//			}else
			{
				auto p = mask.at<uchar>((int)j, (int)i);
				data[offs[p]++] = point((int)i, (int)j);
			}
        }
    }
	size_t total = (size_t) mask.cols * (size_t) mask.rows;
	if (type == bc::ProcType::f255t0)
	{
		for (int i = 0, ts = total / 2; i < ts; ++i)
		{
			point temp = data[i];
			data[i] = data[total - i - 1];
			data[total - i - 1] = temp;
		}
	}
	return data;
}


bc::barcodeCreator::barcodeCreator()
{
	lastB = 0;
}

void bc::barcodeCreator::init(const cv::Mat& src, const  ProcType& type, cv::Mat& img)
{
	if (src.channels() != 1)
		cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
	else
		img = src;

	if (type == bc::ProcType::f255t0)
	{
		img = 255 - img;
	}

	wid = img.cols;
	hei = img.rows;
	totalSize = img.cols * img.rows;
	included = new Component *[totalSize];
	memset(included, 0, totalSize * sizeof( Component *));


	if (createBin)
	{
		matrix = new uchar[totalSize];
		memset(matrix, 0, totalSize);
	}

	//от 255 до 0
	sortedArr = sort(img, type);
	memset(b, 0, 256 * sizeof(b[0]));

	lastB = 0;
	visualize = true;

	if (colors.size() == 0 && visualize)
	{
		for (int b = 0; b < 255; b += 20)
			for (int g = 255; g > 20; g -= 20)
				for (int r = 0; r < 255; r += 50)
					colors.push_back(cv::Vec3b(b, g, r));
	}
}
//#include <QDebug>

void bc::barcodeCreator::processHole(cv::Mat& img, int* retBty, Barcontainer* item)
{
	size_t len = totalSize - 1;
	reverse = false;

	for (size_t i = 0; i < totalSize; ++i)
	{
		curpix = sortedArr[i];
		curbright = img.at<uchar>(curpix.cvPoint());

	/*	if (i == 25)
			qDebug() << "";*/
#ifdef VDEBUG
		VISULA_DEBUG(totalSize, i);
#else
		checkCloserB1();
#endif

		if (i != len)
		{
			uchar scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
			if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
			{
				for (int k = curbright; k < scnd; ++k) {
					retBty[k] = lastB;
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

void bc::barcodeCreator::processComp(cv::Mat& img, int* retBty, bc::Barcontainer* item)
{
    //cv::imshow("res", img);
    //cv::waitKey(0);
	size_t len = totalSize - 1;
	// reverse = false;
	for (size_t i = 0; i < totalSize; ++i) {
		curpix = sortedArr[i];
		curbright = img.at<uchar>(curpix.cvPoint());
#ifdef VDEBUG
		VISULA_DEBUG_COMP(totalSize, i);
#else
		checkCloserB0();
#endif

		if (i != len)
		{
			uchar scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
			if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
			{
				for (int k = curbright; k < scnd; ++k)
					retBty[k] = lastB;
			}
		}
		else {
			retBty[(int)curbright] = lastB;
		}
	}

//    curbright = 255;
	assert(((void)"ALARM! B0 is not one", lastB == 1));
    for (auto *c : components)
    {
        if (c->isAlive())
        {
            c->kill();
            break;
        }
    }
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

void bc::barcodeCreator::addItemToCont(bc::Barcontainer* container)
{
	if (container != nullptr)
	{
		container->addItem(getBarcode());
	}
}

void bc::barcodeCreator::VISULA_DEBUG(int y, int i)
{
	checkCloserB1();

	if (visualize)
	{
		draw("main");
		cv::waitKey(0);
	}
}

void bc::barcodeCreator::VISULA_DEBUG_COMP(int y, int i)
{
	visualize = true;
	checkCloserB0();
	if (visualize)
	{
		draw("main");
		cv::waitKey(0);
	}
}

void bc::barcodeCreator::clearIncluded()
{
	for (Component* c : components)
	{
		if (c != nullptr)
		{
			delete c;
		}
	}
	components.clear();

	if (included != nullptr)
    {
		memset(included, 0, totalSize * sizeof(included[0]));
		delete[] included;
		included = nullptr;
	}
	if (createBin)
	{
		memset(matrix, 0, totalSize * sizeof(matrix[0]));
		delete[] matrix;
	}
}

bc::barcodeCreator::~barcodeCreator()
{
	clearIncluded();
	colors.clear();
}

bool compareLines(const bc::bline* i1, const bc::bline* i2)
{
	if (i1->len == i2->len)
		return i1->start > i2->start;

	return (i1->len > i2->len);
}


struct GraphVer
{
    GraphVer()
    {
	}
	GraphVer(bc::Component *comp)
    {
        this->comp = comp;
//		this->node = node;
    }
//	bc::BarNode *node;
    bc::Component *comp = nullptr;
    GraphVer *parent = nullptr;
    //bc::Component *comp;
    std::vector<GraphVer *> childrens;
};

void reverseCom(GraphVer *root)
{
    for (auto *c : root->childrens)
        reverseCom(c);

	if (root->parent)
    {
		auto &rootCoords = (*root->comp->coords);
//		auto &rootSubCoords = root->comp->subCoords;
		auto& rootParrentCoords = (*root->parent->comp->coords);
		for (int i = 0, total = rootCoords.size(); i < total; ++i)
        {
			auto &ccod = rootCoords[i];
			rootParrentCoords.push_back(ppair(ccod.first, root->parent->comp->end - root->comp->end));

//			ccod = rootSubCoords[i];
//			rootParrentCoords.push_back(
//				std::pair(ccod.first, root->parent->comp->end - root->comp->end));


			// root->parent->comp->coords->push_back(std::pair((*root->comp->coords)[i].first,(*root->comp->coords)[i].end -  root->comp->end));
//            root->parent->comp->coords->push_back(std::pair((*root->comp->coords)[i].first,(*root->comp->coords)[i].second -  root->comp->end));
        }
    }
}

bc::Baritem* bc::barcodeCreator::getBarcode()
{
	Baritem* lines = new Baritem();
	std::unordered_map<Component *, GraphVer> vers;

	std::unordered_map<Component *, BarNode *> graph;

    if (!this->useBetty)
	{
		GraphVer *root = nullptr;
		for (Component* c : components)
		{
			if (c == nullptr)
				continue;

			pmap *map = (createBin || getCoords) ? c->coords : nullptr;
			bc::bline *line = new bc::bline(c->start, c->end - c->start, map);
			BarNode *bnode;
			if (createGraph)
			{
				bnode = new BarNode(line);
				graph.insert(std::pair<Component *, BarNode *>(c, bnode));
			}

			if (createBin)// || getCoords
			{
				GraphVer *vchld, *vparnt;

				if (vers.find(c) == vers.end())
					vers.insert(std::pair<Component *, GraphVer>(c, GraphVer(c)));

				vchld = &vers[c];

				if (c->parent)
				{
					if (vers.find(c->parent) == vers.end())
						vers.insert(std::pair<Component *, GraphVer>(c->parent, GraphVer(c->parent)));

					vparnt = &vers[c->parent];

					vchld->parent = vparnt;
					vparnt->childrens.push_back(vchld);
				} else
				{
					root = vchld;
					graphRoot = bnode;
				}
			}
			lines->add(line);
		}

		if (createBin)// || getCoords
		{
			reverseCom(root);
			vers.clear();
		}

		if (createGraph)
		{
			for (Component *c : components)
			{
				if (c == nullptr)
					continue;

				if (c->parent)
					graph[c]->setParrent(graph[c->parent]);
			}

			lines->rootNode = graphRoot;
			graphRoot = nullptr;
		}
//        if (reverse)
//        {
//            for (Component* c : components)
//            {
//                for (int i = 0, total = c->coords->size(); i < total; ++i)
//                    (*c->coords)[i].second = 255 - (*c->coords)[i].second;
//            }
//        }
	}
	else
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
					auto t = tempStack.top();
					lines->add(t, i - t);
					tempStack.pop();
				}
			}
			else if (p > 0)
			{
				for (int j = 1; j <= p; j++)
					tempStack.push((uchar)i);
			}
		}
		while (!tempStack.empty()) {
			auto t = tempStack.top();
			lines->add(t, MIN(255, 256 - t));
			tempStack.pop();
		}
		std::sort(lines->bar.begin(), lines->bar.end(), compareLines);
	}

	return lines;
}

void bc::barcodeCreator::processTypeF(const bc::barstruct& str, cv::Mat& src, Barcontainer* item)
{
	cv::Mat img;
	init(src, str.proctype, img);

	switch (str.comtype)
	{
	case  bc::ComponentType::Component:
		processComp(img, b, item);
		break;
	case  bc::ComponentType::Hole:
		processHole(img, b, item);
		break;
	case  bc::ComponentType::FullPrepair:
		ProcessFullPrepair(img, b, item);
		break;
	case  bc::ComponentType::PrepairComp:
		ProcessPrepComp(img, b, settStep, item);
		break;
	default:
		break;
	}


	delete[] sortedArr;
}

void bc::barcodeCreator::processFULL(const bc::barstruct& str, cv::Mat& src, bc::Barcontainer* item)
{
	bool rgb = (src.channels() == 3);

	if (str.coltype == ColorType::rgb || (str.coltype == ColorType::native && rgb)) {
		if (src.channels() == 3) {
			std::vector<cv::Mat> bgr;
			cv::split(src, bgr);
			processTypeF(str, bgr[0], item);
			processTypeF(str, bgr[1], item);
			processTypeF(str, bgr[2], item);
		}
		else {
			processTypeF(str, src, item);
			Baritem* last = item->lastItem();
			item->addItem(new Baritem(*last));
			item->addItem(new Baritem(*last));
		}
	}
	else
		processTypeF(str, src, item);
}

bc::Barcontainer* bc::barcodeCreator::createBarcode(cv::Mat src, const std::vector<bc::barstruct>& structure)
{
	Barcontainer* cont = new Barcontainer();

	for (const auto& it : structure)
	{
		for (short i = 0; i < 256; ++i)
		{
			b[i] = 0;
		}
		processFULL(it, src, cont);
	}
	return cont;
}

#ifdef _PYD
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <Python.h>
bc::Barcontainer* bc::barcodeCreator::createBarcode(bn::ndarray& img, bp::list& structure)
{
	auto shape = img.get_shape();
	//img.
	int type = CV_8UC1;
	if (img.get_nd() == 3 && shape[2] == 3)
		type = CV_8UC3;
	cv::Mat image(shape[0], shape[1], type, img.get_data());

	//cv::imshow("test", image);
	//cv::waitKey(0);

	std::vector<bc::barstruct> cstruct;
	for (int i = 0; i < len(structure); ++i)
		cstruct.push_back(boost::python::extract<bc::barstruct>(structure[i]));

	return createBarcode(image, cstruct);
}
#endif


bc::Barcontainer* bc::barcodeCreator::createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd, Barcontainer* cont)
{
	cv::Mat img;
	init(src, bc::ProcType::f0t255, img);
	if (cont == nullptr)
		cont = new Barcontainer();

	const size_t len = totalSize - 1;
	size_t foneStartI = 0;
	foneStart = MAX(foneStart, img.at<uchar>(sortedArr[0].cvPoint()));
	foneStart = MIN(foneStart, img.at<uchar>(sortedArr[len].cvPoint()));

	size_t foneEndI = 0;
	foneEnd = MAX(foneEnd, img.at<uchar>(sortedArr[0].cvPoint()));
	foneEnd = MIN(foneEnd, img.at<uchar>(sortedArr[len].cvPoint()));

	for (size_t i = 0; i < totalSize; ++i)
	{
		curbright = img.at<uchar>(sortedArr[i].cvPoint());
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
    processHole(img, b, cont);

	delete[] sortedArr;
	delete[] included;

	return cont;
}

bc::Barcontainer* bc::barcodeCreator::createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd, bool createRGBbar)
{
	Barcontainer* cont = new Barcontainer();
	if (!createRGBbar)
		return createSLbarcode(src, foneStart, foneEnd, cont);
	if (src.channels() == 3) {
		std::vector<cv::Mat>  bgr;
		cv::split(src, bgr);
		Barbase* b = createSLbarcode(bgr[0], foneStart, foneEnd, cont);
		Barbase* g = createSLbarcode(bgr[1], foneStart, foneEnd, cont);
		Barbase* r = createSLbarcode(bgr[2], foneStart, foneEnd, cont);

		return cont;
	}
	else {
		createSLbarcode(src, foneStart, foneEnd, cont);
		cont->addItem(cont->lastItem()->clone());
		cont->addItem(cont->lastItem()->clone());

		return cont;
	}
}


// ***************************************************

using cv::Mat;

uchar dif(uchar a, uchar b)
{
	if (a > b)
		return a - b;
	else
		return b - a;
}

void bc::barcodeCreator::Prepair(Mat mat, int step)
{
	int N = 4;
	//	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
	static char poss[5][2] = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}, {-1, 0}}; //эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
	for (int c = 0; c < mat.cols; ++c)//x //wid
	{
		for (int r = 0; r < mat.rows; ++r)//y //hei
		{
			this->curbright = mat.at<uchar>(r, c);
			this->curpix = point(c, r);
			Component *mainC = getComp(c, r);
			for (int off = 0; off < N; ++off)
			{
				point p(c + poss[off][0], r + poss[off][1]);

				if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
					continue;

				Component *subC = getComp(p);
				if (subC == mainC && mainC != nullptr)
					continue;

				uchar val = mat.at<uchar>(p.cvPoint());

				if (dif(val, curbright) <= step)
				{
					if (subC == nullptr && mainC == nullptr)
					{
						mainC = new Component(this->curpix, this);
						mainC->add(p);
					}
					else if (mainC != nullptr && subC == nullptr)
					{
						mainC->add(p);
					}
					else if (mainC == nullptr && subC != nullptr)
					{
						mainC = subC;
						mainC->add(curpix);
					}
					else //if (mainC != nullptr && subC != nullptr)
					{
						mainC = attach(mainC, subC);
					}
				}
				if (mainC == nullptr)
				{
					mainC = new Component(this->curpix, this);
				}
			}
		}
	}
}

uchar bc::barcodeCreator::GetStep() const
{
	return settStep;
}

void bc::barcodeCreator::SetStep(const uchar value)
{
	settStep = value;
}


void bc::barcodeCreator::ProcessFullPrepair(cv::Mat &img, int *retBty, bc::Barcontainer *item)
{
	for (int i = 0; i < 256; ++i)
	{
		Prepair(img, i);
		retBty[i] = this->lastB;
	}

	for (auto *c : components)
	{
		if (c->isAlive())
		{
			c->kill();
			break;
		}
	}
	addItemToCont(item);
	clearIncluded();
	lastB = 0;
}

void bc::barcodeCreator::ProcessPrepComp(cv::Mat &img, int *retBty, int step, bc::Barcontainer *item)
{
	Prepair(img, step);
	processComp(img, retBty, item);
}

struct tripl
{
	int x, y;
	float val;
	tripl(int x, int y, float val): x(x), y(y), val(val)
	{

	}
	tripl(): x(-1), y(-1), val(-9999)
	{

	}
};

inline tripl* floatSort(const float* arr, int wid, int hei)
{
	size_t total = (size_t)wid * (size_t)hei;

	tripl* data = new tripl[total];//256

	size_t k = 0;
	for (int i = 0; i < total; ++i)//wid
	{
		float p = arr[i];
		data[k++] = tripl(i % wid, i / wid, p);
	}
	std::sort(data, data + total, [](tripl& a, tripl& b){
		return a.val > b.val;
	});
	return data;
}

bc::Barcontainer* bc::barcodeCreator::searchHoles(float* img, int wid, int hei)
{
	this->wid = wid;
	this->hei = hei;
	totalSize = wid * hei;
	included = new Component * [totalSize];
	memset(included, 0, totalSize * sizeof(Component*));
	
	createBin = false;
	createGraph = false;
	getCoords = true;
	useBetty = false;

	auto* arr = floatSort(img, wid, hei);

	visualize = false;

	for (size_t i = 0; i < totalSize; ++i)
	{
		auto &val = arr[i];
		curpix = point(val.x, val.y);
		curbright = val.val;
#ifdef VDEBUG
		VISULA_DEBUG_COMP(totalSize, i);
#else
		checkCloserB0();
#endif
	}

	delete[] arr;

	for (auto* c : components)
	{
		if (c->isAlive())
		{
			c->kill();
			break;
		}
	}

	Barcontainer* item = new Barcontainer();

	addItemToCont(item);
	clearIncluded();
	return item;
}

