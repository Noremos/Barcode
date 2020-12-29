#include "barcontainer.h"

bc::Baritem::Baritem() {}

bc::Baritem::Baritem(const bc::Baritem& obj)
{
	bar.insert(bar.begin(), obj.bar.begin(), obj.bar.end());
}

void bc::Baritem::add(uchar st, uchar len)
{
	bar.push_back(new bline(st, len));
}

//void bc::Baritem::add(uchar st, uchar len, cv::Mat binmat)
//{
//    bar.push_back(bline(st, len, binmat));
//}
void bc::Baritem::add(uchar st, uchar len, pmap* binmat)
{
	bar.push_back(new bline(st, len, binmat));
}

void bc::Baritem::add(bc::bline* line)
{
	bar.push_back(line);
}

int bc::Baritem::sum() const
{
	int sum = 0;
	for (const bline* l : bar)
		sum += l->len;

	return sum;
}

bc::Baritem* bc::Baritem::clone() const
{
	Baritem* nb = new Baritem();
	nb->bar.insert(nb->bar.begin(), bar.begin(), bar.end());
    for (int i = 0, total = nb->bar.size(); i < total; ++i) {
        nb->bar[i] = nb->bar[i]->clone();
    }
	return nb;
}

uchar bc::Baritem::maxLen() const
{
	uchar max = 0;
	for (const bline* l : bar)
		if (l->len > max)
			max = l->len;

	return max;
}

void bc::Baritem::relen()
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

void bc::Baritem::removePorog(const uchar porog)
{
	if (porog == 0)
		return;
	Baritem res;
	for (bline* line : bar) {
		if (line->len >= porog)
			res.bar.push_back(line);
		else
			delete line;
	}
	bar.clear();
	bar.insert(bar.begin(), res.bar.begin(), res.bar.end());
}

void bc::Baritem::preprocessBar(const int& porog, bool normalize)
{
	if (porog > 0)
		this->removePorog((uchar)roundf((porog * float(this->maxLen()) / 100.f)));

	if (normalize)
		this->relen();
}




float bc::Baritem::compireCTML(const bc::Barbase* bc) const
{
	Baritem* Y = dynamic_cast<const Baritem*>(bc)->clone();
	Baritem* X = clone();
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

float bc::Baritem::compireCTS(const bc::Barbase* bc) const
{
	Baritem* Y = dynamic_cast<const Baritem*>(bc)->clone();
	Baritem* X = clone();
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

bc::Baritem::~Baritem()
{
	bar.clear();
}

//=======================barcontainer=====================

bc::Barcontainer::Barcontainer()
{
}

int bc::Barcontainer::sum() const
{
	int sm = 0;
	for (const Baritem* it : items)
		sm += it->sum();

	return sm;
}

void bc::Barcontainer::relen()
{
	for (Baritem* it : items)
		it->relen();
}


uchar bc::Barcontainer::maxLen() const
{
	uchar mx = 0;
	for (const Baritem* it : items) {
		uchar curm = it->maxLen();
		if (curm > mx)
			mx = curm;
	}

	return mx;
}


size_t bc::Barcontainer::count()
{
	return items.size();
}

bc::Baritem* bc::Barcontainer::get(int i)
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

void bc::Barcontainer::removePorog(const uchar porog)
{
	for (Baritem* it : items)
		it->removePorog(porog);
}

void bc::Barcontainer::preprocessBar(const int& porog, bool normalize)
{
	for (Baritem* it : items)
		it->preprocessBar(porog, normalize);
}

bc::Barbase* bc::Barcontainer::clone() const
{
	Barcontainer* newBar = new Barcontainer();

	for (Baritem* it : items)
		newBar->items.push_back(new Baritem(*it));

	return newBar;
}


float bc::Barcontainer::compireCTML(const bc::Barbase* bc) const
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

float bc::Barcontainer::compireCTS(const bc::Barbase* bc) const
{
	float res = 0;
	float s = sum();
	for (Baritem* it : items)
		res += bc->compireCTS(it) * it->sum() / s;

	return res;
}
bc::Barcontainer::~Barcontainer()
{
	for (size_t i = 0; i < items.size(); ++i)
		delete items[i];
}

#ifdef _PYD
float bc::Barcontainer::cmpCTML(const Barcontainer* bc) const
{
	return compireCTML((const Baritem*)bc);
}

float bc::Barcontainer::cmpCTS(Barcontainer const* bc) const
{
	return compireCTS((const Baritem*)bc);
}
#endif
