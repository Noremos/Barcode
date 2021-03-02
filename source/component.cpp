#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>

template<class T>
void bc::Component<T>::init(BarcodeCreator<T>* factory)
{
	this->factory = factory;
	factory->components.push_back(this);
	num = factory->components.size();
	start = factory->curbright;
	end = factory->curbright;
	bar3d = new barcounter();
}

template<class T>
bc::Component<T>::Component(point pix, bc::BarcodeCreator<T>* factory)
{
	init(factory);

	factory->lastB++;
	add(pix);
}

template<class T>
bc::Component<T>::Component(bc::BarcodeCreator<T>* factory, bool create)
{
	init(factory);

	if (create)
		factory->lastB++;
}

template<class T>
bool bc::Component<T>::isContain(int x, int y)
{
	return factory->getComp(x, y) == this;
}

template<class T>
bool bc::Component<T>::isContain(point p)
{
	return factory->getComp(p) == this;
}

template<class T>
void bc::Component<T>::add(const point& p)
{
	if (lived)
	{
		if (factory->curbright > end)
			end = factory->curbright;
		if (factory->curbright < start)
			start = factory->curbright;
	}
	if (this->parent)
	{
		++this->parent->getMaxParrent()->totalCount;
	}else
		++totalCount;

	factory->setInclude(p, this);
	//coords->push_back(ppair<T>(p, factory->curbright));

	// 3d barcode/ —читаем кол-во добавленных значений
	++cashedSize;
	if (factory->curbright != lastVal)
	{
		bar3d->push_back(cashedSize);
		lastVal = factory->curbright;
		cashedSize = 0;
	}
}

template<class T>
void bc::Component<T>::kill()
{
	lived = false;
	if (end < factory->curbright)
		end = factory->curbright;
	--factory->lastB;
	//coords->reserve(coords->size());
	//if (factory->createBin)
	 //{
		//for (auto p = coords->begin(); p != coords->end(); ++p)
		//		p->second = end - p->second;
	//}

	bar3d->push_back(cashedSize);
	lastVal = factory->curbright;
	cashedSize = 0;
}

template<class T>
void bc::Component<T>::setParrent(bc::Component<T>* parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;
	this->parent->totalCount += totalCount;
}

template<class T>
bc::Component<T>::~Component()
{
	factory->components[num - 1] = nullptr;
}


INIT_TEMPLATE_TYPE(bc::Component)
