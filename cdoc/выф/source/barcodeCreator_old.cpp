#include "barcodeCreator.h"
#include <stack>
//#define VDEBUG

void bc::barcodeCreator::draw(std::string name)
{
    int wd = wid * 10;
    int hi = hei * 10;
    cv::Mat img = cv::Mat(hi, wd, CV_8UC3, cv::Scalar(255, 255, 255));
    static std::vector<cv::Vec3b> colors;
    if (colors.size() == 0)
    {
        for (int b = 255; b > 90; b -= 40)
            for (int g = 255; g > 90; g -= 40)
                for (int r = 200; r > 90; r -= 40)
                {
                    colors.push_back(cv::Vec3b(b, g, r));
                }
    }
    int size = colors.size();
    //если 2 занимают одну клтку
    bool* arr = new bool[wid * hei];
    for (int i = 0, total = wid * hei; i < total; ++i) arr[i] = false;

    for (int i = 0; i < hi; i += 10)
        for (int j = 0; j < wd; j++)
        {
            img.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            img.at<cv::Vec3b>(i + 1, j) = cv::Vec3b(0, 0, 0);
        }


    for (int j = 0; j < wd; j += 10)
        for (int i = 0; i < hi; i++)
        {
            img.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);
            img.at<cv::Vec3b>(i, j + 1) = cv::Vec3b(0, 0, 0);
        }

    int k = 0;
    std::unordered_map<Hole*, bool> unice;
    for (int i = 0; i < coords_len; i++)
    {
        Hole* h = (Hole*)included[i];
        if (unice.find(h) == unice.end() && h != nullptr)
        {
            unice.insert(std::pair<Hole*, bool>(h, true));
            if (h->isValid && !h->isOutside)
                ++k;
        }
    }
    std::cout << "u:" << k;
    if (unice.size() == 0)
        return;
    k = 0;

    //int p = 510 / unice.size();
    for (auto& phole : unice)
    {
        int tic = 1;
        int marc = cv::MARKER_TILTED_CROSS;
        cv::Vec3b col;
        if (phole.first->isValid)
        {
            if (phole.first->isOutside)
            {
                col = cv::Vec3b(0, 0, 10);
                tic = 2;
                marc = cv::MARKER_CROSS;
            }
            else
            {
                col = colors[k % size];
                marc = cv::MARKER_TILTED_CROSS;
            }
        }
        else
        {
            marc = cv::MARKER_DIAMOND;
            col = cv::Vec3b(0, 0, 255);
        }


        for (ppair coord : phole.first->coords)
        {
            cv::Point p = coord.first.cvPoint();
            bool exs = arr[p.y * wid + p.x];
            arr[p.y * wid + p.x] = !phole.first->isOutside;

            p.x = p.x * 10 + 5;
            p.y = p.y * 10 + 5;
            cv::drawMarker(img, p, col, exs ? cv::MARKER_CROSS : marc, 10, tic, cv::LINE_4);
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
        cv::resizeWindow(name, corWin, hi * ad);
    }
    else
    {
        float ad = (float)corHei / hi;
        cv::resizeWindow(name, wd * ad, corHei);
    }
    cv::waitKey(1);
}


//Образовала новый?
inline bc::Component* bc::barcodeCreator::attach(Component* main, Component* second)
{
    if (main->coords.size() < second->coords.size())//свапаем, если у первого меньше элементов. Нужно для производиельности
    {
        Component* temp = main;
        main = second;
        second = temp;
    }

    for (auto it = second->coords.begin(); it != second->coords.end(); ++it)
        included[GETPOFF(it->first)] = main;

    main->coords.insert(second->coords.begin(), second->coords.end());
    second->coords.clear();

    //Еще надо проверить треугольники
    delete second;
    second = nullptr;
    return main;//возращаем единую компоненту.
}

