#include "component.h"

#include "barcodeCreator.h"
#include <assert.h>


void bc::Component::init(BarcodeCreator* factory, const Barscalar& val)
{
#ifndef POINTS_ARE_AVAILABLE
	startIndex = factory->curIndexInSortedArr;
#endif // !POINTS_ARE_AVAILABLE

	this->factory = factory;
	//	index = factory->components.size();
	factory->components.push_back(this);

	resline = new barline();
	resline->start = val;
	resline->m_end = val;
	lastVal = val;

	if (factory->settings.returnType == bc::ReturnType::barcode3d ||
		factory->settings.returnType == bc::ReturnType::barcode3dold)
		resline->bar3d = new barcounter();
}


bc::Component::Component(poidex pix, const Barscalar& val, bc::BarcodeCreator* factory)
{
	init(factory, val);

	// factory->lastB++;

	add(pix, factory->getPoint(pix), val);
#ifdef ENABLE_ENERGY
	maxe = factory->settings.energyStart;
	energy[pix] = maxe;
#endif
}


bc::Component::Component(bc::BarcodeCreator* factory, bool /*create*/)
{
	init(factory, factory->curbright);
}

Barscalar bc::Component::getStart()
{
	assert(resline != NULL);
	return resline->start;
}

bool bc::Component::justCreated()
{
	return lastVal == factory->curbright && same;
}


bool bc::Component::isContain(poidex index)
{
	return factory->getComp(index) == this;
}


bool bc::Component::add(const poidex index, const point p, const Barscalar& col, bool forsed)
{
	assert(lived);

	if (!forsed)
	{
		//if (!canBeConnected(p, true))
		//	return false;

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

#ifdef ENABLE_ENERGY
	int dds = 0;
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 } };
	for (size_t i = 0; i < 8; i++)
	{
		point po = p + poss[i];
		if (po.x < 0 || po.y < 0)
			break;

		poidex d = po.getLiner(factory->wid);
		auto t = energy.find(d);
		if (t != energy.end())
		{
			if (t->second > 0)
			{
				auto half = t->second / 2;
				t->second -= half;
				dds += half;
				if (dds > maxe)
					maxe = dds;
			}
		}
	}
	energy.insert(std::pair(index, dds));
#endif

	if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(p, col);
	}
	bool eq = col == lastVal;
	same = same && eq;
	// 3d barcode/ —читаем кол-во добавленных значений
	if (!eq)
	{
		if (factory->settings.returnType == ReturnType::barcode3d)
		{
			resline->bar3d->push_back(bar3dvalue(static_cast<float>(x - xMin) / (cashedSize * (1 + xMax - xMin)), static_cast<float>(y - yMin) / (cashedSize * (1 + yMax - yMin)), static_cast<float>(cashedSize) / factory->workingImg->length())); //всего
		}
		else if (factory->settings.returnType == ReturnType::barcode3dold)
		{
			resline->bar3d->push_back(bar3dvalue(lastVal, cashedSize)); // сколкьо было доабвлено
		}
		cashedSize = 0;
	}
	else
	{
		x += p.x;
		y += p.y;
		if (x > xMax)
			xMax = x;
		if (x < xMin)
			xMin = x;

		if (y > yMax)
			yMax = y;
		if (y < yMin)
			yMin = y;
	}

	++cashedSize;
	lastVal = col;

	return true;
}

void bc::Component::kill()
{
	kill(lastVal);
}

void bc::Component::kill(const Barscalar& endScalar)
{
	if (!lived)
		return;
	lived = false;

	resline->m_end = endScalar;

	//if (col < resline->start)
	//	resline->start = col;
	//if (col > resline->m_end)
	//	resline->m_end = col;

//	assert(resline->len() != 0);

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
		// Not working for radius
		//Barscalar bot = resline->start;
		//Barscalar top = resline->m_end;

		//if (bot > top)
		//{
		//	bot = resline->m_end;
		//	top = resline->start;
		//}

		for (barvalue& a : resline->matr)
		{
			//assert(bot <= a.value);
			//assert(a.value <= top);
#ifdef ENABLE_ENERGY
			const uint id = barvalue::getStatInd(a.getX(), a.getY(), factory->wid);
			a.value = Barscalar(static_cast<float>(energy[id]) / maxe, BarType::FLOAT32_1);
#else
			a.value = resline->m_end.absDiff(a.value);
#endif
		}
	}

	lastVal = endScalar;
	cashedSize = 0;
}


void bc::Component::setParent(bc::Component* parnt)
{
	assert(parent == nullptr);
	this->parent = parnt;

#ifndef POINTS_ARE_AVAILABLE
	this->parent->totalCount += totalCount;
	parnt->startIndex = MIN(parnt->startIndex, startIndex);
	//parnt->sums += this->sums;
#endif // ! POINTS_ARE_AVAILABLE

	// at moment when this must be dead
	assert(lived);

	// Мы объединяем, потому что одинаковый добавился (но для оптимизации не добавлятся в конце)
	const Barscalar& endScalar = factory->curbright;
	if (factory->settings.createBinaryMasks && resline->matr.size() > 0)
	{
		parnt->resline->matr.reserve(parnt->resline->matr.size() + resline->matr.size() + 1);

		for (barvalue& val : resline->matr)
		{
#ifdef ENERGY


			val.value = Barscalar(static_cast<float>(energy[val.getIndex()]) / maxe, BarType::FLOAT32_1);
			parnt->resline->addCoord(val);

			// Записываем длину сущщетвования точки
			//val.value = endScalar.absDiff(val.value);
			//val.value = col - val.value;

			//avgSr += val.value;
			// Эти точки сичтаются как только что присоединившиеся
#else
			parnt->resline->addCoord(barvalue(val.getPoint(), endScalar));
#endif // ENERGY
		}
		parnt->same = false;
		// Мы объединяем, потому что одинаковый добавился, т.е. считаем, что lasVal одинаковыйы
		//parnt->lastVal = lastVal;
	}

#ifdef ENERGY
	parent->energy.insert(energy.begin(), energy.end());
	energy.clear();

	if (maxe > parent->maxe)
		parent->maxe = maxe;
#endif // ENERGY


	kill(endScalar);

	if (factory->settings.createGraph)
		resline->setparent(parnt->resline);
}


