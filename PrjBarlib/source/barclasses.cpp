#ifndef SKIP_M_INC

#include "../include/barclasses.h"
#include <math.h>

#endif

bc::Barbase::~Barbase() {}


bc::Baritem::Baritem(int wid, BarType type)
{
	this->wid = wid;
	this->type = type;
}

void bc::Baritem::add(Barscalar st, Barscalar len)
{
	(new barline(st, len, wid))->initRoot(this);
}

void bc::Baritem::add(bc::barline* line)
{
	if (line->id == UINT_MAX)
	{
		line->initRoot(this);
	}
}

Barscalar bc::Baritem::sum() const
{
	Barscalar ssum;
	for (const barline* l : barlines)
		ssum += l->len();

	return ssum;
}

std::array<int, 256> bc::Baritem::getBettyNumbers() const
{
	std::array<int, 256> bs;
	std::fill(bs.begin(), bs.end(), 0);

	for (const barline* l : barlines)
	{
		for (size_t i = l->start.data.b1; i < l->end().data.b1; ++i)
		{
			++bs[i];
		}
	}

	return bs;
}

//
//void cloneGraph(bc::barline* old, bc::barline* newone)
//{
//	for (size_t i = 0; i < old->children.size(); i++)
//	{
//		cloneGraph(old->children[i], newone->children[i]);
//		old->children[i] = newone->children[i]->clone();
//	}
//}

bc::Baritem* bc::Baritem::clone() const
{
	Baritem* nb = new Baritem(wid);
	nb->barlines.resize(barlines.size());


	for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
	{
		nb->barlines[i] = barlines[i]->clone();
	}

	return nb;
}

Barscalar bc::Baritem::maxLen() const
{
	Barscalar max;
	for (const barline* l : barlines)
		if (l->len() > max)
			max = l->len();

	return max;
}

void bc::Baritem::relength()
{
	if (barlines.size() == 0)
		return;

	Barscalar mini = barlines[0]->start;
	for (size_t i = 1; i < barlines.size(); ++i)
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;

	for (size_t i = 0; i < barlines.size(); ++i)
		barlines[i]->start -= mini;

	//mini = std::min_element(arr.begin(), arr.end(), [](barline &b1, barline &b2) { return b1.start < b2.start; })->start;
	//std::for_each(arr.begin(), arr.end(), [mini](barline &n) {return n.start - buchar(mini); });
}

void bc::Baritem::removeByThreshold(const Barscalar porog)
{
	if (porog == 0)
		return;
	std::vector<barline*> res;
	for (size_t i = 0; i < barlines.size(); i++)
	{
		barline* line = barlines[i];
		if (line->len() >= porog)
			res.push_back(line);
		else// if (line->isCopy)
			delete line;
	}
	barlines.clear();
	barlines.insert(barlines.begin(), res.begin(), res.end());
}

void bc::Baritem::preprocessBarcode(const Barscalar& porog, bool normalize)
{
	this->removeByThreshold(porog);

	if (normalize)
		this->relength();
}

float findCoof(bc::barline* X, bc::barline* Y, bc::CompareStrategy& strat)
{
	const Barscalar &Xst = X->start < X->end() ? X->start : X->end();
	const Barscalar &Xed = X->start < X->end() ? X->end() : X->start;

	const Barscalar &Yst = Y->start < Y->end() ? Y->start : Y->end();
	const Barscalar &Yed = Y->start < Y->end() ? Y->end() : Y->start;


	float maxlen, minlen;
	if (strat == bc::CompareStrategy::CommonToSum)
	{
		if (Xst == Yst && Xed == Yed)
			return 1.f;

		float st = MAX(Xst, Yst).getAvgFloat();
		float ed = MIN(Xed, Yed).getAvgFloat();
		minlen = ed - st;

		st = MIN(Xst, Yst).getAvgFloat();
		ed = MAX(Xed, Yed).getAvgFloat();
		maxlen = ed - st;
	}
	else if (strat == bc::CompareStrategy::CommonToLen)
	{
		if (Xst == Yst && Xed == Yed)
			return 1.f;

		// Start всегда меньше end
		float st = MAX(Xst, Yst).getAvgFloat();
		float ed = MIN(Xed, Yed).getAvgFloat();
		minlen = ed - st; // Может быть меньше 0, если воообще не перекасаются
		maxlen = MAX(X->lenFloat(), Y->lenFloat());
	}
	else
	{
		return X->compare3dbars(Y, strat);
	}

	if (minlen <= 0 || maxlen <= 0)
		return -1;

	assert(minlen <= maxlen);
	return minlen / maxlen;
}

