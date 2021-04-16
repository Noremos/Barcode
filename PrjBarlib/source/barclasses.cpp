#include "barclasses.h"
#include <math.h>

#include <unordered_map>

template<class T>
bc::Barbase<T>::~Barbase() {}

template<class T>
bc::Baritem<T>::Baritem() {}

template<class T>
bc::Baritem<T>::Baritem(const bc::Baritem<T>& obj)
{
	barlines.insert(barlines.begin(), obj.barlines.begin(), obj.barlines.end());
}

template<class T>
void bc::Baritem<T>::add(T st, T len)
{
	barlines.push_back(new barline<T>(st, len));
}


template<class T>
void bc::Baritem<T>::add(bc::barline<T>* line)
{
	barlines.push_back(line);
}

template<class T>
T bc::Baritem<T>::sum() const
{
	T ssum = 0;
	for (const barline<T>* l : barlines)
		ssum += l->len;

	return ssum;
}

template<>
void bc::Baritem<uchar>::getBettyNumbers(int* bs)
{
	memset(bs, 0, 256 * sizeof(int));

	for (const barline<uchar>* l : barlines)
	{
		for (size_t i = l->start; i < l->end(); ++i)
		{
			++bs[i];
		}
	}
}


template<class T>
void bc::Baritem<T>::getBettyNumbers(int* /*bs*/)
{
}



//template<class T>
//void cloneGraph(bc::barline<T>* old, bc::barline<T>* newone)
//{
//	for (size_t i = 0; i < old->childrens.size(); i++)
//	{
//		cloneGraph(old->childrens[i], newone->childrens[i]);
//		old->childrens[i] = newone->childrens[i]->clone();
//	}
//}

template<class T>
bc::Baritem<T>* bc::Baritem<T>::clone() const
{
	std::unordered_map<barline<T>*, barline<T>*> oldNew;
	Baritem<T>* nb = new Baritem<T>();
	nb->barlines.insert(nb->barlines.begin(), barlines.begin(), barlines.end());
	bool createGraph = false;
	if ((barlines.size() > 0 && barlines[0]->parrent != nullptr) || barlines[0]->childrens.size() > 0)
		createGraph = true;

	for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
	{
		auto* nnew = nb->barlines[i]->clone();
		if (createGraph)
			oldNew.insert(std::pair<barline<T>*, barline<T>*>(nb->barlines[i], nnew));

		nb->barlines[i] = nnew;
	}
	if (createGraph)
	{
		for (size_t i = 0, total = nb->barlines.size(); i < total; ++i)
		{
			auto* nline = nb->barlines[i];
			nline->parrent = oldNew[nline->parrent];

			for (size_t i = 0; i < nline->childrens.size(); i++)
				nline->childrens[i] = oldNew[nline->childrens[i]];
		}
	}
	return nb;
}

template<class T>
T bc::Baritem<T>::maxLen() const
{
	T max = 0;
	for (const barline<T>* l : barlines)
		if (l->len > max)
			max = l->len;

	return max;
}

template<class T>
void bc::Baritem<T>::relen()
{
	if (barlines.size() == 0)
		return;

	T mini = barlines[0]->start;
	for (size_t i = 1; i < barlines.size(); ++i)
		if (barlines[i]->start < mini)
			mini = barlines[i]->start;

	for (size_t i = 0; i < barlines.size(); ++i)
		barlines[i]->start -= mini;

	//mini = std::min_element(arr.begin(), arr.end(), [](barline &b1, barline &b2) { return b1.start < b2.start; })->start;
	//std::for_each(arr.begin(), arr.end(), [mini](barline &n) {return n.start - uchar(mini); });
}

template<class T>
void bc::Baritem<T>::removePorog(const T porog)
{
	if (porog == 0)
		return;
	std::vector<barline<T>*> res;
	for (size_t i = 0; i < barlines.size(); i++)
	{
		barline<T>* line = barlines[i];
		if (line->len >= porog)
			res.push_back(line);
		else// if (line->isCopy)
			delete line;
	}
	barlines.clear();
	barlines.insert(barlines.begin(), res.begin(), res.end());
}