//****************************************B0**************************************
inline bool bc::barcodeCreator::checkCloserB0(const int x, const int y)
{
    Component* first = nullptr;
    Component* connected;// = new rebro(x, y); //included[{(int)i, (int)j}];
    //TODO выделять паять заранее
    point midP(x, y);
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

                lastB0 -= 1;
                first = attach(first, connected);//проверить, чему равен included[point(x, y)] Не должно, ибо first заменяется на connect
                //included[point((int)i, (int)j)] = first;// уже в аттаче делаем это заменяем тот, что был на новый --nt
            }
        }
    }
    if (first == nullptr)
    {
        lastB0 += 1;

        connected = new Component(x, y);
        setInclude(x, y, connected);
        return true;
    }

    return false;
}
//********************************************************************************


bc::Hole* bc::barcodeCreator::tryAttach(Hole* main, Hole* add, point p)
{
    if (main != add && main->findCross(p, add))
    {
        //если хотя бы одна из дыр аутсайд - одна дыра умрет. Если они обе не аутсайд - онисоединятся в одну и одна дыра умрет.
        //еси они уба уже аутсайды, то ничего не произйдет, получается, что они живут только если обе isOut =true, т.е.
        //умирают, если хотя бы один из них false
        if (main->isOutside == false || add->isOutside == false)
            --lastB1;

        //как будет после соединения
        main->isOutside = main->isOutside || add->isOutside;
        add->isOutside = main->isOutside;
        return (Hole*)attach(main, add);//все connected и first соединились в одну компоненту first
    }
    return main;
}

void bc::barcodeCreator::checkB1(bc::Hole* h, bool outDo)
{
    if (h->isOutside != outDo) --lastB1;
}

//****************************************B1**************************************
inline bool bc::barcodeCreator::checkCloserB1(const int x, const int y)
{
    static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };//эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник
    point p(x, y);
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
                included[GETPOFF(p)] = hr;
            }
            //вариант 2 - h1 - валид, h2- не валид. Мы уже проверили, что треугольник p-p1-p2 есть
            //cod 402
            else if (h1->isValid && !h2->isValid)
            {
                hr = new Hole(p, p1, p2, wid, hei);
                if (!hr->isOutside)
                    ++lastB1;

                delete h2;
                included[GETPOFF(p)] = hr;
                included[GETPOFF(p2)] = hr;

                hr = tryAttach(hr, h1, p);
            }
            //вариант 3 - h1 - не  валид, h2- валид.Мы уже проверили, что треугольник p-p1-p2 есть
            //cod 402
            else if (h2->isValid && !h1->isValid)
            {
                hr = new Hole(p, p1, p2, wid, hei);
                if (!hr->isOutside) ++lastB1;
                delete h1;
                included[GETPOFF(p1)] = hr;
                included[GETPOFF(p)] = hr;

                hr = tryAttach(hr, h2, p);
            }
            //вариант 4 - оба не валид
            else if (!h1->isValid && !h2->isValid)//не факт, что они не валидны
            {
                //Т.К. мы уже проверили вышле, что образуется треуготльник, можно смело создаать дыру
                hr = new Hole(p, p1, p2, wid, hei);
                if (!hr->isOutside)
                    ++lastB1;
                delete h1;
                delete h2;

                included[GETPOFF(p)] = hr;
                included[GETPOFF(p1)] = hr;
                included[GETPOFF(p2)] = hr;
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
            {
                break;
            }
        }
    }

    if (hr == nullptr)
    {
        hr = new Hole(p, wid, hei);
        included[GETPOFF(p)] = hr;
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
                    Hole* nh = new Hole(p, curp, next, wid, hei);
                    if (!nh->isOutside) ++lastB1;
                    delete h_t;
                    delete h2;

                    included[GETPOFF(p)] = nh;
                    included[GETPOFF(curp)] = nh;
                    included[GETPOFF(next)] = nh;
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
inline bool bc::barcodeCreator::tryAdd(bc::Hole* h, point& p)
{
    bool outDo = h->isOutside;

    if (h->tryAdd(p))
    {
        included[GETPOFF(p)] = h;
        if (h->isOutside != outDo)
            --lastB1;

        return true;
    }
    return false;
}
inline void bc::barcodeCreator::add(bc::Hole* h, point& p)
{
    bool outDo = h->isOutside;
    h->add(p);
    included[GETPOFF(p)] = h;

    if (h->isOutside != outDo)
        --lastB1;
}

inline bc::point* bc::barcodeCreator::sort(cv::Mat* arr, size_t* totalSize)
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

    *totalSize = (size_t)arr->cols * (size_t)arr->rows;
    point* data = new point[*totalSize];//256
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
    lastB1 = 0;
    lastB0 = 0;
}


