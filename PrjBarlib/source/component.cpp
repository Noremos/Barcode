#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>

template <class T>
void bc::Component<T>::init(BarcodeCreator<T>* factory)
{
#ifndef POINTS_ARE_AVAILABLE
	startIndex = factory->curIndexInSortedArr;
#endif // !POINTS_ARE_AVAILABLE

	this->factory = factory;
	//	index = factory->components.size();
	factory->components.push_back(this);

	resline = new barline<T>(factory->workingImg->wid());
	resline->start = factory->curbright;
	resline->m_end = factory->curbright;

	lastVal = factory->curbright;

	if (factory->settings.returnType == bc::ReturnType::barcode3d ||
		factory->settings.returnType == bc::ReturnType::barcode3dold)
		resline->bar3d = new barcounter<T>();
}

template <class T>
bc::Component<T>::Component(poidex pix, bc::BarcodeCreator<T>* factory)
{
	init(factory);

	// factory->lastB++;

	add(pix);
}

template <class T>
bc::Component<T>::Component(bc::BarcodeCreator<T>* factory, bool /*create*/)
{
	init(factory);

	// if (create)	factory->lastB++;
}

template <class T>
bool bc::Component<T>::isContain(poidex index)
{
	return factory->getComp(index) == this;
}

template <class T>
bool bc::Component<T>::add(poidex index, const point p)
{
	assert(lived);

	float val = (float)factory->workingImg->get(p.x, p.y);
	Component<T>* comp = getMaxparent();
	if ((float)comp->totalCount / factory->workingImg->length() >= .1f)
	{
		float st = (float)comp->getStart();
		float avg = ((float)comp->sums + val)/ (comp->totalCount + 1);
		float dff = abs((float)st - avg);
		if (abs(val - avg) > dff)
		{
			return false;
		}
	}
	comp->sums += val;

#ifndef POINTS_ARE_AVAILABLE
	assert(getMaxparent() == this);
	++getMaxparent()->totalCount;
#endif // !POINTS_ARE_AVAILABLE


	factory->setInclude(index, this);

	if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(p, factory->curbright);
	}
	// 3d barcode/ —читаем кол-во добавленных значений
	if (factory->settings.returnType == ReturnType::barcode3d)
	{
		if (factory->curbright != lastVal)
		{
			resline->bar3d->push_back(bar3dvalue<T>(lastVal, cashedSize)); //всего
		}
		++cashedSize;
	}
	else if (factory->curbright != lastVal)
	{
		if (factory->settings.returnType == ReturnType::barcode3dold)
		{
			resline->bar3d->push_back(bar3dvalue<T>(lastVal, cashedSize)); // сколкьо было доабвлено
			cashedSize = 0;
		}
		++cashedSize;
	}
	lastVal = factory->curbright;

	return true;
}

template <class T>
bool bc::Component<T>::add(poidex index)
{
	return this->add(index, factory->curpix);
}

template <class T>
void bc::Component<T>::kill()
{
	if (!lived)
		return;
	lived = false;

	resline->m_end = factory->curbright;

	if (factory->settings.returnType == ReturnType::barcode3dold)
	{
		resline->bar3d->push_back(bar3dvalue<T>(lastVal, cashedSize));
	}
	else if (factory->settings.returnType == ReturnType::barcode3d)
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

template <class T>
void bc::Component<T>::setParent(bc::Component<T>* parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;

#ifndef POINTS_ARE_AVAILABLE
	this->parent->totalCount += totalCount;
	parnt->startIndex = MIN(parnt->startIndex, startIndex);
	parnt->sums += this->sums;
#endif // ! POINTS_ARE_AVAILABLE

	// at moment when this must be dead
	assert(lived);

	if (factory->settings.createBinaryMasks)
	{
		parnt->resline->matr.reserve(parnt->resline->matr.size() + resline->matr.size() + 1);
		for (barvalue<T>& val : resline->matr)
		{
			/*	if (factory->settings.returnType == ReturnType::barcode3dold)
				{
					if (val.value != parnt->lastVal)
					{
						parnt->resline->bar3d->push_back(bar3dvalue<T>(parnt->lastVal, parnt->lastVal->cashedSize));
						parnt->lastVal = factory->curbright;
						parnt->lastVal->cashedSize = 0;
					}
					++parnt->lastVal->cashedSize;
				}
				else if (factory->settings.returnType == ReturnType::barcode3d)
				{
					if (val.value != parnt->lastVal)
					{
						parnt->resline->bar3d->push_back(bar3dvalue<T>(parnt->lastVal, parnt->totalCount));
						parnt->lastVal = val.value;
					}
				}*/

				// Записываем длину сущщетвования точки
			val.value = factory->curbright - val.value;

			// Эти точки сичтаются как только что присоединившиеся
			parnt->resline->addCoord(barvalue<T>(val.getPoint(), factory->curbright));
		}
	}

	kill();

	if (factory->settings.createGraph)
		resline->setparent(parnt->resline);
}

template <class T>
bc::Component<T>::~Component()
{
	//	factory->components[index] = nullptr;
}

INIT_TEMPLATE_TYPE(bc::Component)
