#ifndef SKIP_M_INC

#include "../include/component.h"

#include "../include/barcodeCreator.h"
#include <assert.h>

#endif

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
	lastDistance = val;

	if (factory->settings.returnType == bc::ReturnType::barcode3d ||
		factory->settings.returnType == bc::ReturnType::barcode3dold)
		resline->bar3d = new barcounter();
}


bc::Component::Component(poidex pix, const Barscalar& val, const Barscalar& distance, bc::BarcodeCreator* factory)
{
	init(factory, val);

	// factory->lastB++;

	add(pix, factory->getPoint(pix), val, distance);
}


bc::Component::Component(bc::BarcodeCreator* factory, Barscalar start)
{
	init(factory, start);
}

Barscalar bc::Component::getStart()
{
	assert(resline != NULL);
	return resline->start;
}


bool bc::Component::justCreated()
{
	// started after the same start
	return startIndex >= factory->sameStart;
}

bool bc::Component::isContain(poidex index)
{
	return factory->getComp(index) == this;
}


bool bc::Component::add(const poidex index, const point p, const Barscalar& value, const Barscalar& distance, bool forsed)
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
	// Ignore createBinaryMasks to merge points
	//if (factory->settings.createBinaryMasks)
	{
		resline->addCoord(p, value);
	}

	const bool eq = distance.getAvgFloat() == lastDistance.getAvgFloat();

	if (!eq && resline->root == nullptr)
	{
		// Not just created
		resline->initRoot(factory->root);
	}

	// 3d barcode/ —читаем кол-во добавленных значений
	if (!eq)
	{
		if (factory->settings.returnType == ReturnType::barcode3d)
		{
			resline->bar3d->push_back(bar3dvalue(static_cast<float>(x - xMin) / (cashedSize * (1 + xMax - xMin)), static_cast<float>(y - yMin) / (cashedSize * (1 + yMax - yMin)), static_cast<float>(cashedSize) / factory->workingImg->length())); //всего
		}
		else if (factory->settings.returnType == ReturnType::barcode3dold)
		{

			resline->bar3d->push_back(bar3dvalue(resline->matr.at(resline->matr.size() - 2).value, cashedSize)); // сколкьо было доабвлено
		}
		cashedSize = 0;
	}

	++cashedSize;
	lastDistance = distance;

	return true;
}

void bc::Component::kill()
{
	kill(lastDistance);
}

void bc::Component::kill(const Barscalar& endDistance)
{
	if (!lived)
		return;

	lived = false;

	resline->m_end = endDistance;
	// assert(resline->m_end.getAvgFloat() != resline->start.getAvgFloat());

	if (factory->settings.returnType == ReturnType::barcode3dold)
	{
		resline->bar3d->push_back(bar3dvalue(resline->matr.back().value, cashedSize));
	}
	else if (factory->settings.returnType == ReturnType::barcode3d)
	{
		resline->bar3d->push_back(bar3dvalue(resline->matr.back().value, cashedSize));
	}

	//if (factory->settings.createBinaryMasks)
	{
		for (barvalue& a : resline->matr)
		{
			//assert(bot <= a.value);
			//assert(a.value <= top);
			a.value = resline->m_end.absDiff(a.value);
		}
	}

	if (resline->root == nullptr)
	{
		// Not just created
		resline->initRoot(factory->root);
	}

	lastDistance = endDistance;
	cashedSize = 0;
}