std::vector<std::array<int, 2>> bc::barcodeCreator::detectExp(cv::Mat src, int avg, bool createWidBar)
{
    widExists = createWidBar;
    cv::Mat img;
    if (src.channels() != 1)
        cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    else
        img = src.clone();

    wid = img.cols;
    hei = img.rows;
    coords_len = img.cols * img.rows;
    included = new Component * [coords_len];

    for (int i = 0; i < coords_len; i++)
        included[i] = nullptr;

    size_t totalLen;// = (size_t)img.rows * (size_t)img.cols;
    auto sortedArr = sort(&img, &totalLen);
    int avgImin5 = -1;
    int avgIplus5 = -1;
    for (size_t i = 0; i < totalLen; i++)
    {
        uchar col = img.at<uchar>(sortedArr[i].cvPoint());
        if (avgImin5 == -1 && col >= avg - 5)
            avgImin5 = i;
        if (avgIplus5 == -1 && col >= avg + 5)
        {
            avgIplus5 = i;
            break;
        }
    }
    if (avgIplus5 == -1)
        avgIplus5 = totalLen - 1;
    std::vector<int> bf;

    for (int i = 0; i < avgImin5; ++i)
    {
        point pix = sortedArr[i];
        uchar cur = img.at<uchar>(pix.cvPoint());
        checkCloserB1(pix.x, pix.y);
        if (i != 0)
        {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (cur != scnd)
                for (short k = cur; k > scnd; --k)
                    bf.push_back(lastB1);//2222
        }
        else bf.push_back(lastB1);//2222
    }

    for (int i = totalLen-1; i > avgIplus5; --i)
    {
        point pix = sortedArr[i];
        uchar cur = img.at<uchar>(pix.cvPoint());
        checkCloserB1(pix.x, pix.y);
        if (i != 0)
        {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (cur != scnd)
                for (short k = cur; k > scnd; --k)
                    bf.push_back(lastB1);//2222
        }
        else bf.push_back(lastB1);//2222
    }

    for (int i = avgImin5; i <= avgIplus5; i++)
    {
        point pix = sortedArr[i];
        uchar cur = img.at<uchar>(pix.cvPoint());
        checkCloserB1(pix.x, pix.y);
        if (i != 0)
        {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (cur != scnd)
                for (short k = cur; k > scnd; --k)
                    bf.push_back(lastB1);//2222
        }
        else bf.push_back(lastB1);//2222
    }
    clearIncluded(coords_len);

    delete[] sortedArr;
    return getFullWidBars(bf);
}


