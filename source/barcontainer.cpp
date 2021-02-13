#include "barcontainer.h"


template<class T>
bc::Baritem<T>::Baritem() {}

template<class T>
bc::Baritem<T>::Baritem(const bc::Baritem<T>& obj)
{
	bar.insert(bar.begin(), obj.bar.begin(), obj.bar.end());
}

template<class T>
void bc::Baritem<T>::add(T st, T len)
{
	bar.push_back(new bline<T>(st, len));
}

//void bc::Baritem::add(uchar st, uchar len, cv::Mat binmat)
//{
//    bar.push_back(bline(st, len, binmat));
//}
template<class T>
void bc::Baritem<T>::add(T st, T len, pmap<T>* binmat)
{
	bar.push_back(new bline<T>(st, len, binmat));
}

template<class T>
void bc::Baritem<T>::add(bc::bline<T>* line)
{
	bar.push_back(line);
}

template<class T>
double bc::Baritem<T>::sum() const
{
	int sum = 0;
	for (const bline<T>* l : bar)
		sum += l->len;

	return sum;
}

template<class T>
bc::Baritem<T>* bc::Baritem<T>::clone() const
{
	Baritem* nb = new Baritem();
	nb->bar.insert(nb->bar.begin(), bar.begin(), bar.end());
    for (int i = 0, total = nb->bar.size(); i < total; ++i) {
        nb->bar[i] = nb->bar[i]->clone();
    }
	return nb;
}

template<class T>
T bc::Baritem<T>::maxLen() const
{
	T max = 0;
	for (const bline<T>* l : bar)
		if (l->len > max)
			max = l->len;

	return max;
}

template<class T>
void bc::Baritem<T>::relen()
{
	if (bar.size() == 0)
		return;

	uchar mini = bar[0]->start;
	for (size_t i = 1; i < bar.size(); ++i)
		if (bar[i]->start < mini)
			mini = bar[i]->start;

	for (size_t i = 0; i < bar.size(); ++i)
		bar[i]->start -= mini;

	//mini = std::min_element(arr.begin(), arr.end(), [](bline &b1, bline &b2) { return b1.start < b2.start; })->start;
	//std::for_each(arr.begin(), arr.end(), [mini](bline &n) {return n.start - uchar(mini); });
}

template<class T>
void bc::Baritem<T>::removePorog(const T porog)
{
	if (porog == 0)
		return;
	Baritem<T> res;
	for (bline<T>* line : bar) {
		if (line->len >= porog)
			res.bar.push_back(line);
		else
			delete line;
	}
	bar.clear();
	bar.insert(bar.begin(), res.bar.begin(), res.bar.end());
}

template<class T>
void bc::Baritem<T>::preprocessBar(const int& porog, bool normalize)
{
	if (porog > 0)
		this->removePorog((uchar)roundf((porog * float(this->maxLen()) / 100.f)));

	if (normalize)
		this->relen();
}



template<class T>
float bc::Baritem<T>::compireCTML(const bc::Barbase* bc) const
{
	Baritem<T>* Y = dynamic_cast<const Baritem<T>*>(bc)->clone();
	Baritem<T>* X = clone();
	if (X->bar.size() == 0 || Y->bar.size() == 0)
		return 0;
	float sum = (float)(X->sum() + Y->sum());
	int n = static_cast<int>(MIN(bar.size(), Y->bar.size()));

	float tsum = 0.f;
	for (int re = 0; re < n; ++re) 
	{
		float maxCoof = 0;
		float maxsum = 0;
		int ik = 0;
		int jk = 0;
		for (int i = 0, totalI = X->bar.size(); i < totalI; ++i) {
			for (int j = 0, totalY = Y->bar.size(); j < totalY; ++j) {
				short st = MAX(X->bar[i]->start, Y->bar[j]->start);
				short ed = MIN(X->bar[i]->start + X->bar[i]->len, Y->bar[j]->start + Y->bar[j]->len);
				float minlen = (float)(ed - st);
				float maxlen = MAX(X->bar[i]->len, Y->bar[j]->len);
				//Если меньше 0, значит линии не пересекаются
				if (minlen <= 0 || maxlen <= 0)
					continue;

				float coof = minlen / maxlen;
				if (coof > maxCoof) {
					maxCoof = coof;
					maxsum = (float)(X->bar[i]->len + Y->bar[j]->len);
					ik = i;
					jk = j;
				}
			}
		}
		X->bar.erase(X->bar.begin() + ik);
		Y->bar.erase(Y->bar.begin() + jk);
		tsum += maxsum * maxCoof;
	}
	return tsum / sum;
}