void bc::Component::addChild(bc::Component* child, const Barscalar& endValue, bool allowMerge)
{
	assert(child->lived);
	assert(lived);
	assert(child->parent == nullptr);
	assert(this != child);

	if (allowMerge)
	{
		if (child->justCreated())
		{
			merge(child);
			return;
		}
		if (justCreated())
		{
			child->merge(this);
			return;
		}
	}


#ifndef POINTS_ARE_AVAILABLE
	totalCount += child->totalCount;
	//parnt->startIndex = MIN(parnt->startIndex, startIndex);
	//parnt->sums += this->sums;
#endif // ! POINTS_ARE_AVAILABLE


	// Мы объединяем, потому что одинаковый добавился (но для оптимизации не добавлятся в конце)
	if (/*factory->settings.createBinaryMasks &&*/ resline->matr.size() > 0)
	{
		// Эти точки считаются как только что присоединившиеся
		resline->matr.reserve(resline->matr.size() + child->resline->matr.size() + 1);
		for (barvalue& val : child->resline->matr)
		{
			resline->addCoord(barvalue(val.getPoint(), endValue));
		}
		// Мы объединяем, потому что одинаковый добавился, т.е. считаем, что lasVal одинаковыйы
		//parnt->lastVal = lastVal;
	}

	child->kill(endValue);
	child->parent = this; //! Should by after the kill

	if (factory->settings.createGraph)
	{
		if (resline->root == nullptr)
			resline->initRoot(factory->root);

		resline->addChild(child->resline);
	}
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

void bc::Component::passSame(BarcodeCreator* factory)
{
	//TODO выделять паять заранее
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	Component* justCreated = nullptr;
	for (uchar i = 0; i < 8; ++i)
	{
		const point IcurPoint(factory->curpix + poss[i]);

		if (factory->IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
			continue;

		const poidex IcurPindex = IcurPoint.getLiner(factory->wid);

		Component* first = factory->getComp(IcurPindex);
		if (first == nullptr)//существует ли ребро вокруг
			continue;

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
	}

	if (justCreated)
	{
		justCreated->add(factory->curpoindex, factory->curpix, factory->curbright, factory->curbright);
	}
	else
	{
		new Component(factory->curpoindex, factory->curbright, factory->curbright, factory);
	}
}


void bc::Component::passConnections(BarcodeCreator* factory)
{
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	AttachList attachCondidates;
	attachCondidates.reserve(8);
	attachCondidates.push_back({ factory->getComp(factory->curpoindex)});

	for (uchar i = 0; i < 8; ++i)
	{
		const point IcurPoint(factory->curpix + poss[i]);

		if (factory->IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
			continue;

		poidex IcurPindex = IcurPoint.getLiner(factory->wid);

		Component* first = factory->getPorogComp(IcurPoint, IcurPindex);
		if (first == nullptr)//существует ли ребро вокруг
			continue;

		// if len more then maxlen, kill the component
		const bool more = factory->curbright.absDiff(first->getStart()) > factory->settings.maxLen;
		if (more)
		{
			//qDebug() << first->num << " " << curbright << " " << settings.maxLen.getOrDefault(0);
			if (factory->settings.killOnMaxLen)
			{
				first->kill(factory->curbright); //Интересный результат
			}
		}
		else
		{
			Barscalar temp = factory->curbright.absDiff(factory->workingImg->get(IcurPoint));
			//auto asd = attachCondidates.find(first->startIndex);

			bool found = false;
			for (short i = 0; i < attachCondidates.size(); i++)
			{
				auto& inf = attachCondidates[i];
				if (inf.comp != first)
					continue;

				found = true;
				if (inf.diff > temp)
				{
					inf.diff = temp;
				}
				break;
			}

			if (!found)
			{
				attachCondidates.push_back({first, temp});
			}
		}
	}

	if (attachCondidates.size() > 1)
	{
		Component::attach(factory->settings, factory->curpix, factory->curpoindex, factory->curbright, attachCondidates);
	}
}


void bc::Component::process(BarcodeCreator* factory)
{
	//TODO выделять паять заранее
	static char poss[9][2] = { { -1,0 },{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };

	Component* justCreated = nullptr;
	AttachList attachCondidates;
	int i = 0;
	Component* minComp = nullptr;
	Barscalar minDiff;
	for (; i < 8; ++i)
	{
		point IcurPoint(factory->curpix + poss[i]);

		if (factory->IS_OUT_OF_REG(IcurPoint.x, IcurPoint.y))
			continue;

		//if (!checkAvg(IcurPoint))
		//	continue;

		poidex IcurPindex = IcurPoint.getLiner(factory->wid);

		Component* first = factory->getComp(IcurPindex);
		if (first == nullptr)//существует ли ребро вокруг
			continue;

		Barscalar temp = factory->curbright.absDiff(factory->workingImg->get(IcurPoint.x, IcurPoint.y));
		if (minComp == nullptr || temp < minDiff)
		{
			minDiff = temp;
			minComp = first;
		}

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
			bool more = factory->curbright.absDiff(first->getStart()) > factory->settings.maxLen;
			if (more)
			{
				//qDebug() << first->num << " " << curbright << " " << settings.maxLen.getOrDefault(0);
				if (factory->settings.killOnMaxLen)
				{
					first->kill(factory->curbright); //Интересный результат
				}
				first = nullptr;
			}
			else if (attachCondidates.empty() || attachCondidates.back().comp != first)
			{
				// Skip some duplicates
				attachCondidates.push_back({ first, temp });
			}
		}

		//else if (first)
		//	first->add(IcurPindex, IcurPoint, workingImg->get(curpix.x, curpix.y));
	}

	if (justCreated)
		attachCondidates.push_back({justCreated});

	if (attachCondidates.size() == 0)
	{
		//lastB += 1;

		new Component(factory->curpoindex, factory->curbright, factory->curbright, factory);
		return;
	}
	else
	{
		minComp->add(factory->curpoindex, factory->curpix, factory->curbright, factory->curbright);
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


#ifdef ENABLE_ENERGY
	int s = dummy->resline->matr.size();
	if (s > factory->maxe)
		factory->maxe = s;
#endif // ENABLE_ENERGY


	for (auto& val : dummy->resline->matr)
	{
		const bc::point p = val.getPoint();
		add(p.getLiner(factory->wid), p, val.value, lastDistance, true);

#ifdef ENABLE_ENERGY
		factory->energy[p.getLiner(factory->wid)] = s;
#endif // ENABLE_ENERGY
	}

	startIndex = MIN(dummy->startIndex, startIndex);
#endif // POINTS_ARE_AVAILABLE


	assert(dummy->resline->id == -1);
	delete dummy->resline;
	dummy->resline = nullptr;
}


void bc::Component::attach(const barstruct& settings, bc::point p, bc::poidex index, Barscalar& distance, AttachList& attachList)
{
	switch (settings.attachMode)
	{
	case AttachMode::closer:
		std::sort(attachList.begin(), attachList.end(), [](const AttachInfo& c1, const AttachInfo& c2) {
			if (c1.diff == c2.diff) // We need the same to be in a row
				return c1.comp->startIndex < c2.comp->startIndex;

			return c1.diff < c2.diff;
			});// lower is first
		break;

	case AttachMode::dontTouch:
		return;

	case AttachMode::firstEatSecond:
		std::sort(attachList.begin(), attachList.end(), [](const AttachInfo& c1, const AttachInfo& c2) {
			return c1.comp->startIndex < c2.comp->startIndex;
			});// lower is first
		break;
	case AttachMode::secondEatFirst:
		// <  makes i as a parent of the i + 1
		// >= makes i + 1 as a parent of the i
		std::sort(attachList.begin(), attachList.end(), [](const AttachInfo& c1, const AttachInfo& c2) {
			return c1.comp->startIndex > c2.comp->startIndex;
			});

		break;
	case AttachMode::morePointsEatLow:
	{

		std::sort(attachList.begin(), attachList.end(), [](const AttachInfo& c1, const AttachInfo& c2)
			{
				const size_t a = c1.comp->getTotalSize();
				const size_t b = c2.comp->getTotalSize();
				if (a == b) // We need the same to be in a row
					return c1.comp->startIndex < c2.comp->startIndex;

				return a > b;
			});// Bigger is first

		break;
	}
	case AttachMode::createNew:
	{
		COMPP newOne = new Component(attachList.front().comp->factory, distance);
		for (size_t i = 0; i < attachList.size() - 1; i++)
		{
			if (attachList[i].comp == attachList[i + 1].comp)
				continue;

			newOne->addChild(attachList[i].comp, distance, false);
		}

		newOne->addChild(attachList.back().comp, distance, false);
		return;
	}
	default: throw;
	}

	// The first one is the parent

	// keep this right beacuse a right can be merged
	auto* right = attachList.back().comp;
	for (size_t i = attachList.size() - 1; i > 0 ; --i)
	{
		auto* left = attachList[i - 1].comp;
		if (left == attachList[i].comp)
			continue;

		//const Barscalar fs = left->getStart();
		//const Barscalar sc = right->getStart();
		//const Barscalar diff = (fs > sc) ? (fs - sc) : (sc - fs);
		//if (diff > settings.getMaxLen())
		//{
		//	continue;
		//}

		left->addChild(right, distance, true);
		// left can be merged so check it
		if (left->resline)
			right = left;
	}

	//attachList.back()->add(index, p, curb);
}



//
//struct HoleFined
//{
//	std::vector<poidex>& path;
//
//};
//
//void RadiusRoot::findPath(const poidex p1, const poidex p2, bool frist, std::vector<poidex>& out)
//{
//	std::unordered_set<poidex> landscape;
//	std::unordered_set<poidex> extraTernimate;
//	const auto* curConnectons = &rebs[p1];
//
//	bool first = true;
//	int firs = first ? 0 : 8;
//	int last = first ? 8 : 0;
//	int iter = first ? 1 : -1;
//
//	for (int i = firs; i != last; i += iter)
//	{
//		poidex c = rebs[p1][i];
//		if (c != -1)
//		{
//			if (first)
//			{
//				first = false;
//			}
//			else
//			{
//				extraTernimate.insert(c);
//			}
//		}
//	}
//	extraTernimate.insert(p1);
//	poidex cur;
//
//	while (true)
//	{
//		int i;
//		for (int i = firs; i != last; i += iter)
//		{
//			cur = (*curConnectons)[i];
//			if (cur == p2)
//			{
//				return;
//			}
//			else if (cur != -1 && landscape.count(cur) == 0)
//			{
//				break;
//			}
//		}
//
//		if (i == 8)
//		{
//			if (out.size() == 0)
//				return;
//			out.pop_back();
//		}
//		else
//		{
//			if (extraTernimate.count(cur) != 0)
//			{
//				out.clear();
//				return;
//			}
//
//			landscape.insert(cur);
//
//			curConnectons = &rebs[cur];
//		}
//	}
//}
//
//
//
//void RadiusRoot::addConnection(const poidex p1, const poidex p2)
//{
//
//	auto& r1 = rebs[p1];
//	auto& r2 = rebs[p2];
//
//	for (short i = 0; i < 2; i++)
//	{
//		auto* phole = r1.holes[i];
//		if (!phole && phole->isAlive())
//			continue;
//
//		for (short j = 0; j < 2; j++)
//		{
//			if (phole == r2.holes[j])
//			{
//				phole->addConnection();
//				return;
//			}
//
//		}
//	}
//	if (r1.size > 0 && r2.size > 0)
//	{
//		std::vector<poidex> path1;
//		findPath(p1, p2, true, path1);
//
//		std::vector<poidex> path2;
//		findPath(p1, p2, false, path2);
//
//		if (path1.size() > 0 && path1.size() > path2.size())
//		{
//			RadiusHole* prev = nullptr;
//			RadiusHole* nd = new RadiusHole(path1);
//			nd->initRoot(this);
//
//			poidex prevP = -1;
//			for (auto& p : path1)
//			{
//				nd->add(barvalue(barvalue::getStatPoint(p, wid), img->getLiner(p)));
//				bool add = false;
//
//				if (prevP)
//				{
//					for (auto& lp1 : rebs[p].holes)
//					{
//						for (auto& lp2 : rebs[prevP].holes)
//						{
//							if (lp1 == lp2)
//							{
//								nd.tryAddChild(lp1);
//							}
//						}
//					}
//
//					p = prevP;
//				}
//
//				rebs[p].holes.push_back(nd);
//			}
//		}
//
//	}
//	else
//	{
//		rebs[p1].cons.push_back(p2);
//		rebs[p2].cons.push_back(p1);
//	}
//}
//
//RadiusHole::RadiusHole(const std::vector<poidex>& path)
//{
//	resline = new barline();
//}
//
//void RadiusHole::addConnection()
//{
//	if (--leftToCollapse == 0)
//	{
//		resline->
//	}
//}
//
//void RadiusHole::add(const barvalue& value)
//{
//	resline->matr.push_back(value);
//}
//void RadiusHole::tryAddChild()
//{
//	resline->childrenId.push_back()
//}
