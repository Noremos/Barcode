extern "C"
{
	// Class handlers
	typedef unsigned int BarbaseHandler;
	typedef unsigned int BaritemHandler;
	typedef unsigned int BarcontainerHandler;
	typedef unsigned int barlineHandler;
	typedef unsigned int BarscalarHandler;
	typedef unsigned int CachedValueHandler;
	typedef unsigned int pointHandler;
	typedef unsigned int BarcodeCreatorHandler;


	// For class : Baritem

	// Constructors:
	BaritemHandler create_Baritem(BaritemHandler obj);

	// Desructor:
	void destroy_Baritem(BaritemHandler handler);

	// Functions:
	void Baritem_add(BaritemHandler handler, BarscalarHandler st, BarscalarHandler len);
	void Baritem_add(BaritemHandler handler, barlineHandler line);
	BarscalarHandler Baritem_sum(BaritemHandler handler);
	void Baritem_relength(BaritemHandler handler);
	BarscalarHandler Baritem_maxLen(BaritemHandler handler);
	BaritemHandler Baritem_clone(BaritemHandler handler);
	void Baritem_getBettyNumbers(BaritemHandler handler, int bs);
	void Baritem_removeByThreshold(BaritemHandler handler, BarscalarHandler porog);
	void Baritem_preprocessBarcode(BaritemHandler handler, BarscalarHandler porog, bool normalize);
	float Baritem_compareFull(BaritemHandler handler, BarbaseHandler bc, CompareStrategy strat);
	float Baritem_compareBestRes(BaritemHandler handler, BaritemHandler bc, CompareStrategy strat);
	float Baritem_compareOccurrence(BaritemHandler handler, BaritemHandler bc, CompareStrategy strat);
	void Baritem_normalize(BaritemHandler handler);
	void Baritem_getJsonObejct(BaritemHandler handler, string out);
	void Baritem_getJsonLinesArray(BaritemHandler handler, string out);
	void Baritem_setRootNode(BaritemHandler handler, BarRoot root);
	BarscalarHandler Baritem_getMax(BaritemHandler handler);
	// For class : Barcontainer

	// Constructors:
	BarcontainerHandler create_Barcontainer();

	// Desructor:
	void destroy_Barcontainer(BarcontainerHandler handler);

	// Functions:
	BarscalarHandler Barcontainer_sum(BarcontainerHandler handler);
	void Barcontainer_relength(BarcontainerHandler handler);
	BarbaseHandler Barcontainer_clone(BarcontainerHandler handler);
	BarscalarHandler Barcontainer_maxLen(BarcontainerHandler handler);
	unsigned long long Barcontainer_count(BarcontainerHandler handler);
	BaritemHandler Barcontainer_getItem(BarcontainerHandler handler, unsigned long long i);
	void Barcontainer_setItem(BarcontainerHandler handler, unsigned long long index, BaritemHandler newOne);
	BaritemHandler Barcontainer_exractItem(BarcontainerHandler handler, unsigned long long index);
	void Barcontainer_remoeLast(BarcontainerHandler handler);
	void Barcontainer_exractItems(BarcontainerHandler handler, vector< Baritem *);
	BaritemHandler Barcontainer_lastItem(BarcontainerHandler handler);
	void Barcontainer_addItem(BarcontainerHandler handler, BaritemHandler item);
	void Barcontainer_removeByThreshold(BarcontainerHandler handler, BarscalarHandler porog);
	void Barcontainer_preprocessBarcode(BarcontainerHandler handler, BarscalarHandler porog, bool normalize);
	float Barcontainer_compareFull(BarcontainerHandler handler, BarbaseHandler bc, CompareStrategy strat);
	float Barcontainer_compireBest(BarcontainerHandler handler, BaritemHandler bc, CompareStrategy strat);
	void Barcontainer_clear(BarcontainerHandler handler);
	// For class : barline

	// Constructors:
	barlineHandler create_barline(barlineHandler obj);

	// Desructor:
	void destroy_barline(barlineHandler handler);

	// Functions:
	int barline_getWid(barlineHandler handler);
	BarRect barline_getBarRect(barlineHandler handler);
	void barline_addCoord(barlineHandler handler, pointHandler first, BarscalarHandler bright);
	void barline_addCoord(barlineHandler handler, barvalue val);
	barlineHandler barline_clone(barlineHandler handler);
	void barline_setparent(barlineHandler handler, barlineHandler node);
	BarscalarHandler barline_len(barlineHandler handler);
	float barline_lenFloat(barlineHandler handler);
	int barline_getDeath(barlineHandler handler);
	BarscalarHandler barline_end(barlineHandler handler);
	unsigned long long barline_getPointsSize(barlineHandler handler);
	barvalue barline_getPoint(barlineHandler handler, unsigned long long index);
	pointHandler barline_getPointValue(barlineHandler handler, unsigned long long index);
	void barline_getChildsMatr(barlineHandler handler, unordered_map< bc::point, bool, bc::pointHash);
	void barline_getChildsMatr(barlineHandler handler, barvector vect);
	float barline_compare3dbars(barlineHandler handler, barlineHandler inc, CompareStrategy cmp);
	barlineHandler barline_getParent(barlineHandler handler);
	void barline_getJsonObject(barlineHandler handler, string outObj, bool exportGraph = false, bool export3dbar = false, bool expotrBinaryMask = false);
	void barline_getChilredAsList(barlineHandler handler, barlinevector lines, bool includeItself, bool clone);
	bool barline_is3dmoreing(barlineHandler handler);
	// For class : Barscalar

	// Constructors:
	BarscalarHandler create_Barscalar(T i, BarType type);

	BarscalarHandler create_Barscalar(BarscalarHandler R);

	// Desructor:

	// Functions:
	float Barscalar_val_div(BarscalarHandler handler, BarscalarHandler X);
	float Barscalar_val_distance(BarscalarHandler handler, BarscalarHandler R);
	BarscalarHandler Barscalar_absDiff(BarscalarHandler handler, BarscalarHandler R);
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
	void destroy_BarcodeCreator(BarcodeCreatorHandler handler);

	// Functions:
	BarcontainerHandler BarcodeCreator_createBarcode(BarcodeCreatorHandler handler, DatagridProvider img, BarConstructor structure);
	BarcontainerHandler BarcodeCreator_createBarcode(BarcodeCreatorHandler handler, ndarray img, BarConstructor structure);
}

barmap<unsigned int, bc::Baritem*> BaritemMap;
unsigned int BaritemCounter;
barmap<unsigned int, bc::Barcontainer*> BarcontainerMap;
unsigned int BarcontainerCounter;
barmap<unsigned int, bc::barline*> barlineMap;
unsigned int barlineCounter;
barmap<unsigned int, Barscalar> BarscalarMap;
unsigned int BarscalarCounter;
barmap<unsigned int, bc::CachedValue*> CachedValueMap;
unsigned int CachedValueCounter;
barmap<unsigned int, bc::point*> pointMap;
unsigned int pointCounter;
barmap<unsigned int, bc::BarcodeCreator*> BarcodeCreatorMap;
unsigned int BarcodeCreatorCounter;
