#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>

template<class T>
void bc::Component<T>::init(BarcodeCreator<T>* factory)
{
#ifndef POINTS_ARE_AVAILABLE
	startIndex = factory->curindex;
#endif // !POINTS_ARE_AVAILABLE

	this->factory = factory;
	//	index = factory->components.size();
	factory->components.push_back(this);

	resline = new barline<T>(factory->workingImg->wid());
	resline->start = factory->curbright;
	resline->len = 0;

	lastVal = factory->curbright;

	if (factory->settings.returnType == bc::ReturnType::barcode3d)
		resline->bar3d = new barcounter<T>();
}

template<class T>
bc::Component<T>::Component(poidex pix, bc::BarcodeCreator<T>* factory)
{
	init(factory);

	// factory->lastB++;

	add(pix);
}

template<class T>
bc::Component<T>::Component(bc::BarcodeCreator<T>* factory, bool /*create*/)
{
	init(factory);

	// if (create)	factory->lastB++;
}

template<class T>
bool bc::Component<T>::isContain(poidex index)
{
	return factory->getComp(index) == this;
}

template<class T>
void bc::Component<T>::add(poidex index)
{
	assert(lived);

#ifndef POINTS_ARE_AVAILABLE
	++getMaxparent()->totalCount;
#endif // !POINTS_ARE_AVAILABLE

	factory->setInclude(index, this);

	if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(index, factory->curbright);
	}
	// 3d barcode/ —читаем кол-во добавленных значений
	if (factory->settings.returnType == ReturnType::barcode3d)
	{
		if (factory->curbright != lastVal)
		{
			resline->bar3d->push_back(bar3dvalue<T>(lastVal, cashedSize));
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
	lived = false;

	resline->len = factory->curbright - resline->start;

	if (factory->settings.returnType == ReturnType::barcode3d && factory->curbright != lastVal)
	{
		resline->bar3d->push_back(bar3dvalue<T>(lastVal, cashedSize));
	}

	if (parent == nullptr && factory->settings.createBinaryMasks)
	{
		for (barvalue<T>& a : resline->matr)
		{
			a.value = factory->curbright - a.value;
		}
	}

	lastVal = factory->curbright;
	cashedSize = 0;
}

template<class T>
void bc::Component<T>::setparent(bc::Component<T>* parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;

#ifndef  POINTS_ARE_AVAILABLE
	this->parent->totalCount += totalCount;
#endif // ! POINTS_ARE_AVAILABLE


	// at moment when this must be dead
	assert(lived);

	if (factory->settings.createBinaryMasks)
	{
		parnt->resline->matr.reserve(parnt->resline->matr.size() + resline->matr.size() + 1);
		for (barvalue<T>& val : resline->matr)
		{
			// Записываем длину сущщетвования точки
			val.value = factory->curbright - val.value;

			// Эти точки сичтаются как только что присоединившиеся
			parnt->resline->addCoord(barvalue<T>(val.getIndex(), factory->curbright));
		}
	}

	kill();

	if (factory->settings.createGraph)
		resline->setparent(parnt->resline);
}

template<class T>
bc::Component<T>::~Component()
{
	//	factory->components[index] = nullptr;

}


INIT_TEMPLATE_TYPE(bc::Component)
