#include "barclasses.h"
#include <math.h>

template<class T>
bc::Barbase<T>::~Barbase() {}

template<class T>
float bc::Barbase<T>::compireBarcodes(const bc::Barbase<T>* X, const bc::Barbase<T>* Y, const CompireFunction& type)
{
	switch (type) {
	case CompireFunction::CommonToLen:
		return X->compireCTML(Y);
		break;
	case CompireFunction::CommonToSum:
		return X->compireCTS(Y);
		break;
	default:
		return 0;
		//X->compireCTML(Y);
		break;
	}
}

/////////////////////////////////////////////////////////

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

//void bc::Baritem::add(uchar st, uchar len, cv::Mat binmat)
//{
//    bar.push_back(barline(st, len, binmat));
//}
//template<class T>
//void bc::Baritem<T>::add(T st, T len, pmap<T>* binmat)
//{
//	bar.push_back(new barline<T>(st, len, binmat));
//}

template<class T>
void bc::Baritem<T>::add(bc::barline<T>* line)
{
	barlines.push_back(line);
}

template<class T>
T bc::Baritem<T>::sum() const
{
	T sum = 0;
	for (const barline<T>* l : barlines)
		sum += l->len;

	return sum;
}

template<class T>
bc::Baritem<T>* bc::Baritem<T>::clone() const
{
	Baritem* nb = new Baritem();
	nb->barlines.insert(nb->barlines.begin(), barlines.begin(), barlines.end());
	for (size_t i = 0, total = nb->barlines.size(); i < total; ++i) {
		nb->barlines[i] = nb->barlines[i]->clone();
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
	Baritem<T> res;
	for (barline<T>* line : barlines) {
		if (line->len >= porog)
			res.barlines.push_back(line);
		else
			delete line;
	}
	barlines.clear();
	barlines.insert(barlines.begin(), res.barlines.begin(), res.barlines.end());
}

template<class T>
void bc::Baritem<T>::preprocessBar(const T& porog, bool normalize)
{
	this->removePorog(porog);

	if (normalize)
		this->relen();
}

template<class T>
float bc::Baritem<T>::compireCTML(const bc::Barbase<T>* bc) const
{
	Baritem<T>* Y = dynamic_cast<const Baritem<T>*>(bc)->clone();
	Baritem<T>* X = clone();
	if (X->barlines.size() == 0 || Y->barlines.size() == 0)
		return 0;
	float sum = (float)(X->sum() + Y->sum());
	int n = static_cast<int>(MIN(barlines.size(), Y->barlines.size()));

	float tsum = 0.f;
	for (int re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t ik = 0;
		size_t jk = 0;
		for (size_t i = 0, totalI = X->barlines.size(); i < totalI; ++i)
		{
			for (size_t j = 0, totalY = Y->barlines.size(); j < totalY; ++j)
			{
				T st = MAX(X->barlines[i]->start, Y->barlines[j]->start);
				T ed = MIN(X->barlines[i]->start + X->barlines[i]->len, Y->barlines[j]->start + Y->barlines[j]->len);
				float minlen = (float)(ed - st);
				float maxlen = MAX(X->barlines[i]->len, Y->barlines[j]->len);
				//���� ������ 0, ������ ����� �� ������������
				if (minlen <= 0 || maxlen <= 0)
					continue;

				float coof = minlen / maxlen;
				if (coof > maxCoof) {
					maxCoof = coof;
					maxsum = (float)(X->barlines[i]->len + Y->barlines[j]->len);
					ik = i;
					jk = j;
				}
			}
		}
		X->barlines.erase(X->barlines.begin() + ik);
		Y->barlines.erase(Y->barlines.begin() + jk);
		tsum += maxsum * maxCoof;
	}
	return tsum / sum;
}

template<class T>
float bc::Baritem<T>::compireCTS(const bc::Barbase<T>* bc) const
{
	Baritem<T>* Y = dynamic_cast<const Baritem<T>*>(bc)->clone();
	Baritem<T>* X = clone();
	if (X->barlines.size() == 0 || Y->barlines.size() == 0)
		return 0;
	float sum = (float)(X->sum() + Y->sum());
	size_t n = static_cast<int>(MIN(X->barlines.size(), Y->barlines.size()));

	float tsum = 0.f;
	for (size_t re = 0; re < n; ++re)
	{
		float maxCoof = 0;
		float maxsum = 0;
		size_t ik = 0;
		size_t jk = 0;
		for (size_t i = 0, total = X->barlines.size(); i < total; ++i) {
			for (size_t j = 0, total2 = Y->barlines.size(); j < total2; ++j) {
				T st = MAX(X->barlines[i]->start, Y->barlines[j]->start);
				T ed = MIN(X->barlines[i]->start + X->barlines[i]->len, Y->barlines[j]->start + Y->barlines[j]->len);
				float minlen = (float)(ed - st);

				st = MIN(X->barlines[i]->start, Y->barlines[j]->start);
				ed = MAX(X->barlines[i]->start + X->barlines[i]->len, Y->barlines[j]->start + Y->barlines[j]->len);
				float maxlen = (float)(ed - st);

				//���� ������ 0, ������ ����� �� ������������
				if (minlen <= 0 || maxlen <= 0)
					continue;

				float coof = minlen / maxlen;
				if (coof > maxCoof)
				{
					maxCoof = coof;
					maxsum = (float)(X->barlines[i]->len + Y->barlines[j]->len);
					ik = i;
					jk = j;
				}
			}
		}
		X->barlines.erase(X->barlines.begin() + ik);
		Y->barlines.erase(Y->barlines.begin() + jk);
		tsum += maxsum * maxCoof;
	}
	return tsum / sum;
}
#include <algorithm>

template<class T>
void bc::Baritem<T>::sortByLen()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline<T> *a, const bc::barline<T> *b)
			  {
				  return a->len > b->len;
	});
}

