#include "c_wrapper.h"

#include "barcodeCreator.h"

using namespace std;
using namespace bc;

extern "C"
{
	// For class : Baritem

	// Constructors:
	BaritemHandler create_Baritem(BaritemHandler obj)
	{
		int handl = BaritemCounter++;
		BaritemMap.insert(std::pair(handl, new Baritem(obj)));
		return handl;
	}

	// Desructor:
	void destroy_Baritem(BaritemHandler handler)
	{
		delete BaritemMap[handler];
	}

	// Functions:
	void Baritem_add(BaritemHandler handler, BarscalarHandler st, BarscalarHandler len)
	{
		BaritemMap[handler]->add(BarscalarMap[st], BarscalarMap[len]);
	}
	void Baritem_add(BaritemHandler handler, barlineHandler line)
	{
		BaritemMap[handler]->add(barlineMap[line]);
	}
	BarscalarHandler Baritem_sum(BaritemHandler handler)
	{
		Barscalar* ret = BaritemMap[handler]->sum();
		unsigned int handl = BaritemCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	void Baritem_relen(BaritemHandler handler)
	{
		BaritemMap[handler]->relen();
	}
	BarscalarHandler Baritem_maxLen(BaritemHandler handler)
	{
		Barscalar* ret = BaritemMap[handler]->maxLen();
		unsigned int handl = BaritemCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	BaritemHandler Baritem_clone(BaritemHandler handler)
	{
		Baritem* ret = BaritemMap[handler]->clone();
		unsigned int handl = BaritemCounter++;
		BaritemMap.insert(pair(handl, ret));
		return handl;
	}
	void Baritem_getBettyNumbers(BaritemHandler handler, int bs)
	{
		BaritemMap[handler]->getBettyNumbers(bs);
	}
	void Baritem_removePorog(BaritemHandler handler, BarscalarHandler porog)
	{
		BaritemMap[handler]->removePorog(BarscalarMap[porog]);
	}
	void Baritem_preprocessBar(BaritemHandler handler, BarscalarHandler porog, bool normalize)
	{
		BaritemMap[handler]->preprocessBar(BarscalarMap[porog], normalize);
	}
	float Baritem_compireFull(BaritemHandler handler, BarbaseHandler bc, CompireStrategy strat)
	{
		return BaritemMap[handler]->compireFull(BarbaseMap[bc], strat);
	}
	float Baritem_compireBestRes(BaritemHandler handler, BaritemHandler bc, CompireStrategy strat)
	{
		return BaritemMap[handler]->compireBestRes(BaritemMap[bc], strat);
	}
	float Baritem_compareOccurrence(BaritemHandler handler, BaritemHandler bc, CompireStrategy strat)
	{
		return BaritemMap[handler]->compareOccurrence(BaritemMap[bc], strat);
	}
	void Baritem_normalize(BaritemHandler handler)
	{
		BaritemMap[handler]->normalize();
	}
	void Baritem_getJsonObejct(BaritemHandler handler, string out)
	{
		BaritemMap[handler]->getJsonObejct(out);
	}
	void Baritem_getJsonLinesArray(BaritemHandler handler, string out)
	{
		BaritemMap[handler]->getJsonLinesArray(out);
	}
	void Baritem_setRootNode(BaritemHandler handler, BarRoot root)
	{
		BaritemMap[handler]->setRootNode(root);
	}
	BarscalarHandler Baritem_getMax(BaritemHandler handler)
	{
		Barscalar* ret = BaritemMap[handler]->getMax();
		unsigned int handl = BaritemCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	// For class : Barcontainer

	// Constructors:
	BarcontainerHandler create_Barcontainer()
	{
		int handl = BarcontainerCounter++;
		BarcontainerMap.insert(std::pair(handl, new Barcontainer()));
		return handl;
	}

	// Desructor:
	void destroy_Barcontainer(BarcontainerHandler handler)
	{
		delete BarcontainerMap[handler];
	}

	// Functions:
	BarscalarHandler Barcontainer_sum(BarcontainerHandler handler)
	{
		Barscalar* ret = BarcontainerMap[handler]->sum();
		unsigned int handl = BarcontainerCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	void Barcontainer_relen(BarcontainerHandler handler)
	{
		BarcontainerMap[handler]->relen();
	}
	BarbaseHandler Barcontainer_clone(BarcontainerHandler handler)
	{
		Barbase* ret = BarcontainerMap[handler]->clone();
		unsigned int handl = BarcontainerCounter++;
		BarbaseMap.insert(pair(handl, ret));
		return handl;
	}
	BarscalarHandler Barcontainer_maxLen(BarcontainerHandler handler)
	{
		Barscalar* ret = BarcontainerMap[handler]->maxLen();
		unsigned int handl = BarcontainerCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	unsigned long long Barcontainer_count(BarcontainerHandler handler)
	{
		size_t* ret = BarcontainerMap[handler]->count();
		unsigned int handl = BarcontainerCounter++;
		size_tMap.insert(pair(handl, ret));
		return handl;
	}
	BaritemHandler Barcontainer_getItem(BarcontainerHandler handler, unsigned long long i)
	{
		Baritem* ret = BarcontainerMap[handler]->getItem(size_tMap[i]);
		unsigned int handl = BarcontainerCounter++;
		BaritemMap.insert(pair(handl, ret));
		return handl;
	}
	void Barcontainer_setItem(BarcontainerHandler handler, unsigned long long index, BaritemHandler newOne)
	{
		BarcontainerMap[handler]->setItem(size_tMap[index], BaritemMap[newOne]);
	}
	BaritemHandler Barcontainer_exractItem(BarcontainerHandler handler, unsigned long long index)
	{
		Baritem* ret = BarcontainerMap[handler]->exractItem(size_tMap[index]);
		unsigned int handl = BarcontainerCounter++;
		BaritemMap.insert(pair(handl, ret));
		return handl;
	}
	void Barcontainer_remoeLast(BarcontainerHandler handler)
	{
		BarcontainerMap[handler]->remoeLast();
	}
	void Barcontainer_exractItems(BarcontainerHandler handler, vector< Baritem *)
	{
		BarcontainerMap[handler]->exractItems(Baritem *);
	}
	BaritemHandler Barcontainer_lastItem(BarcontainerHandler handler)
	{
		Baritem* ret = BarcontainerMap[handler]->lastItem();
		unsigned int handl = BarcontainerCounter++;
		BaritemMap.insert(pair(handl, ret));
		return handl;
	}
	void Barcontainer_addItem(BarcontainerHandler handler, BaritemHandler item)
	{
		BarcontainerMap[handler]->addItem(BaritemMap[item]);
	}
	void Barcontainer_removePorog(BarcontainerHandler handler, BarscalarHandler porog)
	{
		BarcontainerMap[handler]->removePorog(BarscalarMap[porog]);
	}
	void Barcontainer_preprocessBar(BarcontainerHandler handler, BarscalarHandler porog, bool normalize)
	{
		BarcontainerMap[handler]->preprocessBar(BarscalarMap[porog], normalize);
	}
	float Barcontainer_compireFull(BarcontainerHandler handler, BarbaseHandler bc, CompireStrategy strat)
	{
		return BarcontainerMap[handler]->compireFull(BarbaseMap[bc], strat);
	}
	float Barcontainer_compireBest(BarcontainerHandler handler, BaritemHandler bc, CompireStrategy strat)
	{
		return BarcontainerMap[handler]->compireBest(BaritemMap[bc], strat);
	}
	void Barcontainer_clear(BarcontainerHandler handler)
	{
		BarcontainerMap[handler]->clear();
	}
	// For class : barline

	// Constructors:
	barlineHandler create_barline(barlineHandler obj)
	{
		int handl = barlineCounter++;
		barlineMap.insert(std::pair(handl, new barline(obj)));
		return handl;
	}

	// Desructor:
	void destroy_barline(barlineHandler handler)
	{
		delete barlineMap[handler];
	}

	// Functions:
	int barline_getWid(barlineHandler handler)
	{
		return barlineMap[handler]->getWid();
	}
	BarRect barline_getBarRect(barlineHandler handler)
	{
		return barlineMap[handler]->getBarRect();
	}
	void barline_addCoord(barlineHandler handler, pointHandler first, BarscalarHandler bright)
	{
		barlineMap[handler]->addCoord(pointMap[first], BarscalarMap[bright]);
	}
	void barline_addCoord(barlineHandler handler, barvalue val)
	{
		barlineMap[handler]->addCoord(val);
	}
	barlineHandler barline_clone(barlineHandler handler)
	{
		barline* ret = barlineMap[handler]->clone();
		unsigned int handl = barlineCounter++;
		barlineMap.insert(pair(handl, ret));
		return handl;
	}
	void barline_setparent(barlineHandler handler, barlineHandler node)
	{
		barlineMap[handler]->setparent(barlineMap[node]);
	}
	BarscalarHandler barline_len(barlineHandler handler)
	{
		Barscalar* ret = barlineMap[handler]->len();
		unsigned int handl = barlineCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	float barline_lenFloat(barlineHandler handler)
	{
		return barlineMap[handler]->lenFloat();
	}
	int barline_getDeath(barlineHandler handler)
	{
		return barlineMap[handler]->getDeath();
	}
	BarscalarHandler barline_end(barlineHandler handler)
	{
		Barscalar* ret = barlineMap[handler]->end();
		unsigned int handl = barlineCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	unsigned long long barline_getPointsSize(barlineHandler handler)
	{
		size_t* ret = barlineMap[handler]->getPointsSize();
		unsigned int handl = barlineCounter++;
		size_tMap.insert(pair(handl, ret));
		return handl;
	}
	barvalue barline_getPoint(barlineHandler handler, unsigned long long index)
	{
		return barlineMap[handler]->getPoint(size_tMap[index]);
	}
	pointHandler barline_getPointValue(barlineHandler handler, unsigned long long index)
	{
		point* ret = barlineMap[handler]->getPointValue(size_tMap[index]);
		unsigned int handl = barlineCounter++;
		pointMap.insert(pair(handl, ret));
		return handl;
	}
	void barline_getChildsMatr(barlineHandler handler, unordered_map< bc::point, bool, bc::pointHash)
	{
		barlineMap[handler]->getChildsMatr(bc::point, bool, bc::pointHash);
	}
	void barline_getChildsMatr(barlineHandler handler, barvector vect)
	{
		barlineMap[handler]->getChildsMatr(vect);
	}
	float barline_compire3dbars(barlineHandler handler, barlineHandler inc, CompireStrategy cmp)
	{
		return barlineMap[handler]->compire3dbars(barlineMap[inc], cmp);
	}
	barlineHandler barline_getParent(barlineHandler handler)
	{
		barline* ret = barlineMap[handler]->getParent();
		unsigned int handl = barlineCounter++;
		barlineMap.insert(pair(handl, ret));
		return handl;
	}
	void barline_getJsonObject(barlineHandler handler, string outObj, bool exportGraph = false, bool export3dbar = false, bool expotrBinaryMask = false)
	{
		barlineMap[handler]->getJsonObject(outObj, exportGraph, export3dbar, expotrBinaryMask);
	}
	void barline_getChilredAsList(barlineHandler handler, barlinevector lines, bool includeItself, bool clone)
	{
		barlineMap[handler]->getChilredAsList(lines, includeItself, clone);
	}
	bool barline_is3dmoreing(barlineHandler handler)
	{
		return barlineMap[handler]->is3dmoreing();
	}
	// For class : Barscalar

	// Constructors:
	BarscalarHandler create_Barscalar(T i, BarType type)
	{
		int handl = BarscalarCounter++;
		BarscalarMap.insert(std::pair(handl, new Barscalar(i, type)));
		return handl;
	}

	BarscalarHandler create_Barscalar(BarscalarHandler R)
	{
		int handl = BarscalarCounter++;
		BarscalarMap.insert(std::pair(handl, new Barscalar(R)));
		return handl;
	}

	// Desructor:

	// Functions:
	float Barscalar_val_div(BarscalarHandler handler, BarscalarHandler X)
	{
		return BarscalarMap[handler]->val_div(BarscalarMap[X]);
	}
	float Barscalar_val_distance(BarscalarHandler handler, BarscalarHandler R)
	{
		return BarscalarMap[handler]->val_distance(BarscalarMap[R]);
	}
	BarscalarHandler Barscalar_absDiff(BarscalarHandler handler, BarscalarHandler R)
	{
		Barscalar* ret = BarscalarMap[handler]->absDiff(BarscalarMap[R]);
		unsigned int handl = BarscalarCounter++;
		BarscalarMap.insert(pair(handl, ret));
		return handl;
	}
	// For class : CachedValue

	// Constructors:
	// Desructor:

	// Functions:
	// For class : point

	// Constructors:
	// Desructor:

	// Functions:
	// For class : BarcodeCreator

	// Constructors:
	// Desructor:
	void destroy_BarcodeCreator(BarcodeCreatorHandler handler)
	{
		delete BarcodeCreatorMap[handler];
	}

	// Functions:
	BarcontainerHandler BarcodeCreator_createBarcode(BarcodeCreatorHandler handler, DatagridProvider img, BarConstructor structure)
	{
		Barcontainer* ret = BarcodeCreatorMap[handler]->createBarcode(img, structure);
		unsigned int handl = BarcodeCreatorCounter++;
		BarcontainerMap.insert(pair(handl, ret));
		return handl;
	}
	BarcontainerHandler BarcodeCreator_createBarcode(BarcodeCreatorHandler handler, ndarray img, BarConstructor structure)
	{
		Barcontainer* ret = BarcodeCreatorMap[handler]->createBarcode(img, structure);
		unsigned int handl = BarcodeCreatorCounter++;
		BarcontainerMap.insert(pair(handl, ret));
		return handl;
	}
}