#include <algorithm>


void soirBarlens(bc::barlinevector& barl)
{
	std::sort(barl.begin(), barl.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->len() > b->len();
		});
}

float bc::Baritem::compareBestRes(const bc::Baritem* bc, bc::CompareStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0.f;
	int n = static_cast<int>(MIN(barlines.size(), Ybarlines.size()));

	float tsum = 0.f;
	for (int re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t ik = 0;
		size_t jk = 0;
		for (size_t i = 0, totalI = Xbarlines.size(); i < totalI; ++i)
		{
			for (size_t j = 0, totalY = Ybarlines.size(); j < totalY; ++j)
			{
                float coof = findCoof(Xbarlines[i], Ybarlines[j], strat);
				if (coof < 0)
					continue;

				if (coof > maxCoof)
				{
					maxCoof = coof;
					maxsum = Xbarlines[i]->len().getAvgFloat() + Ybarlines[j]->len().getAvgFloat();
					ik = i;
					jk = j;
				}
			}
		}
		Xbarlines.erase(Xbarlines.begin() + ik);
		Ybarlines.erase(Ybarlines.begin() + jk);
		tsum += maxsum * maxCoof;
		totalsum += maxsum;
	}
	return tsum / totalsum;
}

float bc::Baritem::compareFull(const bc::Barbase* bc, bc::CompareStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0;
    size_t n = MIN(Xbarlines.size(), Ybarlines.size());
	soirBarlens(Xbarlines);
	soirBarlens(Ybarlines);

	float tcoof = 0.f;
	for (size_t i = 0; i < n; ++i)
	{
        float coof = findCoof(Xbarlines[i], Ybarlines[i], strat);
		if (coof < 0)
			continue;

		float xysum = static_cast<float>(Xbarlines[i]->len()) + static_cast<float>(Ybarlines[i]->len());
		totalsum += xysum;
		tcoof += xysum * coof;
	}
	return totalsum !=0 ? tcoof / totalsum : 0;
}

float bc::Baritem::compareOccurrence(const bc::Baritem* bc, bc::CompareStrategy strat) const
{
	barlinevector Xbarlines = barlines;
	barlinevector Ybarlines = dynamic_cast<const Baritem*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	size_t n = static_cast<int>(MIN(Xbarlines.size(), Ybarlines.size()));
	soirBarlens(Xbarlines);
	soirBarlens(Ybarlines);

	float coofsum = 0.f, totalsum = 0.f;
	for (size_t re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t jk = 0;
		for (size_t j = 0, total2 = Ybarlines.size(); j < total2; ++j)
		{
            float coof = findCoof(Xbarlines[re], Ybarlines[j], strat);
			if (coof < 0)
				continue;

			if (coof > maxCoof)
			{
				maxCoof = coof;
				maxsum = (float)(Xbarlines[re]->len() + Ybarlines[j]->len());
				jk = j;
			}
		}
		Ybarlines.erase(Ybarlines.begin() + jk);
		totalsum += maxsum;
		coofsum += maxsum * maxCoof;
	}
	return coofsum / totalsum;
}

void bc::Baritem::normalize()
{
	if (barlines.size() == 0)
		return;

	Barscalar mini = barlines[0]->start;
	Barscalar maxi = barlines[0]->end();
	for (size_t i = 1; i < barlines.size(); ++i)
	{
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;
		if (barlines[i]->end() > maxi)
			maxi = barlines[i]->end();
	}

	for (size_t i = 0; i < barlines.size(); ++i)
	{
		barlines[i]->start = (barlines[i]->start - mini) / (maxi - mini);
		barlines[i]->m_end = barlines[i]->start + (barlines[i]->len() - mini) / (maxi - mini);
	}
}