template<class T>
void bc::Baritem<T>::preprocessBar(const T& porog, bool normalize)
{
	this->removePorog(porog);

	if (normalize)
		this->relen();
}

template<class T>
float findCoof(bc::barline<T>* X, bc::barline<T>* Y, bc::CompireStrategy& strat)
{
    float maxlen, minlen;
	if (strat == bc::CompireStrategy::CommonToSum)
	{
		T st = MAX(X->start, Y->start);
		T ed = MIN(X->end(), Y->end());
        minlen = static_cast<float>(ed - st);

		st = MIN(X->start, Y->start);
		ed = MAX(X->end(), Y->end());
        maxlen = static_cast<float>(ed - st);
	}
	else if (strat == bc::CompireStrategy::CommonToLen)
	{
		T st = MAX(X->start, Y->start);
		T ed = MIN(X->end(), Y->end());
        minlen = static_cast<float>(ed - st);
        maxlen = static_cast<float>(MAX(X->len, Y->len));
	}
	else
	{
		return X->compire3dbars(Y, strat);
	}

	if (minlen <= 0 || maxlen <= 0)
		return -1;

	return minlen / maxlen;
}

#include <algorithm>

template<class T>
void soirBarlens(bc::barlinevector<T>& barl)
{
	std::sort(barl.begin(), barl.end(), [](const bc::barline<T>* a, const bc::barline<T>* b)
		{
			return a->len > b->len;
		});
}


template<class T>
float bc::Baritem<T>::compireBestRes(const bc::Baritem<T>* bc, bc::CompireStrategy strat) const
{
	barlinevector<T> Xbarlines = barlines;
	barlinevector<T> Ybarlines = dynamic_cast<const Baritem<T>*>(bc)->barlines;

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
					maxsum = (float)(Xbarlines[i]->len + Ybarlines[j]->len);
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

template<class T>
float bc::Baritem<T>::compireFull(const bc::Barbase<T>* bc, bc::CompireStrategy strat) const
{
	barlinevector<T> Xbarlines = barlines;
	barlinevector<T> Ybarlines = dynamic_cast<const Baritem<T>*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	float totalsum = 0;
    size_t n = MIN(Xbarlines.size(), Ybarlines.size());
	soirBarlens<T>(Xbarlines);
	soirBarlens<T>(Ybarlines);

	float tcoof = 0.f;
	for (size_t i = 0; i < n; ++i)
	{
        float coof = findCoof(Xbarlines[i], Ybarlines[i], strat);
		if (coof < 0)
			continue;

        float xysum = static_cast<float>(Xbarlines[i]->len + Ybarlines[i]->len);
		totalsum += xysum;
		tcoof += xysum * coof;
	}
	return totalsum!=0 ? tcoof / totalsum : 0;
}

template<class T>
float bc::Baritem<T>::compareOccurrence(const bc::Baritem<T>* bc, bc::CompireStrategy strat) const
{
	barlinevector<T> Xbarlines = barlines;
	barlinevector<T> Ybarlines = dynamic_cast<const Baritem<T>*>(bc)->barlines;

	if (Xbarlines.size() == 0 || Ybarlines.size() == 0)
		return 0;

	size_t n = static_cast<int>(MIN(Xbarlines.size(), Ybarlines.size()));
	soirBarlens<T>(Xbarlines);
	soirBarlens<T>(Ybarlines);

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
				maxsum = (float)(Xbarlines[re]->len + Ybarlines[j]->len);
				jk = j;
			}
		}
		Ybarlines.erase(Ybarlines.begin() + jk);
		totalsum += maxsum;
		coofsum += maxsum * maxCoof;
	}
	return coofsum / totalsum;
}

using std::string;

