//#include "point.h"

#include "barcodeCreator.h"
#include "component.h"

bc::Component::Component(int x, int y, bc::barcodeCreator* factory)
{
    coords = new pmap();
    this->factory = factory;

    factory->components.push_back(this);
    num = factory->components.size();
    start = factory->curbright;
    end = 255;

    //    binmap.create(factory->hei, factory->wid, CV_16SC1);
    //    binmap.setTo(-1);
    add(point(x, y));
}

bc::Component::Component(bc::barcodeCreator* factory)
{
    this->factory = factory;

    factory->components.push_back(this);
    num = factory->components.size();
    start = factory->curbright;
    end = 255;

    //    binmap.create(factory->hei, factory->wid, CV_16SC1);
    //    binmap.setTo(-1);
    coords = new pmap();
}

bool bc::Component::isContain(int x, int y)
{
    auto  it = coords->find(point(x, y));
    return (it != coords->end());
}

bool bc::Component::isContain(point p)
{
    auto  it = coords->find(p);
    return (it != coords->end());
}

inline void bc::Component::add(const point& p)
{
    factory->included[factory->GETPOFF(p)] = this;
    coords->insert(ppair(p, factory->curbright));
    //    setB(p);
}

void bc::Component::kill()
{
    end = factory->curbright;
}

//void bc::Component::setB(const point &p)
//{
//    binmap.at<short>(p.y, p.x) = (short)(factory->curbright);
//}

bc::Component::~Component()
{
    factory->components[num - 1] = nullptr;
    if (!factory->createBin)
    {
        coords->clear();
        delete coords;
    }
}