void bc::Baritem::sortByLen()
{
	soirBarlens(barlines);
}

void bc::Baritem::sortBySize()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->matr.size() > b->matr.size();
		});
}

void bc::Baritem::sortByStart()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline* a, const bc::barline* b)
		{
			return a->start > b->start;
		});
}

bc::Baritem::~Baritem()
{
	for (bc::barline* bline : barlines)
	{
		if (bline != nullptr)
			delete bline;
	}
	barlines.clear();

	//if (rootNode != nullptr)
	//	delete rootNode;
}

//=======================barcontainer=====================


bc::Barcontainer::Barcontainer()
{
}

Barscalar bc::Barcontainer::sum() const
{
	Barscalar sm;
	for (const Baritem *it : items)
	{
		if (it!=nullptr)
			sm += it->sum();
	}
	return sm;
}

void bc::Barcontainer::relength()
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->relength();
	}
}

Barscalar bc::Barcontainer::maxLen() const
{
	Barscalar mx;
	for (const Baritem* it : items)
	{
		if (it!=nullptr)
		{
			Barscalar curm = it->maxLen();
			if (curm > mx)
				mx = curm;
		}
	}

	return mx;
}

size_t bc::Barcontainer::count()
{
	return items.size();
}

bc::Baritem* bc::Barcontainer::getItem(size_t i)
{
	if (items.size() == 0)
		return nullptr;

	while (i < 0)
		i += items.size();

	while (i >= items.size())
		i -= items.size();

	return items[i];
}

//bc::Baritem *bc::Barcontainer::operator[](int i)
//{
//    if (items.size() == 0)
//        return nullptr;

//    while (i < 0)
//        i += items.size();

//    while (i >= (int) items.size())
//        i -= items.size();

//    return items[i];
//}


bc::Baritem* bc::Barcontainer::lastItem()
{
	if (items.size() == 0)
		return nullptr;

	return items[items.size() - 1];
}


void bc::Barcontainer::addItem(bc::Baritem* item)
{
	items.push_back(item);
}


void bc::Barcontainer::removeByThreshold(const Barscalar porog)
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->removeByThreshold(porog);
	}
}


void bc::Barcontainer::preprocessBarcode(const Barscalar& porog, bool normalize)
{
	for (Baritem *it : items)
	{
		if (it!=nullptr)
			it->preprocessBarcode(porog, normalize);
	}
}


bc::Barbase* bc::Barcontainer::clone() const
{
	Barcontainer* newBar = new Barcontainer();

	for (Baritem* it : items)
	{
		if (it!=nullptr)
			newBar->items.push_back(new Baritem(*it));
	}
	return newBar;
}


float bc::Barcontainer::compareFull(const bc::Barbase* bc, bc::CompareStrategy strat) const
{
	const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
    float res = 0;
    float s = static_cast<float>(sum() + bcr->sum());
	for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
	{
		if (items[i]!=nullptr)
			res += items[i]->compareFull(bcr->items[i], strat) * static_cast<float>(items[i]->sum() + bcr->items[i]->sum()) / s;
	}

    return res;
}

float bc::Barcontainer::compireBest(const bc::Baritem* bc, bc::CompareStrategy strat) const
{
	float res = 0;
	for (size_t i = 0; i < items.size(); i++)
	{
		if (items[i] != nullptr)
			res = MAX(items[i]->compareFull(bc, strat), res);
	}

	return res;
}

bc::Barcontainer::Barcontainer::~Barcontainer()
{
	clear();
}


#ifdef USE_OPENCV

//-------------BARIMG

Barscalar bc::BarMat::get(int x, int y) const
{
	if (type == BarType::BYTE8_3)
	{
		cv::Vec3b v = mat.at<cv::Vec3b>(y, x);
		return Barscalar(v.val[0], v.val[1], v.val[2]);
	}
	else
	{
		return Barscalar(mat.at<uchar>(y, x), BarType::BYTE8_1);
	}
}

#endif // USE_OPENCV
