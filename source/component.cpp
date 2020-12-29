#include "point.h"

#include "barcodeCreator.h"
#include "component.h"

bc::Component::Component(point pix, bc::barcodeCreator *factory)
{
    coords = new pmap();
    this->factory = factory;

    factory->components.push_back(this);
    num = factory->components.size();
    start = factory->curbright;
    end = 0;
    factory->lastB++;

	add(pix);
}

bc::Component::Component(bc::barcodeCreator *factory, bool create)
{
	coords = new pmap();
    this->factory = factory;

    factory->components.push_back(this);
    num = factory->components.size();
    start = factory->curbright;
	end = 0;
	if (create)
		factory->lastB++;
//    binmap.create(factory->hei, factory->wid, CV_16SC1);
//    binmap.setTo(-1);
}

bool bc::Component::isContain(int x, int y)
{
    return factory->getComp(x,y) == this;
}

bool bc::Component::isContain(point p)
{
    return factory->getComp(p) == this;
}

inline void bc::Component::add(const point &p)
{
	if (lived)
	{
		if (factory->curbright > end)
			end = factory->curbright;
		if (factory->curbright < start)
			start = factory->curbright;
	}
	++totalCount;
	if (this->parent)
	{
		++this->parent->getMaxParrent()->totalCount;
	}
    factory->setInclude(p, this);
    coords->push_back(ppair(p, factory->curbright));
}

void bc::Component::kill()
{
    lived = false;
	if (end < factory->curbright)
		end = factory->curbright;
    --factory->lastB;
	coords->reserve(coords->size());
    if (factory->createBin)
    {
        for (auto p = coords->begin(); p != coords->end(); ++p)
			p->second = end - p->second;
	}
}

void bc::Component::setParrent(bc::Component *parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;
	this->parent->totalCount += totalCount;

}

bc::Component::~Component()
{
	factory->components[num - 1] = nullptr;
	if (!factory->createBin && !factory->getCoords)
    {
        coords->clear();
        delete coords;
    }
}