bool bc::Component::canBeConnected(const bc::point& p, bool incrSum)
{
	return true;

	//	if (factory->settings.maxRadius < (lastVal.val_distance(factory->curbright)))
	//		return false;
	//
	////	if (!factory->settings.maxLen.isCached)
	////		return true;
	//	if (totalCount == 0)
	//		return true;

	return true;
}

bc::Component::~Component()
{
	//	factory->components[index] = nullptr;
}


void bc::Component::process(BarcodeCreator* factory)
{
	Component* first = nullptr;
	//TODO выделять паять заранее
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	Component* justCreated = nullptr;
	std::vector<Component*> attachCondidates;
	int i = 0;
	for (; i < 8; ++i)
	{
		point IcurPoint(factory->curpix + poss[i]);

		if (factory->IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
			continue;

		//if (!checkAvg(IcurPoint))
		//	continue;

		poidex IcurPindex = IcurPoint.getLiner(factory->wid);

		Component* first = factory->getPorogComp(IcurPoint, IcurPindex);
		if (first != nullptr)//существует ли ребро вокруг
		{
			if (first->justCreated())
			{
				if (justCreated)
				{
					if (justCreated != first)
						justCreated->merge(first);
				}
				else
					justCreated = first;
			}
			else
			{
				// if len more then maxlen, kill the component
				bool more = factory->settings.maxLen.isCached && factory->curbright.absDiff(first->getStart()) > factory->settings.maxLen.getOrDefault(0);
				if (more)
				{
					//qDebug() << first->num << " " << curbright << " " << settings.maxLen.getOrDefault(0);
					if (factory->settings.killOnMaxLen)
					{
						first->kill(factory->curbright); //Интересный результат
					}
					first = nullptr;
				}
				else if (attachCondidates.empty() || attachCondidates.back() != first)
				{
					// Skip some duplicates
					attachCondidates.push_back(first);
				}
			}
		}
		//else if (first)
		//	first->add(IcurPindex, IcurPoint, workingImg->get(curpix.x, curpix.y));
	}

	if (justCreated)
		attachCondidates.push_back(justCreated);

	if (attachCondidates.size() == 0)
	{
		//lastB += 1;

		new Component(factory->curpoindex, factory->curbright, factory);
		return ;
	}
	else if (attachCondidates.size() == 1)
	{
		attachCondidates[0]->add(factory->curpoindex, factory->curpix, factory->curbright);
	}
	else
	{
		Component::attach(factory->settings, factory->curpix, factory->curpoindex, factory->curbright, attachCondidates);
	}
}

void bc::Component::merge(bc::Component* dummy)
{
	assert(this != dummy);

#ifdef POINTS_ARE_AVAILABLE
	for (const auto& val : dummy->resline->matr)
	{
		assert(workingImg->get(val.getX(wid), val.getY(wid)) == curbright);
		assert(included[val.getIndex()] == dummy);

		main->add(val.getIndex());
	}
#else

	for (auto& val : dummy->resline->matr)
	{
		const bc::point p = val.getPoint();
		add(p.getLiner(factory->wid), p, val.value, true);
	}

	startIndex = MIN(dummy->startIndex, startIndex);
#endif // POINTS_ARE_AVAILABLE
	delete dummy->resline;
	dummy->resline = nullptr;
}

void bc::Component::attach(const BarConstructor& settings, bc::point p, bc::poidex index, Barscalar& curb, std::vector<bc::Component*>& attachList)
{
	switch (settings.attachMode)
	{
	case AttachMode::dontTouch:
		return;

	case AttachMode::firstEatSecond:
		std::sort(attachList.begin(), attachList.end(), [](const Component* c1, const Component* c2) {
			return c1->startIndex < c2->startIndex;
			});
		break;
	case AttachMode::secondEatFirst:
		// <  makes i as a parent of the i + 1
		// >= makes i + 1 as a parent of the i
		std::sort(attachList.begin(), attachList.end(), [](const Component* c1, const Component* c2) {
			return c1->startIndex >= c2->startIndex;
			});

		break;
	case AttachMode::morePointsEatLow:
		std::sort(attachList.begin(), attachList.end(), [](const Component* c1, const Component* c2) {
			return c1->getTotalSize() >= c2->getTotalSize();
			});

		break;

		//case AttachMode::createNew:
		//	//if ((double)MIN(main->getTotalSize(), second->getTotalSize()) / totalSize > 0.05)
		//{
		//	COMPP newOne = new Component(this, true);
		//	main->setParent(newOne);
		//	second->setParent(newOne);
		//	//main->kill();
		//	//second->kill();
		//	return newOne;
		//}
	default: throw;
	}
	// The last one is the parent
	for (size_t i = 0; i < attachList.size() - 1; i++)
	{
		auto* left = attachList[i];
		auto* right = attachList[i + 1];
		if (left == right)
			continue;

		const Barscalar fs = left->getStart();
		const Barscalar sc = right->getStart();
		const Barscalar diff = (fs > sc) ? (fs - sc) : (sc - fs);
		if (diff > settings.getMaxLen())
		{
			continue;
		}

		right->addChild(left);
	}

	attachList.back()->add(index, p, curb);
}

