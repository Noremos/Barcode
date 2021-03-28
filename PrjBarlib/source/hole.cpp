#include "hole.h"
#include "barcodeCreator.h"

template<class T>
bc::Hole<T>::Hole(point p1, BarcodeCreator<T> *factory) : Component<T>(factory)
{
    isValid = false;
    add(p1);
}

template<class T>
bc::Hole<T>::Hole(point p1, point p2, point p3, BarcodeCreator<T> *factory) : Component<T>(factory)
{
    isValid = true;
//    zeorStart = p1;
    ++factory->lastB;

    add(p1);
    add(p2);
    add(p3);
}

template<class T>
bc::Hole<T>::~Hole()
{
}

template<class T>
bool bc::Hole<T>::getIsOutside() const
{
    return isOutside;
}

template<class T>
void bc::Hole<T>::setShadowOutside(bool outside)
{
    isOutside = outside;
}

template<class T>
void bc::Hole<T>::setOutside()
{
    if (!isOutside) {
        isOutside = true;
        if (isValid)
        {
            this->end = this->factory->curbright;
            --this->factory->lastB;
        }
    }
}

template<class T>
void bc::Hole<T>::kill()
{
    if(!isOutside && isValid)
    {
        this->end = this->factory->curbright;
        --this->factory->lastB;
    }
}

template<class T>
bool bc::Hole<T>::tryAdd(const point &p)
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
        if (this->isContain((point)p + poss[i]) &&
                (this->isContain((point)p + poss[i + 1]) || (i % 2 == 0 && this->isContain((point)p + poss[i + 2])))
           )//есть ли нужное ребро
        {
            this->add(p);
            return true;
        }
    }
    return false;
}

template<class T>
inline void bc::Hole<T>::add(const point& p)
{
    bool outDo = isOutside;
    bc::Component<T>::add(p);
//    setB(p);

    if (!isOutside)//ребро должно быть на границе
    {
        if ((p.x == 0 || p.x == this->factory->wid-1) &&
                (this->isContain(p.x, p.y - 1) || this->isContain(p.x, p.y + 1)))
        {
            isOutside = true;
        }
        else if ((p.y == 0 || p.y == this->factory->hei-1) &&
                 (this->isContain(p.x - 1, p.y) || this->isContain(p.x + 1, p.y)))
        {
            isOutside = true;
        }

        if (isOutside != outDo)
        {
            --this->factory->lastB;
            this->end = this->factory->curbright;
        }
    }
//    ++size;
}

template<class T>
bool bc::Hole<T>::checkValid(point p)
{
    if (this->totalCount < 3)
        return false;

    if (isValid)
        return true;

    static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
    //эти сочетания могу образовывать дубли, поэтому перед добавление СЛЕДУЕТ ПРОВЕРЯТЬ, был ли уже добавлен такой треугольник

    //точка p в цетре. Сканирем ребра вокруг точки
    for (size_t i = 0; i < 8; ++i)
    {
        //Она соединяется только с соседними ближйми ребрами
        if (this->isContain(p + poss[i]) && (this->isContain(p + poss[i + 1]) || this->isContain(p + poss[i + 1])))//есть ли нужное ребро
        {
            isValid = true;
            return true;
        }
    }
    return false;
}


//явяется ли точка точкой соединения двух дыр - рис1
template<class T>
bool bc::Hole<T>::findCross(point p, bc::Hole<T>* hole)
{
    static char poss[5][2] = { { -1,0 },{ 0,-1 },{ 1,0 },{ 0,1 },{ -1,0 } };
    static char poss2[5][2] = { { -1,-1 },{ 1,-1 },{ 1,1 },{ -1,1 } };
    //static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 }};
    if (this->isContain(p) && hole->isContain(p))
    {
        //ВАЖНО! Две дыры можно соединить если у ниъ есть 2 общие точки. Если мы знаем одну, то вторая должна быть четко над/под/слева/справа от предыдужей.
        //При дургом расопложении дыры не соединятся
        for (size_t i = 0; i < 4; ++i)
        {
            if (this->isContain(p + poss[i]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
                return true;

            //************cdoc 15**************
            if (this->isContain(p + poss[i]) && hole->isContain(p + poss[i + 1]))//если есть ребро, оразующиееся из разных дыр
                return true;
            if (this->isContain(p + poss[i + 1]) && hole->isContain(p + poss[i]))//если есть ребро, оразующиееся из разных дыр
                return true;
            //*********************************
        }
        //**********CDOC 590**************
        for (size_t i = 0; i < 4; ++i)
        {
            if (this->isContain(p + poss2[i]) && hole->isContain(p + poss2[i]))//если есть ребро, оразующиееся из разных дыр
                return true;
        }
        //********************************
    }
    return false;
}

INIT_TEMPLATE_TYPE(bc::Hole)