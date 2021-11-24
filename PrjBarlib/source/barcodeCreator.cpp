#include "barcodeCreator.h"
#include <stack>
#include <string>

#undef VDEBUG

void bc::barcodeCreator::drawColorMap(int delay)
{
    if (delay < 0)
        return;
    ushort xy;
    cv::Mat colorMap(hei, wid, CV_8UC3);
    for (int x = 0; x < wid; ++x) {
        for (int y = 0; y < hei; ++y) {
            xy = barMap.at<ushort>(y, x);
            if (xy != 0)
                colorMap.at<cv::Vec3b>(y, x) = colors[xy % colors.size()];
        }
    }
    std::string name = "colro map";
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    const int corWin = 600;
    const int corHei = 500;
    if (wid > hei)
    {
        float ad = (float)corWin / wid;
        cv::resizeWindow(name, corWin, (int)(hei * ad));
    }
    else
    {
        float ad = (float)corHei / hei;
        cv::resizeWindow(name, (int)(wid * ad), corHei);
    }
    cv::imshow(name, colorMap);
    cv::waitKey(delay);
    //    cv::imwrite("colormap.jpg",colorMap);
}

void bc::barcodeCreator::draw(std::string name)
{
    int wd = wid * 10;
    int hi = hei * 10;
    cv::Mat img(hi, wd, CV_8UC3, cv::Scalar(255, 255, 255));
    cv::Vec3b v(100, 100, 100);
    size_t size = colors.size();
    //если 2 занимают одну клтку
    bool* arr = new bool[wid * hei];
    for (int i = 0, total = wid * hei; i < total; ++i) arr[i] = false;

    for (int i = 0; i < hi; i += 10)
        for (int j = 0; j < wd; j++)
        {
            img.at<cv::Vec3b>(i, j) = v;
            img.at<cv::Vec3b>(i + 1, j) = v;
        }


    for (int j = 0; j < wd; j += 10)
        for (int i = 0; i < hi; i++)
        {
            img.at<cv::Vec3b>(i, j) = v;
            img.at<cv::Vec3b>(i, j + 1) = v;
        }

    int k = 0;
    int nk = 0;


    //int p = 510 / unice.size();
    for (auto pcomp : components)
    {
        if (pcomp == nullptr)
            continue;

        Hole* phole = dynamic_cast<Hole*>(pcomp);
        int tic = 1;
        int marc = cv::MARKER_TILTED_CROSS;
        cv::Vec3b col;

        if (phole != nullptr)
        {
            if (phole->isValid) {
                if (phole->getIsOutside()) {
                    col = cv::Vec3b(0, 0, 10);
                    tic = 2;
                    marc = cv::MARKER_CROSS;
                }
                else {
                    col = colors[k % size];
                    marc = cv::MARKER_TILTED_CROSS;
                    ++nk;
                }
            }
            else {
                marc = cv::MARKER_DIAMOND;
                col = cv::Vec3b(0, 0, 255);
            }
            for (ppair coord : *phole->coords)
            {
                cv::Point p = coord.first.cvPoint();
                bool exs = arr[p.y * wid + p.x];
                arr[p.y * wid + p.x] = !phole->getIsOutside();

                if (marc == cv::MARKER_TILTED_CROSS) {
                    std::string s = std::to_string(pcomp->num);

                    auto plan = cv::FONT_HERSHEY_COMPLEX;
                    int tic = 1;
                    float ds = cv::getFontScaleFromHeight(plan, 8, tic);
                    int baseline = 0;
                    cv::Size textSize = cv::getTextSize(s, plan, ds, tic, &baseline);
                    if (textSize.width > 10) {
                        ds *= 0.6;
                        textSize.width *= 0.6;
                    }
                    p.x = p.x * 10 + (10 - textSize.width);
                    p.y = p.y * 10 + (10 - textSize.height) / 3;

                    cv::putText(img, s, p, plan, ds, cv::Scalar(0, 0, 0), tic, cv::LINE_4);

                    //                    int height = 10, bottom, width = 10;
                    //                    int font = cv::FONT_HERSHEY_COMPLEX_SMALL; //defining font
                    //                    double scale = ((float) (height - 3)) / 20.0; //So far, I just try to divide height by 20
                    //                    cv::Size textSize = cv::getTextSize(s, font, scale, 1, &bottom); //Geting text rect
                    //                    cv::Point textOrg((width - textSize.width) / 2 + p.x, (height + textSize.height) / 2 + p.y); //Computing point to center the text in the button
                    //                    cv::putText(img, s, textOrg, font, scale, cv::Scalar(0, 0, 0), 1);

                }
                else {
                    p.x = p.x * 10 + 5;
                    p.y = p.y * 10 + 5;
                    cv::drawMarker(img, p, col, exs ? cv::MARKER_CROSS : marc, 10, tic, cv::LINE_4);
                }
            }

        }
        else
        {
            for (ppair coord : *pcomp->coords)
            {
                cv::Point p = coord.first.cvPoint();
                auto plan = cv::FONT_HERSHEY_COMPLEX;
                int tic = 1;
                float ds = cv::getFontScaleFromHeight(plan, 8, tic);
                int baseline = 0;
                std::string s = std::to_string(pcomp->num);

                cv::Size textSize = cv::getTextSize(s, plan, ds, tic, &baseline);
                p.x = p.x * 10 + (10 - textSize.width);
                p.y = p.y * 10 + (10 - textSize.height) / 3;
                cv::putText(img, s, p, plan, ds, cv::Scalar(0, 0, 0), tic, cv::LINE_4);
            }
        }
        ++k;
    }
    delete[] arr;
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

//Образовала новый?
inline bc::Component* bc::barcodeCreator::attach(Component* main, Component* second)
{
    if (main->coords->size() < second->coords->size())//свапаем, если у первого меньше элементов. Нужно для производиельности
    {
        Component* temp = main;
        main = second;
        second = temp;
    }

    for (auto it = second->coords->begin(); it != second->coords->end(); ++it) {
        included[GETPOFF(it->first)] = main;
        //        if(createBin)
        //            main->setB(it->first);
        main->coords->insert(ppair(it->first, curbright));
        if (createMap)
            mapp((Hole*)main, it->first);
    }

    if (!createBin) {
        //        main->coords.insert(second->coords.begin(), second->coords.end());
        second->coords->clear();
    }


    second->end = curbright;
    //    main->start = MIN(second->start, main->start);

        //Еще надо проверить треугольники
    //    delete second;
    //    second = nullptr;
    return main;//возращаем единую компоненту.
}

//****************************************b**************************************
inline bool bc::barcodeCreator::checkCloserB0(point midP)
{
    Component* first = nullptr;
    Component* connected;// = new rebro(x, y); //included[{(int)i, (int)j}];
    //TODO выделять паять заранее
    static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
    for (int i = 0; i < 8; ++i)
    {
        point curPoint(midP + poss[i]);
        if (isContain(curPoint))//существует ли ребро вокруг
        {
            if (first == nullptr)
            {
                first = getComp(curPoint);
                first->add(midP);
                setInclude(midP, first);//n--nt обяз нужно
            }
            else// соединяет несколько разных компоненты
            {
                if (first->isContain(curPoint))//если в найденном уже есть этот элемент
                    continue;

                connected = getComp(curPoint);

                lastB -= 1;
                first = attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
                //included[point((int)i, (int)j)] = first;// уже в аттаче делаем это заменяем тот, что был на новый --nt
            }
        }
    }
    if (first == nullptr)
    {
        lastB += 1;

        connected = new Component(midP.x, midP.y, this);
        setInclude(midP.x, midP.y, connected);
        return true;
    }

    return false;
}
//********************************************************************************

void  bc::barcodeCreator::mapp(bc::Hole* h, const point& p)
{
    if (!createMap || !h->isValid || h->getIsOutside())
        return;

    barMap.at<ushort>(p.y, p.x) = static_cast<ushort>(h->num);
    drawColorMap(-1);
    //    barMap.at<ushort>()
}

int bc::barcodeCreator::GETPOFF(const bc::point& p) const {
    return wid * p.y + p.x;
}


bool bc::barcodeCreator::isContain(int x, int y) const
{
    if (x < 0 || y < 0 || x >= wid || y >= hei)
    {
        return false;
    }
    return included[wid * y + x] != nullptr;
}


bool bc::barcodeCreator::isContain(const bc::point& p, bool valid) const
{
    if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
    {
        return false;
    }
    return (included[wid * p.y + p.x] != nullptr && ((Hole*)included[wid * p.y + p.x])->isValid == valid);
}


bool bc::barcodeCreator::isContain(const bc::point& p) const
{
    if (p.x < 0 || p.y < 0 || p.x >= wid || p.y >= hei)
    {
        return false;
    }
    return included[wid * p.y + p.x] != nullptr;
}


bc::Component* bc::barcodeCreator::getComp(int x, int y)
{
    return included[wid * y + x];
}

bc::Component* bc::barcodeCreator::getComp(const bc::point& p)
{
    return included[wid * p.y + p.x];
}

bc::Hole* bc::barcodeCreator::getHole(int x, int y)
{
    return dynamic_cast<Hole*>(included[wid * y + x]);
}

bc::Hole* bc::barcodeCreator::getHole(const bc::point& p)
{
    return dynamic_cast<Hole*>(included[wid * p.y + p.x]);
}


void bc::barcodeCreator::setInclude(int x, int y, bc::Component* comp)
{
    included[wid * y + x] = comp;
}

void bc::barcodeCreator::setInclude(const bc::point& p, bc::Component* comp)
{
    included[wid * p.y + p.x] = comp;
}


bc::Hole* bc::barcodeCreator::tryAttach(Hole* main, Hole* add, point p)
{
    if (main != add && main->findCross(p, add))
    {
        //если хотя бы одна из дыр аутсайд - одна дыра умрет. Если они обе не аутсайд - соединятся в одну и одна дыра умрет.
        //еси они уба уже аутсайды, то ничего не произйдет, получается, что они живут только если обе isOut =true, т.е.
        //умирают, если хотя бы один из них false
        //flase - жива, true - мертва
        if (main->getIsOutside() == false || add->getIsOutside() == false)
            --lastB;

        //как будет после соединения
        main->setShadowOutside(main->getIsOutside() || add->getIsOutside());
        add->setShadowOutside(main->getIsOutside());

        Hole* ret = dynamic_cast<Hole*>(attach(main, add));
        if (ret->getIsOutside() && curbright != ret->end)
            ret->end = curbright;

        return ret;//все connected и first соединились в одну компоненту first
    }
    return main;
}

//****************************************b**************************************
inline bool bc::barcodeCreator::checkCloserB1(point& p)
{
    static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
    Hole* hr = nullptr;
    point p1;
    point p2;

    //после обовления дыры  к  ней можно будет присоединить все токи вокруг нее, кроме тточ, что на противоположном углу
    for (int i = 0; i < 8; ++i)
    {
        p1 = p + poss[i];
        if (isContain(p1))
        {
            if (isContain(p + poss[i + 1]))
                p2 = p + poss[i + 1];
            else if (i % 2 == 0 && isContain(p + poss[i + 2]))
                p2 = p + poss[i + 2];
            else
                continue;//если не нашли. Проверяем только потелнциальные дыры

            Hole* h1 = (Hole*)included[GETPOFF(p1)];
            Hole* h2 = (Hole*)included[GETPOFF(p2)];
            //все проверки на out в самом конце
            //вариант 1 - они принадлежат одному объекту. Не валидные могут содержать только одну компоненту, значит, этот объект валидный
            if (h1 == h2 && h1->isValid)
            {
                add(h1, p);
                hr = h1;
                //                included[GETPOFF(p)] = hr;
            }
            //вариант 2 - h1 - валид, h2- не валид. Мы уже проверили, что треугольник p-p1-p2 есть
            //cod 402
            else if (h1->isValid && !h2->isValid)
            {
                hr = new Hole(p, p1, p2, this);
                //                if (!hr->getIsOutside())++lastB;

                delete h2;
                //                included[GETPOFF(p)] = hr;
                //                included[GETPOFF(p2)] = hr;
                h1->tryAdd(p);
                hr = tryAttach(hr, h1, p);
            }
            //вариант 3 - h1 - не  валид, h2- валид.Мы уже проверили, что треугольник p-p1-p2 есть
            //cod 402
            else if (h2->isValid && !h1->isValid)
            {
                hr = new Hole(p, p1, p2, this);
                //                if (!hr->getIsOutside()) ++lastB;
                delete h1;
                //                included[GETPOFF(p1)] = hr;
                //                included[GETPOFF(p)] = hr;
                h2->tryAdd(p);
                hr = tryAttach(hr, h2, p);
            }
            //вариант 4 - оба не валид
            else if (!h1->isValid && !h2->isValid)//не факт, что они не валидны
            {
                //Т.К. мы уже проверили вышле, что образуется треуготльник, можно смело создаать дыру
                hr = new Hole(p, p1, p2, this);
                //                if (!hr->getIsOutside())   ++lastB;
                delete h1;
                delete h2;

                //                included[GETPOFF(p)] = hr;
                //                included[GETPOFF(p1)] = hr;
                //                included[GETPOFF(p2)] = hr;
            }
            //вариант 5 - разные дыры и они валидны CDOC{590}
            else if (h1->isValid && h2->isValid && h1 != h2)
            {
                bool add1 = tryAdd(h1, p);
                bool add2 = tryAdd(h2, p);
                if (add1 && add2)
                {
                    hr = tryAttach(h1, h2, p);
                }
                else if (add1 && !add2)
                {
                    if (tryAdd(h1, p2))
                        hr = tryAttach(h1, h2, p2);
                }
                else if (!add1 && add2)
                {
                    if (tryAdd(h2, p1))
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
        hr = new Hole(p, this);
        //        included[GETPOFF(p)] = hr;
        return false;
    }

    for (int i = 0; i < 16; ++i)
    {
        point curp = p + poss[i % 8];
        if (isContain(curp))
        {
            //получена дыра
            Hole* h_t = (Hole*)included[GETPOFF(curp)];
            if (h_t == hr)
                continue;

            Hole* h2 = nullptr;

            point next = p + poss[(i % 8) + 1];
            if (isContain(next))
                h2 = (Hole*)included[GETPOFF(next)];
            if (h2 == hr || h2 == h_t)
                h2 = nullptr;

            if (!h_t->isValid)
            {
                if (h2 != nullptr && !h2->isValid)
                {
                    Hole* nh = new Hole(p, curp, next, this);
                    //                    if (!nh->getIsOutside()) ++lastB;
                    delete h_t;
                    delete h2;

                    //                    included[GETPOFF(p)] = nh;
                    //                    included[GETPOFF(curp)] = nh;
                    //                    included[GETPOFF(next)] = nh;
                }
                else if (tryAdd(hr, curp))
                    delete h_t;

            }
            //вариант 2 - она валидна
            else
            {
                bool add_t = tryAdd(h_t, p);
                bool add_r = tryAdd(hr, curp);
                if (h2 != nullptr)
                {
                    bool added = tryAdd(h_t, next);
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
                    hr = tryAttach(hr, h_t, p);
                else if (add_t && !add_r)
                {
                    hr = tryAttach(hr, h_t, p);
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
inline bool bc::barcodeCreator::tryAdd(bc::Hole* h, const point& p)
{
    return  h->tryAdd(p);
    //   bool outDo = h->isOutside;

    //   if (h->tryAdd(p))
    //   {
    //       included[GETPOFF(p)] = h;
    //       if (h->isOutside != outDo)
    //           --lastB;

    //       return true;
    //   }
    //   return false;
}
inline void bc::barcodeCreator::add(bc::Hole* h, point& p)
{
    h->add(p);
    //    bool outDo = h->isOutside;
    //    included[GETPOFF(p)] = h;

    //    if (h->isOutside != outDo)
    //        --lastB;
}

inline bc::point* bc::barcodeCreator::sort(cv::Mat* arr)
{
    int hist[256];//256
    int offs[256];//256
    for (size_t i = 0; i < 256; ++i)
    {
        hist[i] = 0;
        offs[i] = 0;
    }
    for (int i = 0; i < arr->cols; ++i)//wid
    {
        for (int j = 0; j < arr->rows; ++j)//hei
        {
            auto p = arr->at<uchar>((int)j, (int)i);
            ++hist[p];//можно vector, но хз
        }
    }


    for (size_t i = 1; i < 256; ++i)
    {
        hist[i] += hist[i - 1];
        offs[i] = hist[i - 1];
    }

    point* data = new point[(size_t)arr->cols * (size_t)arr->rows];//256
    for (int i = 0; i < arr->cols; ++i)//wid
    {
        for (int j = 0; j < arr->rows; ++j)//hei
        {
            auto p = arr->at<uchar>((int)j, (int)i);
            data[offs[p]++] = point((int)i, (int)j);
        }
    }
    //delete[] hist;
    //delete[] offs;

    return data;
}


bc::barcodeCreator::barcodeCreator()
{
    lastB = 0;
    lastB = 0;
}

void bc::barcodeCreator::init(const cv::Mat& src, cv::Mat& img)
{
    if (src.channels() != 1)
        cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    else
        img = src;
    //    components.clear();
    //barMap.release();
    if (createMap) barMap = cv::Mat(img.rows, img.cols, CV_16U, cv::Scalar(0));
    //    cv::imwrite("D:\\2.png", img);
    wid = img.cols;
    hei = img.rows;
    totalSize = img.cols * img.rows;
    included = new Component * [totalSize];
    for (size_t i = 0; i < totalSize; ++i)
        included[i] = nullptr;


    //от 255 до 0
    sortedArr = sort(&img);
    for (int i = 0; i < 256; ++i) {
        b[i] = 0;
        b[i] = 0;
    }
    lastB = 0;
    lastB = 0;
    if (colors.size() == 0)
    {
        for (int b = 0; b < 255; b += 20)
            for (int g = 255; g > 20; g -= 20)
                for (int r = 0; r < 255; r += 20)
                    colors.push_back(cv::Vec3b(b, g, r));
    }

    //bin0t255.clear();
    //bin255t0.clear();
}

bc::Barcode* bc::barcodeCreator::createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd)
{
    cv::Mat img;
    init(src, img);

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
    processHole0to255(img, b);
    //    for (size_t i = 0; i < totalSize; ++i) {
    //        point pix = sortedArr[i];
    //        uchar cur;
    //        if (i < foneStartI)
    //            cur= img.at<uchar>(pix.cvPoint());
    //        else
    //            cur =img.at<uchar>(sortedArr[off--].cvPoint());
    //        ///////////////////////////////////////////////////
    //#ifdef VDEBUG
    //        VISULA_DEBUG(totalSize, i, pix);
    //#else
    //        checkCloserB1(pix);
    //#endif
    //        ///////////////////////////////////////////////////
    //        if (i != len) {
    //            short scnd =i<foneStartI? img.at<uchar>(sortedArr[i + 1].cvPoint()):img.at<uchar>(sortedArr[off].cvPoint());
    //            if (cur != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
    //                for (int k = cur; k < scnd; ++k)
    //                    b[k] = lastB;
    //        } else b[cur] = lastB;
    //    }

    //    assert(((void)"ALARM! b is not zero", lastB == 0));

    //    clearIncluded();
    delete[] sortedArr;
    delete[] included;

    return getSLbarcode(b);
}

bc::Barbase* bc::barcodeCreator::createSLbarcode(const cv::Mat& src, uchar foneStart, uchar foneEnd, bool createRGBbar)
{
    if (!createRGBbar)
        return createSLbarcode(src, foneStart, foneEnd);
    if (src.channels() == 3) {
        std::vector<cv::Mat>  bgr;
        cv::split(src, bgr);
        Barbase* b = createSLbarcode(bgr[0], foneStart, foneEnd);
        Barbase* g = createSLbarcode(bgr[1], foneStart, foneEnd);
        Barbase* r = createSLbarcode(bgr[2], foneStart, foneEnd);

        return new BarcodeRGB(b, g, r, false);
    }
    else {
        auto e = createSLbarcode(src, foneStart, foneEnd);
        BarcodeRGB* ret = new BarcodeRGB(e, e, e, true);
        delete e;

        return ret;
    }
}

void bc::barcodeCreator::addToBin0()
{
    if (createBin)
    {
        cv::Mat t(hei, wid, CV_8U);
        for (size_t i = 0; i < totalSize; ++i)
        {
            Component* c = included[i];
            t = (c != nullptr);
        }
        //bin0t255.push_back(t);
    }
}

void bc::barcodeCreator::addToBin255()
{
    if (createBin)
    {
        cv::Mat t(hei, wid, CV_8U);
        for (size_t i = 0; i < totalSize; ++i)
        {
            Component* c = included[i];
            t = (c != nullptr);
        }
        //bin255t0.push_back(t);
    }
}


void bc::barcodeCreator::processHole0to255(cv::Mat& img, int* retBty, Barcontainer* item)
{
    size_t len = totalSize - 1;

    for (size_t i = 0; i < totalSize; ++i) {
        point pix = sortedArr[i];
        curbright = img.at<uchar>(pix.cvPoint());
#ifdef VDEBUG
        VISULA_DEBUG(totalSize, i, pix);
#else
        checkCloserB1(pix);
#endif

        if (i != len) {
            uchar scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
            if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
            {
                for (int k = curbright; k < scnd; ++k) {
                    retBty[k] = lastB;
                    //                    addToBin0();
                }
            }
        }
        else
        {
            retBty[curbright] = lastB;
            //            addToBin0();
        }
    }
    assert(((void)"ALARM! b is not zero", lastB == 0));
    addItemToCont(item);
    clearIncluded();
    lastB = 0;
}

void bc::barcodeCreator::processComp0to255(cv::Mat& img, int* retBty, bc::Barcontainer* item)
{
    size_t len = totalSize - 1;

    for (size_t i = 0; i < totalSize; ++i) {
        point pix = sortedArr[i];
        curbright = img.at<uchar>(pix.cvPoint());
#ifdef VDEBUG
        VISULA_DEBUG_COMP(totalSize, i, pix);
#else
        checkCloserB0(pix);
#endif

        if (i != len) {
            uchar scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
            if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
            {
                for (int k = curbright; k < scnd; ++k) {
                    retBty[k] = lastB;
                    //                    addToBin0();
                }
            }
        }
        else {
            retBty[curbright] = lastB;
            //            addToBin0();
        }
    }
    assert(((void)"ALARM! b is not one", lastB == 1));
    addItemToCont(item);
    clearIncluded();
    lastB = 0;
}

void bc::barcodeCreator::processHole255to0(cv::Mat& img, int* retBty, Barcontainer* item)
{
    size_t len = totalSize - 1;

    for (size_t i = len;; --i) {
        point pix = sortedArr[i];
        curbright = 255 - img.at<uchar>(pix.cvPoint());

#ifdef VDEBUG
        VISULA_DEBUG(totalSize, i, pix);
#else
        checkCloserB1(pix);
#endif
        curbright = 255 - curbright;
        if (i != 0) {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (curbright != scnd) {
                for (short k = curbright; k > scnd; --k) {
                    retBty[255 - k] = lastB; //2222
                    addToBin255();
                }
            }
        }
        else {
            retBty[255 - curbright] = lastB; //2222
            addToBin255();
        }

        if (i == 0)
            break;
    }
    assert(((void)"ALARM! b is not zero", lastB == 0));
    addItemToCont(item);
    clearIncluded();
    lastB = 0;
}


void bc::barcodeCreator::processComp255to0(cv::Mat& img, int* retBty, bc::Barcontainer* item)
{
    size_t len = totalSize - 1;

    for (size_t i = len;; --i) {
        point pix = sortedArr[i];
        curbright = img.at<uchar>(pix.cvPoint());

#ifdef VDEBUG
        VISULA_DEBUG_COMP(totalSize, i, pix);
#else
        checkCloserB0(pix);
#endif
        if (i != 0)
        {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (curbright != scnd) {
                for (short k = curbright; k > scnd; --k) {
                    retBty[255 - k] = lastB; //2222
                    addToBin255();
                }
            }
        }
        else
        {
            retBty[255 - curbright] = lastB; //2222
            addToBin255();
        }

        if (i == 0)
            break;
    }
    assert(((void)"ALARM! b is not one", lastB == 1));
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
        bc::Baritem* b = new bc::Baritem();
        for (Component* h : components)
        {
            if (h == nullptr)
                continue;

            cv::Mat z;
            pmap* map = nullptr;
            if (createBin)
            {
                //z = cv::Mat::zeros(hei, wid, CV_8UC1);
                for (auto p = h->coords->begin(); p != h->coords->end(); ++p) {
                    //                    auto val = z.at<uchar>(p->first.y, p->first.x);
                    p->second = abs(h->end - p->second);
                    //                    z.at<uchar>(p->first.y, p->first.x) = abs(h->end - p->second);
                }
                map = h->coords;
            }

            b->add(h->start, h->end - h->start, map);
        }

        container->addItem(b);
    }
}

bc::BarcodeTwo* bc::barcodeCreator::createTwoSlbarcode(const cv::Mat& src)
{
    cv::Mat img;
    init(src, img);
    processHole0to255(img, b);
    //    size_t len = totalSize - 1;

    //    for (size_t i = 0; i < totalSize; ++i) {
    //        point pix = sortedArr[i];
    //#ifdef VDEBUG
    //        VISULA_DEBUG(totalSize, i, pix);
    //#else
    //        checkCloserB1(pix);
    //#endif
    //        curbright = img.at<uchar>(pix.cvPoint());

    //        if (i != len) {
    //            uchar scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
    //            if (curbright != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
    //            {
    //                for (int k = curbright; k < scnd; ++k)
    //                    b[k] = lastB;
    //            }
    //        } else
    //            b[curbright] = lastB;
    //    }
    //    assert(((void) "ALARM! b is not zero", lastB == 0));
    //    clearIncluded();
    lastB = 0;
    ///////////////////////////////

    for (short i = 0; i < 256; ++i)
        b[i] = 0;
    processHole255to0(img, b);

    //    for (size_t i = len;; --i) {
    //        point pix = sortedArr[i];
    //        curbright = img.at<uchar>(pix.cvPoint());

    //#ifdef VDEBUG
    //        VISULA_DEBUG(totalSize, i, pix);
    //#else
    //        checkCloserB1(pix);
    //#endif
    //        if (i != 0) {
    //            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
    //            if (curbright != scnd) {
    //                for (short k = curbright; k > scnd; --k)
    //                    b[255-k] = lastB; //2222
    //            }
    //        } else
    //            b[255 - curbright] = lastB; //2222

    //        if (i == 0)
    //            break;
    //    }
    //    assert(((void) "ALARM! b is not zero", lastB == 0));

    auto bc0 = getSLbarcode(b);
    auto bc1 = getSLbarcode(b);

    clearIncluded();

    delete[] sortedArr;
    delete[] included;
    if (createBin)
    {
        //        for (auto v : components)
        //            v->binmap -= 255;
    }
    return new BarcodeTwo(bc0, bc1, false);
}

bc::BarcodeTwo* bc::barcodeCreator::createTwoSlbarcode(const cv::Mat& src, bool createRGBbar)
{
    if (!createRGBbar)
        return createTwoSlbarcode(src);
    if (src.channels() == 3) {
        std::vector<cv::Mat> bgr;
        cv::split(src, bgr);
        BarcodeTwo* b = createTwoSlbarcode(bgr[0]);
        BarcodeTwo* g = createTwoSlbarcode(bgr[1]);
        BarcodeTwo* r = createTwoSlbarcode(bgr[2]);
        BarcodeRGB* first = new BarcodeRGB(r->one, g->one, b->one, true);
        BarcodeRGB* second = new BarcodeRGB(r->two, g->two, b->two, true);
        delete b;
        delete g;
        delete r;
        return new BarcodeTwo(first, second, false);
    }
    else {
        BarcodeTwo* rgb = createTwoSlbarcode(src);
        BarcodeRGB* first = new BarcodeRGB(rgb->one, rgb->one, rgb->one, true);
        BarcodeRGB* second = new BarcodeRGB(rgb->two, rgb->two, rgb->two, true);
        delete rgb;
        return new BarcodeTwo(first, second, false);
    }
}

//#include <QDebug>
void bc::barcodeCreator::VISULA_DEBUG(int y, int i, point& pix)
{
    checkCloserB1(pix);


    //    std::cout << "; L:" << lastB << std::endl;

    //    int k = 0;
    //    for (Component *c : components) {
    //        if (c == nullptr)
    //            continue;

    //        Hole * h= dynamic_cast<Hole *>(c);
    //        if (h->isValid && !h->getIsOutside())
    //            ++k;
    //    }
    //    if (k != lastB)
    {
        // qDebug() << k << "!=" << lastB;
        draw("main");
        cv::waitKey(0);
    }
}

void bc::barcodeCreator::VISULA_DEBUG_COMP(int y, int i, point& pix)
{
    checkCloserB0(pix);

    draw("main");
    cv::waitKey(0);
}

std::vector<std::vector<cv::Point>> bc::barcodeCreator::segmetbar(cv::Mat src, bool reverse, uchar terminateBright)
{
    cv::Mat img;
    createMap = true;
    init(src, img);

    auto sortedArr = sort(&img);
    std::vector<std::vector<cv::Point>> loacs;


    if (reverse)
    {
        for (size_t i = 0, off = totalSize - 1; i < totalSize / 2; ++i, --off) {
            auto temp = sortedArr[i];
            sortedArr[i] = sortedArr[off];
            sortedArr[off] = temp;
        }
    }

    for (size_t i = 0; i < totalSize; ++i)
    {
        point pix = sortedArr[i];
        curbright = img.at<uchar>(pix.cvPoint());
        checkCloserB1(pix);
        if (reverse)
        {
            if (curbright <= terminateBright)
                break;
        }
        else
        {
            if (curbright >= terminateBright)
                break;
        }
    }
    typedef std::pair<ushort, std::vector<cv::Point>> unpair;
    std::unordered_map<ushort, std::vector<cv::Point>> unice;
    ushort xy;
    for (int x = 1; x < wid - 1; ++x)
    {
        for (int y = 1; y < hei - 1; ++y)
        {
            xy = barMap.at<ushort>(y, x);
            if (xy == 0)
                continue;
            ushort mxy = barMap.at<ushort>(y, x - 1);
            ushort xmy = barMap.at<ushort>(y - 1, x);
            ushort pxy = barMap.at<ushort>(y, x + 1);
            ushort xpy = barMap.at<ushort>(y + 1, x);

            if (xy != mxy || xy != xmy || xy != pxy || xy != xpy)
            {
                if (unice.find(xy) == unice.end())
                    unice.insert(unpair(xy, { cv::Point(x, y) }));
                else
                    unice[xy].push_back(cv::Point(x, y));
            }
        }
    }
    drawColorMap(1);

    for (unpair c : unice) {
        loacs.push_back(c.second);
        //        cv::Rect r{999999, 9999999, 0, 0};
        for (cv::Point& p : c.second) {
            barMap.at<ushort>(p.y, p.x) = USHRT_MAX;
        }

    }
    unice.clear();

    std::string name = "bar map";
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    cv::imshow(name, barMap);
    cv::imwrite("barMap.jpg", barMap);
    cv::waitKey(1);

    barMap.release();
    clearIncluded();

    delete[] sortedArr;
    delete[] included;

    return loacs;
}



std::vector<std::vector<cv::Point>> bc::barcodeCreator::segmetbarFull(cv::Mat src, uchar terminateBright)
{
    auto loacs = segmetbar(src, true, terminateBright);
    auto loacs2 = segmetbar(src, true, terminateBright);
    loacs.insert(loacs.begin(), loacs2.begin(), loacs2.end());
    return loacs;
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
    for (size_t i = 0; i < totalSize; ++i)
        included[i] = nullptr;

    components.clear();
}
//соединяет дыры
inline void bc::barcodeCreator::clear()
{
    //b.clear();
    //b.clear();
    barMap.release();
}

bc::barcodeCreator::~barcodeCreator()
{
    clear();
}

bool compareLines(const  bc::bline& i1, const  bc::bline& i2)
{
    if (i1.len == i2.len)
        return i1.start > i2.start;

    return (i1.len > i2.len);
}

bc::Barcode* bc::barcodeCreator::getSLbarcode(int* points)
{
    Barcode* lines = new Barcode();
    if (useCorrectSE)
    {
        for (Component* c : components)
            if (c != nullptr)
                lines->add(c->start, c->end - c->start);

    }
    else
    {
        std::stack<uchar> tempStack;

        for (short i = 0; i < 256; ++i)
        {
            int p = points[i];
            if (i > 0)
            {
                int pred = points[i - 1];
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
    init(src, img);

    if (str.comtype == bc::ComponentType::Component)
    {
        if (str.proctype == bc::ProcType::f0t255)
            processComp0to255(img, b, item);
        else
            processComp255to0(img, b, item);
    }
    else
    {
        if (str.proctype == bc::ProcType::f0t255)
            processHole0to255(img, b, item);
        else
            processHole255to0(img, b, item);
    }

    delete[] sortedArr;
    delete[] included;
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
            b[i] = 0;
        }
        processFULL(it, src, cont);
    }
    return cont;
}