bc::barcode* bc::barcodeCreator::createSLbarcode(const cv::Mat &src, uchar foneStart, uchar foneEnd)
{
    cv::Mat img;
    if (src.channels() != 1)
        cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    else
        img = src.clone();
//    cv::imwrite("D:\\2.png", img);
    wid = img.cols;
    hei = img.rows;
    coords_len = img.cols * img.rows;
    included = new Component *[coords_len];
    for (int i = 0; i < coords_len; i++)
        included[i] = nullptr;

    lastB1 = 0;

    size_t totalLen; // = (size_t)img.rows * (size_t)img.cols;
    //от 255 до 0
    auto sortedArr = sort(&img, &totalLen);
    const size_t len = totalLen - 1;
    int br[256];
    for (int i = 0; i < 256; ++i)
        br[i] = 0;

    size_t foneStartI = 0;
    //size_t foneEndI = 0;
    foneStart = MAX(foneStart, img.at<uchar>(sortedArr[0].cvPoint()));
    foneStart = MIN(foneStart, img.at<uchar>(sortedArr[len].cvPoint()));


    for (size_t i = 0; i <totalLen; ++i) {
        uchar cur = img.at<uchar>(sortedArr[i].cvPoint());
        if (cur >= foneStart) {
            foneStartI = i;
            break;
        }
//        if (foneEndI == 0 && cur > foneEnd) {
//            foneEndI = i - 1;
//            break;
//        }
//        if (foneEndI == 0 && cur == foneEnd) {
//            foneEndI = i;
//            break;
//   }
    }
//    uchar last = img.at<uchar>(sortedArr[len].cvPoint());
    size_t off = len;
    for (size_t i = foneStartI; i < off; ++i, --off) {
        auto temp = sortedArr[i];
        sortedArr[i] = sortedArr[off];
        sortedArr[off] = temp;
    }
//    short inv = 2 * foneStart - last;
    off = len;
    for (size_t i = 0; i < totalLen; ++i) {
        point pix = sortedArr[i];
        uchar cur;
        if (i < foneStartI)
            cur= img.at<uchar>(pix.cvPoint());
        else
            cur =img.at<uchar>(sortedArr[off--].cvPoint());
        ///////////////////////////////////////////////////
#ifdef VDEBUG
        int y = totalLen;
        if (i <= y + 1) {//std::to_string(i)
//            cv::destroyWindow(std::to_string((i + 2)));
            draw("main");
            std::cout << "; L:" << lastB1 << std::endl;
//            if(i==884)
            cv::waitKey();
            assert(i != y + 1);
        }
#endif
        checkCloserB1(pix.x, pix.y);
#ifdef VDEBUG
        std::unordered_map<Hole*, bool> unice;
        int k = 0;
        for (int i = 0; i < coords_len; i++) {
            Hole* h = (Hole*)included[i];
            if (unice.find(h) == unice.end() && h != nullptr) {
                unice.insert(std::pair<Hole*, bool>(h, true));
                if (h->isValid && !h->isOutside)++k;
            }
        }
        assert(k == lastB1);
#endif
        ///////////////////////////////////////////////////
        if (i != len) {
            short scnd =i<foneStartI? img.at<uchar>(sortedArr[i + 1].cvPoint()):img.at<uchar>(sortedArr[off].cvPoint());
            if (cur != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
                for (int k = cur; k < scnd; ++k)
                    br[k] = lastB1;
        } else br[cur] = lastB1;
    }

    assert(((void)"ALARM! B1 is not zero", lastB1 == 0));

    clearIncluded(coords_len);
    delete[] sortedArr;
    delete[] included;

    return getSLbarcode(br);
}

bc::barbase *bc::barcodeCreator::createSLbarcode(const cv::Mat &src, uchar foneStart, uchar foneEnd, bool createRGBbar)
{
    if (!createRGBbar)
        return createSLbarcode(src, foneStart, foneEnd);
    if (src.channels() == 3) {
        std::vector<cv::Mat>  bgr;
        cv::split(src, bgr);
        barbase *b = createSLbarcode(bgr[0], foneStart, foneEnd);
        barbase *g = createSLbarcode(bgr[1], foneStart, foneEnd);
        barbase *r = createSLbarcode(bgr[2], foneStart, foneEnd);

        return new barcodeRGB(b, g, r, false);
    } else {
        auto e = createSLbarcode(src, foneStart, foneEnd);
        barcodeRGB* ret = new barcodeRGB(e, e, e, true);
        delete e;

        return ret;
    }
}

void bc::barcodeCreator::detectBs(cv::Mat src, bool createWidBar)
{
    widExists = createWidBar;
    cv::Mat img;
    if (src.channels() != 1)
        cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    else
        img = src.clone();

    wid = img.cols;
    hei = img.rows;
    coords_len = img.cols * img.rows;
    included = new Component * [coords_len];
    for (int i = 0; i < coords_len; i++)
        included[i] = nullptr;
    //b0.clear();
    lastB0 = 0;//Предыдущий. Нужен для того, чтобы прибавлять к предыд
    //b1.clear();
    lastB1 = 0;

    size_t totalLen;// = (size_t)img.rows * (size_t)img.cols;
    auto sortedArr = sort(&img, &totalLen);
    size_t len = totalLen - 1;

    for (int i = 0; i < 256; ++i) b0[i] = 0;

    for (size_t i = 0; i <  totalLen; ++i)
    {
        point pix = sortedArr[i];
        checkCloserB1(pix.x, pix.y);
        lastB0 = lastB1;
        uchar cur = img.at<uchar>(pix.cvPoint());

        if (i != len)
        {
            short scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
            if (cur != scnd)//идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
            {
                for (int k = cur; k < scnd; ++k)
                    b0[k] = lastB0;
            }
        }
        else b0[cur] = lastB0;
    }
    assert(((void)"ALARM! B0 is not zero", lastB0 == 0));

    clearIncluded(coords_len);
    lastB1 = 0;
    ///////////////////////////////


    for (short i = 0; i < 256; ++i) b1[i] = 0;

    for (int i = len; i >= 0; --i)
    {
        point pix = sortedArr[i];
        uchar cur = img.at<uchar>(pix.cvPoint());

#ifdef VDEBUG
        int y = totalLen;/*VISUAL DEBUGGING*/if (i <= y + 1) {//std::to_string(i)
            cv::destroyWindow(std::to_string((i + 2)));
            draw("main");
            std::cout << "; L:" << lastB1 << std::endl;
            cv::waitKey();
            if (i == y + 1)
                std::cout << "";
        }
#endif
        checkCloserB1(pix.x, pix.y);
#ifdef VDEBUG
        std::unordered_map<Hole*, bool> unice;
        int k = 0;
        for (size_t i = 0; i < coords_len; i++) {
            Hole* h = (Hole*)included[i];
            if (unice.find(h) == unice.end() && h != nullptr) {
                unice.insert(std::pair<Hole*, bool>(h, true));
                if (h->isValid && !h->isOutside)++k;
            }
        }
        if (k != lastB1)
            std::cout << k << "!=" << lastB1;
#endif
        if (i != 0)
        {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (cur != scnd)
            {
                for (short k = cur; k > scnd; --k)
                    b1[255-k] = lastB1;//2222
            }
        }
        else b1[255-cur] = lastB1;//2222
    }
    assert(((void)"ALARM! B1 is not zero", lastB1 == 0));


    clearIncluded(coords_len);



    delete[] sortedArr;

    delete[] included;

    if (createWidBar)
    {
        b0fullwid = getFullWidBars(b0);
        b1fullwid = getFullWidBars(b1);
    }
}

bc::barcodeTwo *bc::barcodeCreator::createTwoSlbarcode(cv::Mat src, bool createRGBbar)
{
    if (!createRGBbar)
        return createTwoSlbarcode(src);
    if (src.channels() == 3) {
        std::vector<cv::Mat> bgr;
        cv::split(src, bgr);
        barcodeTwo *b = createTwoSlbarcode(bgr[0]);
        barcodeTwo *g = createTwoSlbarcode(bgr[1]);
        barcodeTwo *r = createTwoSlbarcode(bgr[2]);
        barcodeRGB *first = new barcodeRGB(r->one, g->one, b->one, true);
        barcodeRGB *second = new barcodeRGB(r->two, g->two, b->two, true);
        delete b;
        delete g;
        delete r;
        return new barcodeTwo(first,second, false);
    } else {
        barcodeTwo *rgb = createTwoSlbarcode(src);
        barcodeRGB *first = new barcodeRGB(rgb->one, rgb->one, rgb->one, true);
        barcodeRGB *second = new barcodeRGB(rgb->two, rgb->two, rgb->two, true);
        delete rgb;
        return new barcodeTwo(first,second, false);
    }
}

bc::barcodeTwo* bc::barcodeCreator::createTwoSlbarcode(cv::Mat src)
{
    cv::Mat img;
    if (src.channels() != 1)
        cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    else
        img = src.clone();

    wid = img.cols;
    hei = img.rows;
    coords_len = img.cols * img.rows;
    included = new Component *[coords_len];
    for (int i = 0; i < coords_len; i++)
        included[i] = nullptr;

    lastB0 = 0; //Предыдущий. Нужен для того, чтобы прибавлять к предыд
    lastB1 = 0;

    size_t totalLen; // = (size_t)img.rows * (size_t)img.cols;
    auto sortedArr = sort(&img, &totalLen);
    size_t len = totalLen - 1;

    for (int i = 0; i < 256; ++i)
        b0[i] = 0;

    for (size_t i = 0; i < totalLen; ++i) {
        point pix = sortedArr[i];
        checkCloserB1(pix.x, pix.y);
        lastB0 = lastB1;
        uchar cur = img.at<uchar>(pix.cvPoint());

        if (i != len) {
            uchar scnd = img.at<uchar>(sortedArr[i + 1].cvPoint());
            if (cur != scnd) //идет от 0 до 255. если перешагиваем больше чем 1, тогда устанавливаем значения все
            {
                for (int k = cur; k < scnd; ++k)
                    b0[k] = lastB0;
            }
        } else
            b0[cur] = lastB0;
    }
    assert(((void) "ALARM! B1 is not zero", lastB0 == 0));
    clearIncluded(coords_len);
    lastB1 = 0;
    ///////////////////////////////

    for (short i = 0; i < 256; ++i)
        b1[i] = 0;

    for (int i = len; i >= 0; --i) {
        point pix = sortedArr[i];
        uchar cur = img.at<uchar>(pix.cvPoint());

#ifdef VDEBUG
        int y = totalLen; /*VISUAL DEBUGGING*/
        if (i <= y + 1) { //std::to_string(i)
            cv::destroyWindow(std::to_string((i + 2)));
            draw("main");
            std::cout << "; L:" << lastB1 << std::endl;
            cv::waitKey();
            if (i == y + 1)
                std::cout << "";
        }
#endif
        checkCloserB1(pix.x, pix.y);
#ifdef VDEBUG
        std::unordered_map<Hole *, bool> unice;
        int k = 0;
        for (size_t i = 0; i < coords_len; i++) {
            Hole *h = (Hole *) included[i];
            if (unice.find(h) == unice.end() && h != nullptr) {
                unice.insert(std::pair<Hole *, bool>(h, true));
                if (h->isValid && !h->isOutside)
                    ++k;
            }
        }
        if (k != lastB1)
            std::cout << k << "!=" << lastB1;
#endif
        if (i != 0) {
            uchar scnd = img.at<uchar>(sortedArr[i - 1].cvPoint());
            if (cur != scnd) {
                for (short k = cur; k > scnd; --k)
                    b1[255-k] = lastB1; //2222
            }
        } else
            b1[255-cur] = lastB1; //2222
    }
    assert(((void) "ALARM! B1 is not zero", lastB1 == 0));


    clearIncluded(coords_len);
//    for (int i = 0; i < 128; ++i) {
//        int temp = b1[i];
//    }


    delete[] sortedArr;

    delete[] included;

    auto bc0 = getSLbarcode(b0);
    auto bc1 = getSLbarcode(b1);
    return new barcodeTwo(bc0,bc1,false);
}

void bc::barcodeCreator::clearIncluded(int len)
{
    for (int i = 0; i < len; i++)
    {
        Component* c = included[i];
        if (c != nullptr)
        {
            if (c->coords.size() != 0)
            {
                for (auto it = c->coords.begin(); it != c->coords.end(); ++it)
                {
                    included[GETPOFF(it->first)] = nullptr;
                }
            }
            delete c;
        }

        included[i] = nullptr;
    }
}

//соединяет дыры
inline void bc::barcodeCreator::clear()
{
    //b0.clear();
    //b1.clear();
    b0fullwid.clear();
    b1fullwid.clear();
}

bc::barcodeCreator::~barcodeCreator()
{
    clear();
}

std::vector<int> bc::barcodeCreator::getWidBars(std::vector<int> points)
{
    std::vector<int> lines;
    std::stack<int> tempStack;
    for (int i = 0; i < (int)points.size(); i++)
    {
        int p = points[i];
        if (i > 0)
        {
            int pred = points[i - 1];
            if (pred == p)
                continue;
            //увеличиваем по высоте
            for (int j = pred + 1; j <= p; j++)
            {
                tempStack.push(i);
            }

            //уменьшаем по высоте
            for (int j = pred; j > p; j--)
            {
                auto t = tempStack.top();
                lines.push_back(i - t);//-1 + 1(-1 потому что тут уже нет линии, а +1чтобы точная длина была от [t до i-1])
                tempStack.pop();
            }
        }
        else if (p > 0)
        {
            for (int j = 1; j <= p; j++)
                tempStack.push(i);
        }
        while (!tempStack.empty()) {
            auto t = tempStack.top();
            lines.push_back(256 - t);
            tempStack.pop();
        }
    }
    return lines;
}

std::vector<std::array<int, 2>> bc::barcodeCreator::getFullWidBars(std::vector<int> points)
{
    std::vector<std::array<int, 2>> lines;
    std::stack<int> tempStack;

    for (size_t i = 0; i < points.size(); i++)
    {
        int p = points[i];
        if (i > 0)
        {
            int pred = points[i - 1];
            if (pred == p)// || (tempStack!=0 && tempStack[tempStack.length-1][1]==p))
                continue;

            for (int j = pred + 1; j <= p; j++)
                tempStack.push(i);

            for (int j = pred; j > p; j--)
            {
                auto t = tempStack.top();
                lines.push_back({ t, (int)i - t });
                tempStack.pop();
            }
        }
        else if (p > 0)
        {
            for (int j = 1; j <= p; j++)
                tempStack.push(i);
        }
    }
    return lines;
}



std::vector<std::array<int, 2>> bc::barcodeCreator::getFullWidBars(int* points, int len)
{
    std::vector<std::array<int, 2>> lines;
    std::stack<int> tempStack;

    for (size_t i = 0; i < len; i++)
    {
        int p = points[i];
        if (i > 0)
        {
            int pred = points[i - 1];
            if (pred == p)// || (tempStack!=0 && tempStack[tempStack.length-1][1]==p))
                continue;

            for (int j = pred + 1; j <= p; j++)
                tempStack.push(i);

            for (int j = pred; j > p; j--)
            {
                auto t = tempStack.top();
                lines.push_back({ t, (int)i - t });
                tempStack.pop();
            }
        }
        else if (p > 0)
        {
            for (int j = 1; j <= p; j++)
                tempStack.push(i);
        }
    }
    return lines;
}

bool compareLines(const  bc::bline& i1, const  bc::bline& i2)
{
    if (i1.len == i2.len)
        return i1.start > i2.start;

    return (i1.len > i2.len);
}
bc::barcode* bc::barcodeCreator::getSLbarcode(int *points)
{
    barcode* lines=new barcode();
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
        lines->add(t,fmin(255,256 - t));
        tempStack.pop();
    }
    std::sort(lines->bar.begin(), lines->bar.end(), compareLines);

    return lines;
}


