#include "hole.h"
#include "barcodeCreator.h"

bc::Hole::Hole(point p1, barcodeCreator *factory) :Component(factory)
{
    isValid = false;
    add(p1);
}


bc::Hole::Hole(point p1, point p2, point p3, barcodeCreator *factory) : Component(factory)
{
    isValid = true;
//    zeorStart = p1;
    ++factory->lastB;

    add(p1);
    add(p2);
    add(p3);
}


bc::Hole::~Hole()
{
}

bool bc::Hole::getIsOutside() const
{
    return isOutside;
}

void bc::Hole::setShadowOutside(bool outside)
{
    isOutside = outside;
}

void bc::Hole::setOutside()
{
    if (!isOutside) {
        isOutside = true;
        if (isValid)
        {
            end = factory->curbright;
            --factory->lastB;
        }
    }
}

void bc::Hole::kill()
{
    if(!isOutside && isValid)
    {
        end = factory->curbright;
        --factory->lastB;
    }
}

bool bc::Hole::tryAdd(const point &p)
{
    if (isValid == false)
    {
        /*addCoord(p);
        return true;*/
        return false;
    }
    static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
    for (size_t i = 0; i < 8; ++i)
    {
        //Она соединяется только с соседними ближйми ребрами
        if (isContain((point)p + poss[i]) &&
                (isContain((point)p + poss[i + 1]) || (i % 2 == 0 && isContain((point)p + poss[i + 2])))
           )//есть ли нужное ребро
        {
            this->add(p);
            return true;
        }
    }
    return false;
}


inline void bc::Hole::add(const point& p)
{
    bool outDo = isOutside;
    coords->push_back(ppair(p,factory->curbright));
    factory->setInclude(p, this);
//    setB(p);

    if (!isOutside)//ребро должно быть на границе
    {
        if ((p.x == 0 || p.x ==factory->wid-1) &&
                (isContain(p.x, p.y - 1) || isContain(p.x, p.y + 1)))
        {
            isOutside = true;
        }
        else if ((p.y == 0 || p.y == factory->hei-1) &&
                 (isContain(p.x - 1, p.y) || isContain(p.x + 1, p.y)))
        {
            isOutside = true;
        }

        if (isOutside != outDo)
        {
            --factory->lastB;
            end = factory->curbright;
        }
    }
//    ++size;
}

bool bc::Hole::checkValid(point p)
{
    if (coords->size() < 3)
        return false;

    if (isValid)
        return true;

    static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
    //эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник

    //точка p в цетре. Сканирем ребра вокруг точки
    for (size_t i = 0; i < 8; ++i)
    {
        //Она соединяется только с соседними ближйми ребрами
        if (isContain(p + poss[i]) && (isContain(p + poss[i + 1]) || isContain(p + poss[i + 1])))//есть ли нужное ребро
        {
            isValid = true;
            return true;
        }
    }
    return false;
}


//явяется ли точка точкой соединения двух дыр - рис1
bool bc::Hole::findCross(point p, bc::Hole* hole)
{
    static char poss[5][2] = { { -1,0 },{ 0,-1 },{ 1,0 },{ 0,1 },{ -1,0 } };
    static char poss2[5][2] = { { -1,-1 },{ 1,-1 },{ 1,1 },{ -1,1 } };
    //static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 }};
    if (isContain(p) && hole->isContain(p))
    {
        //ВАЖНО! Две дыры можно соединить если у ниъ есть 2 общие точки. Если мы знаем одну, то вторая должна быть четко над/под/слева/справа от предыдужей.
        //При дургом расопложении дыры не соединятся
        for (size_t i = 0; i < 4; ++i)
        {
            if (isContain(p + poss[i]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
                return true;

            //************cdoc 15**************
            if (isContain(p + poss[i]) && hole->isContain(p + poss[i + 1]))//если есть ребро, оразующиееся из разных дыр
                return true;
            if (isContain(p + poss[i + 1]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
                return true;
            //*********************************
        }
        //**********CDOC 590**************
        for (size_t i = 0; i < 4; ++i)
        {
            if (isContain(p + poss2[i]) && hole->isContain(p + poss2[i]))//если есть ребро, оразующиееся из разных дыр
                return true;
        }
        //********************************
    }
    return false;
}
