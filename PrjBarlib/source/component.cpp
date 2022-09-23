#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>


void bc::Component::init(BarcodeCreator* factory)
{
#ifndef POINTS_ARE_AVAILABLE
	startIndex = factory->curIndexInSortedArr;
#endif // !POINTS_ARE_AVAILABLE

	this->factory = factory;
	//	index = factory->components.size();
	factory->components.push_back(this);

	resline = new barline(factory->workingImg->wid());
	resline->start = factory->curbright;
	resline->m_end = factory->curbright;

	lastVal = factory->curbright;

	if (factory->settings.returnType == bc::ReturnType::barcode3d ||
		factory->settings.returnType == bc::ReturnType::barcode3dold)
		resline->bar3d = new barcounter();
}


bc::Component::Component(poidex pix, bc::BarcodeCreator* factory)
{
	init(factory);

	// factory->lastB++;

	add(pix, factory->getPoint(pix));
}


bc::Component::Component(bc::BarcodeCreator* factory, bool /*create*/)
{
	init(factory);

	// if (create)	factory->lastB++;
}

Barscalar bc::Component::getStart()
{
	assert(resline != NULL);
	return resline->start;
}

bool bc::Component::justCreated()
{
	return resline->start == resline->m_end && resline->start == factory->curbright;
}


bool bc::Component::isContain(poidex index)
{
	return factory->getComp(index) == this;
}


bool bc::Component::add(const poidex index, const point p, bool forsed)
{
	assert(lived);

	if (!forsed)
	{
		if (!canBeConnected(p, true))
			return false;

		if (cashedSize == factory->settings.colorRange)
		{
			return false;
		}
	}

#ifndef POINTS_ARE_AVAILABLE
	assert(getMaxparent() == this);
	++getMaxparent()->totalCount;
#endif // !POINTS_ARE_AVAILABLE


	factory->setInclude(index, this);

	if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(p, factory->curbright);
		avgSr += factory->curbright;
	}
	// 3d barcode/ —читаем кол-во добавленных значений
	if (factory->settings.returnType == ReturnType::barcode3d)
	{
		if (factory->curbright != lastVal)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); //всего
		}
	}
	else if (factory->curbright != lastVal)
	{
		if (factory->settings.returnType == ReturnType::barcode3dold)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); // сколкьо было доабвлено
		}
		cashedSize = 0;
	}
	++cashedSize;
	lastVal = factory->curbright;

	return true;
}

void bc::Component::kill()
{
	if (!lived)
		return;
	lived = false;

	if (factory->curbright < resline->start)
		resline->start = factory->curbright;
	if (factory->curbright > resline->start)
		resline->m_end = factory->curbright;

	if (factory->settings.returnType == ReturnType::barcode3dold)
	{
		resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize));
	}
	else if (factory->settings.returnType == ReturnType::barcode3d)
	{
		resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize));
	}

	if (parent == nullptr && factory->settings.createBinaryMasks)
	{
		for (barvalue& a : resline->matr)
		{
			a.value = resline->m_end - a.value;
		}
	}

	lastVal = factory->curbright;
	cashedSize = 0;
}


void bc::Component::setParent(bc::Component* parnt)
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

		for (barvalue& val : resline->matr)
		{
			/*	if (factory->settings.returnType == ReturnType::barcode3dold)
				{
					if (val.value != parnt->lastVal)
					{
						parnt->resline->bar3d->push_back(bar3dvalue(parnt->lastVal, parnt->lastVal->cashedSize));
						parnt->lastVal = factory->curbright;
						parnt->lastVal->cashedSize = 0;
					}
					++parnt->lastVal->cashedSize;
				}
				else if (factory->settings.returnType == ReturnType::barcode3d)
				{
					if (val.value != parnt->lastVal)
					{
						parnt->resline->bar3d->push_back(bar3dvalue(parnt->lastVal, parnt->totalCount));
						parnt->lastVal = val.value;
					}
				}*/

			// Записываем длину сущщетвования точки
			val.value = factory->curbright > val.value ? factory->curbright - val.value : val.value - factory->curbright;
			//val.value = factory->curbright - val.value;

			avgSr += val.value;
			// Эти точки сичтаются как только что присоединившиеся
			parnt->resline->addCoord(barvalue(val.getPoint(), factory->curbright));
		}
	}

	kill();

	if (factory->settings.createGraph)
		resline->setparent(parnt->resline);
}


 bool bc::Component::canBeConnected(const bc::point& p, bool incrSum)
{

	if (factory->settings.maxRadius < (lastVal.val_distance(factory->curbright)))
		return false;

//	if (!factory->settings.maxLen.isCached)
//		return true;
	if (totalCount == 0)
		return true;

	return true;
	//return ((avgSr.getAvgFloat() / totalCount) * 1.2f <= factory->curbright.getAvgFloat());


//	Barscalar val = factory->workingImg->get(p.x, p.y);
//	Barscalar diff;
//	if (val > resline->start)
//	{
//		diff = val - resline->start;
//	}
//	else
//	{
//		diff = resline->start - val;
//	}
//	return diff <= factory->settings.maxLen.val;

	//Component* comp = getMaxparent();
	//if ((float)comp->totalCount / factory->workingImg->length() >= .1f)
	//{
	//	float st = (float)comp->getStart();
	//	//float avg = ((float)comp->sums + val) / (comp->totalCount + 1);
	//	float avg = ((float)lastVal - st) / 2;
	//	float dff = abs((float)st - avg);
	//	if (abs(val - avg) > dff)
	//	{
	//		return false;
	//	}
	//}
	//if (incrSum)
	//	comp->sums += val;

	return true;
}

bc::Component::~Component()
{
	//	factory->components[index] = nullptr;
}