template<class T>
float bc::Baritem<T>::compireCTS(const bc::Barbase* bc) const
{
	Baritem<T>* Y = dynamic_cast<const Baritem<T>*>(bc)->clone();
	Baritem<T>* X = clone();
	if (X->bar.size() == 0 || Y->bar.size() == 0)
		return 0;
	float sum = (float)(X->sum() + Y->sum());
	int n = static_cast<int>(MIN(X->bar.size(), Y->bar.size()));

	float tsum = 0.f;
	for (int re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		int ik = 0;
		int jk = 0;
		for (int i = 0, total = X->bar.size(); i < total; ++i) {
			for (int j = 0, total2 = Y->bar.size(); j < total2; ++j) {
				short st = MAX(X->bar[i]->start, Y->bar[j]->start);
				short ed = MIN(X->bar[i]->start + X->bar[i]->len, Y->bar[j]->start + Y->bar[j]->len);
				float minlen = (float)(ed - st);

				st = MIN(X->bar[i]->start, Y->bar[j]->start);
				ed = MAX(X->bar[i]->start + X->bar[i]->len, Y->bar[j]->start + Y->bar[j]->len);
				float maxlen = (float)(ed - st);

				//Если меньше 0, значит линии не пересекаются
				if (minlen <= 0 || maxlen <= 0)
					continue;

				float coof = minlen / maxlen;
				if (coof > maxCoof) 
				{
					maxCoof = coof;
					maxsum = (float)(X->bar[i]->len + Y->bar[j]->len);
					ik = i;
					jk = j;
				}
			}
		}
		X->bar.erase(X->bar.begin() + ik);
		Y->bar.erase(Y->bar.begin() + jk);
		tsum += maxsum * maxCoof;
	}
	return tsum / sum;
}

template<class T>
bc::Baritem<T>::~Baritem()
{
	bar.clear();
}

//=======================barcontainer=====================

template<class T>
bc::Barcontainer<T>::Barcontainer()
{
}

template<class T>
double bc::Barcontainer<T>::sum() const
{
	int sm = 0;
	for (const Baritem<T>* it : items)
		sm += it->sum();

	return sm;
}

template<class T>
void bc::Barcontainer<T>::relen()
{
	for (Baritem* it : items)
		it->relen();
}

template<class T>
T bc::Barcontainer<T>::maxLen() const
{
	T mx = 0;
	for (const Baritem<T>* it : items) {
		uchar curm = it->maxLen();
		if (curm > mx)
			mx = curm;
	}

	return mx;
}

template<class T>
size_t bc::Barcontainer<T>::count()
{
	return items.size();
}

template<class T>
bc::Baritem<T>* bc::Barcontainer<T>::get(int i)
{
	if (items.size() == 0)
		return nullptr;

	while (i < 0)
		i += items.size();

	while (i >= (int)items.size())
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
	for (Baritem<T>* it : items)
		it->removePorog(porog);
}

template<class T>
void bc::Barcontainer<T>::preprocessBar(const T& porog, bool normalize)
{
	for (Baritem* it : items)
		it->preprocessBar(porog, normalize);
}

template<class T>
bc::Barbase* bc::Barcontainer<T>::clone() const
{
	Barcontainer* newBar = new Barcontainer<T>();

	for (Baritem<T>* it : items)
		newBar->items.push_back(new Baritem<T>(*it));

	return newBar;
}

template<class T>
float bc::Barcontainer<T>::compireCTML(const bc::Barbase* bc) const
{
	const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
	float res = 0;
	float s = sum() + bcr->sum();
	for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
	{
		res += items[i]->compireCTML(bcr->items[i]) * (items[i]->sum() + bcr->items[i]->sum())  / s;
	}

	return res;
}

template<class T>
float bc::Barcontainer<T>::compireCTS(const bc::Barbase* bc) const
{
	float res = 0;
	float s = sum();
	for (Baritem<T>* it : items)
		res += bc->compireCTS(it) * it->sum() / s;

	return res;
}

template<class T>
bc::Barcontainer<T>::~Barcontainer()
{
	for (size_t i = 0; i < items.size(); ++i)
		delete items[i];
}

#ifdef _PYD
template<class T>
float bc::Barcontainer<T>::cmpCTML(const Barcontainer<T>* bc) const
{
	return compireCTML((const Baritem*)bc);
}

template<class T>
float bc::Barcontainer<T>::cmpCTS(Barcontainer<T> const* bc) const
{
	return compireCTS((const Baritem*)bc);
}
#endif
