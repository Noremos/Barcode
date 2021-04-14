#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>

template<class T>
void bc::Component<T>::init(BarcodeCreator<T>* factory)
{
	this->factory = factory;
	factory->components.push_back(this);
	start = factory->curbright;
	end = factory->curbright;
	lastVal = factory->curbright;

	if (factory->settings.returnType == bc::ReturnType::barcode3d)
		bar3d = new barcounter<T>();
}

template<class T>
bc::Component<T>::Component(point pix, bc::BarcodeCreator<T>* factory)
{
	init(factory);

	// factory->lastB++;

	//add
	++totalCount;
	factory->setInclude(pix, this);
	++cashedSize;
}

template<class T>
bc::Component<T>::Component(bc::BarcodeCreator<T>* factory, bool /*create*/)
{
	init(factory);

	// if (create)	factory->lastB++;
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
//	if (lived)
//	{
//		if (factory->curbright > end)
//			end = factory->curbright;
//		if (factory->curbright < start)
//			start = factory->curbright;
//	}
	assert(lived);

	++getMaxParrent()->totalCount;

	factory->setInclude(p, this);

	// 3d barcode/ —читаем кол-во добавленных значений
	if (bar3d != nullptr)
	{
		if (factory->curbright != lastVal)
		{
			bar3d->push_back(bar3dvalue<T>(lastVal,cashedSize));
			lastVal = factory->curbright;
			cashedSize = 0;
		}
		++cashedSize;
	}
}

template<class T>
void bc::Component<T>::kill()
{
	if (!lived)
		return;
	// --factory->lastB;
	lived = false;
//	if (end < factory->curbright)
		end = factory->curbright;

//	T maxLen = factory->settings.maxLen.getOrDefault(0);
//	if (end - start > maxLen)
//	{
//		end = start + maxLen;
//	}
	//coords->reserve(coords->size());
	//if (factory->createBin)
	 //{
		//for (auto p = coords->begin(); p != coords->end(); ++p)
		//		p->second = end - p->second;
	//}

	if (bar3d != nullptr)
		bar3d->push_back(bar3dvalue<T>(lastVal, cashedSize));

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
}


INIT_TEMPLATE_TYPE(bc::Component)
