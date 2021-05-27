#pragma once
#include "component.h"
namespace bc
{

template<class T>
class Hole : public Component<T>
{
    bool isOutside = false;
public:
    T start, end;
    bool isValid = false;

    Hole(point p1, point p2, point p3, BarcodeCreator<T>* factory);
    Hole(point p1, BarcodeCreator<T>* factory);
    ~Hole();

//        void setOutside(bool b) {
//            isOutside = b;

//        }
//        inline bool getOutside() { return isOutside;}
//    void addAround();
    bool tryAdd(const point &p);
    //add withot cheks. NOT SAFE. not add if valid
    void add(const point &p);
    //************************************************************************//
    bool checkValid(point p);
    //добовляет точку в дыру, если она там не содержится. Возвращает

    //явяется ли точка точкой соединения двух дыр
    bool findCross(point p, Hole<T>* hole);
    bool getIsOutside() const;
    void setShadowOutside(bool outside);
    void setOutside();
    void kill();
};

}