template<class T>
void bc::Baritem<T>::getJsonObejct(std::string &out)
{
	string nl = "\r\n";

	out = "{" + nl + "lines: ";

	getJsonLinesArray(out);
	out += nl + '}';
}

template<class T>
void bc::Baritem<T>::getJsonLinesArray(std::string &out)
{
	string nl = "\r\n";

	out = "[ ";

	for (bc::barline<T> *line : barlines)
	{
		line->getJsonObject(out);
		out += ",";
	}

	out[out.length() - 1] = ']';
}


template<class T>
void bc::Baritem<T>::sortByLen()
{
	soirBarlens<T>(barlines);
}

template<class T>
void bc::Baritem<T>::sortBySize()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline<T>* a, const bc::barline<T>* b)
		{
			return a->matr.size() > b->matr.size();
		});
}

template<class T>
bc::Baritem<T>::~Baritem()
{
	for (auto* bline : barlines)
	{
		if (bline!=nullptr)
			delete bline;
	}
	barlines.clear();

	if (rootNode != nullptr)
		delete rootNode;
}

//=======================barcontainer=====================

template<class T>
bc::Barcontainer<T>::Barcontainer()
{
}

template<class T>
T bc::Barcontainer<T>::sum() const
{
	T sm = 0;
	for (const Baritem<T> *it : items)
	{
		if (it!=nullptr)
			sm += it->sum();
	}
	return sm;
}

template<class T>
void bc::Barcontainer<T>::relen()
{
	for (Baritem<T> *it : items)
	{
		if (it!=nullptr)
			it->relen();
	}
}

template<class T>
T bc::Barcontainer<T>::maxLen() const
{
	T mx = 0;
	for (const Baritem<T>* it : items)
	{
		if (it!=nullptr)
		{
			T curm = it->maxLen();
			if (curm > mx)
				mx = curm;
		}
	}

	return mx;
}

template<class T>
size_t bc::Barcontainer<T>::count()
{
	return items.size();
}

template<class T>
bc::Baritem<T>* bc::Barcontainer<T>::getItem(size_t i)
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

template<class T>
bc::Baritem<T>* bc::Barcontainer<T>::lastItem()
{
	if (items.size() == 0)
		return nullptr;

	return items[items.size() - 1];
}

template<class T>
void bc::Barcontainer<T>::addItem(bc::Baritem<T>* item)
{
	items.push_back(item);
}

template<class T>
void bc::Barcontainer<T>::removePorog(const T porog)
{
	for (Baritem<T> *it : items)
	{
		if (it!=nullptr)
			it->removePorog(porog);
	}
}

template<class T>
void bc::Barcontainer<T>::preprocessBar(const T& porog, bool normalize)
{
	for (Baritem<T> *it : items)
	{
		if (it!=nullptr)
			it->preprocessBar(porog, normalize);
	}
}

template<class T>
bc::Barbase<T>* bc::Barcontainer<T>::clone() const
{
	Barcontainer* newBar = new Barcontainer<T>();

	for (Baritem<T>* it : items)
	{
		if (it!=nullptr)
			newBar->items.push_back(new Baritem<T>(*it));
	}
	return newBar;
}

template<class T>
float bc::Barcontainer<T>::compireFull(const bc::Barbase<T>* bc, bc::CompireStrategy strat) const
{
	const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
    float res = 0;
    float s = static_cast<float>(sum() + bcr->sum());
	for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
	{
		if (items[i]!=nullptr)
			res += items[i]->compireFull(bcr->items[i], strat) * static_cast<float>(items[i]->sum() + bcr->items[i]->sum()) / s;
	}

    return res;
}

template<class T>
bc::Barcontainer<T>::Barcontainer::~Barcontainer()
{
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (items[i] != nullptr)
			delete items[i];
	}
	items.clear();
}

INIT_TEMPLATE_TYPE(bc::Barbase)
INIT_TEMPLATE_TYPE(bc::Baritem)
INIT_TEMPLATE_TYPE(bc::Barcontainer)
