#pragma once
#include "component.h"
namespace bc
{

class EXPORT Hole : public Component
{
    bool isOutside = false;
public:

    bool isValid = false;

    Hole(point p1, point p2, point p3, barcodeCreator* factory);
    Hole(point p1, barcodeCreator* factory);
    //******************STATIC SIDE**********************



    //***************************************************
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
    bool findCross(point p, Hole* hole);
    ~Hole();
    bool getIsOutside() const;
    void setShadowOutside(bool outside);
    void setOutside();
    void kill();

};
}