template<class T>
void bc::Baritem<T>::sortBySize()
{
	std::sort(barlines.begin(), barlines.end(), [](const bc::barline<T> *a, const bc::barline<T> *b)
	{
		 return a->matr.size() > b->matr.size();
	});
}

template<class T>
bc::Baritem<T>::~Baritem()
{
	barlines.clear();
	for (auto* bline : barlines)
	{
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
	for (const Baritem<T>* it : items)
		sm += it->sum();

	return sm;
}

template<class T>
void bc::Barcontainer<T>::relen()
{
	for (Baritem<T>* it : items)
		it->relen();
}

template<class T>
T bc::Barcontainer<T>::maxLen() const
{
	T mx = 0;
	for (const Baritem<T>* it : items) {
		T curm = it->maxLen();
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
bc::Baritem<T>* bc::Barcontainer<T>::getItem(size_t i)
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
	for (Baritem<T>* it : items)
		it->preprocessBar(porog, normalize);
}

template<class T>
bc::Barbase<T>* bc::Barcontainer<T>::clone() const
{
	Barcontainer* newBar = new Barcontainer<T>();

	for (Baritem<T>* it : items)
		newBar->items.push_back(new Baritem<T>(*it));

	return newBar;
}

template<class T>
float bc::Barcontainer<T>::compireCTML(const bc::Barbase<T>* bc) const
{
	const Barcontainer* bcr = dynamic_cast<const Barcontainer*>(bc);
	double res = 0;
	double s = sum() + bcr->sum();
	for (size_t i = 0; i < MIN(items.size(), bcr->items.size()); i++)
	{
		res += items[i]->compireCTML(bcr->items[i]) * (items[i]->sum() + bcr->items[i]->sum()) / s;
	}

	return (float)res;
}

template<class T>
float bc::Barcontainer<T>::compireCTS(const bc::Barbase<T>* bc) const
{
	float res = 0;
	T s = sum();
	for (Baritem<T>* it : items)
		res += bc->compireCTS(it) * it->sum() / s;

	return (float)res;
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
	return compireCTML((const Baritem<T>*)bc);
}

template<class T>
float bc::Barcontainer<T>::cmpCTS(Barcontainer<T> const* bc) const
{
	return compireCTS((const Baritem<T>*)bc);
}
#endif

INIT_TEMPLATE_TYPE(bc::Barbase)
INIT_TEMPLATE_TYPE(bc::Baritem)
INIT_TEMPLATE_TYPE(bc::Barcontainer)
