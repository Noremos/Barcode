#ifdef _PYD

#include "barcodeCreator.h"

#include "include_py.h"

//typedef float TV;
typedef uchar TV;
typedef barvec3b TV3d;
//typedef short TV;

#define PY_SILENS
#define TN(NAME) NAME

//class Barline8u : public bc::barline<uchar>
//{ };
//class Barline32s : public bc::barline<float>
//{ };
//BarcodeCreator
//BarConstructor
//Baritem
//bar3dvalue
//barvalue
//BarcodeCreator


BOOST_PYTHON_MODULE(barpy)
{
	bn::initialize();
	Py_Initialize();

	enum_<bc::AttachMode>("AttachMode")
		.value("firstEatSecond", bc::AttachMode::firstEatSecond)
		.value("secondEatFirst", bc::AttachMode::secondEatFirst)
		.value("createNew", bc::AttachMode::createNew)
		.value("dontTouch", bc::AttachMode::dontTouch)
		;

	enum_<bc::CompireStrategy>("CompireStrategy")
		.value("CommonToLen", bc::CompireStrategy::CommonToLen)
		.value("CommonToSum", bc::CompireStrategy::CommonToSum)
		.value("compire3d", bc::CompireStrategy::compire3dHist)
		.value("compire3d", bc::CompireStrategy::compire3dBrightless)
		;

	enum_<bc::ComponentType>("ComponentType")
		.value("Component", bc::ComponentType::Component)
		.value("RadiusComp", bc::ComponentType::RadiusComp)
		//.value("Hole", bc::ComponentType::Hole)
		;

	enum_<bc::ProcType>("ProcType")
		.value("f0t255", bc::ProcType::f0t255)
		.value("f255t0", bc::ProcType::f255t0)
		;

	enum_<bc::ColorType>("ColorType")
		.value("gray", bc::ColorType::gray)
		.value("native", bc::ColorType::native)
		.value("rgb", bc::ColorType::rgb)
		;

	enum_<bc::ReturnType>("ReturnType")
		.value("barcode2d", bc::ReturnType::barcode2d)
		.value("barcode3d", bc::ReturnType::barcode3d)
		;

	class_<bc::point>("Point")
		.def(init<int, int>(args("x", "y")))
		.add_property("x", make_getter(&bc::point::x), make_setter(&bc::point::x))
		.add_property("y", make_getter(&bc::point::y), make_setter(&bc::point::y))
		//.def("y", &bc::point::y)
		.def("init", static_cast<void (bc::point::*)(int, int)>(&bc::point::init), args("x", "y"))
		;


	//#define TN(NAME) (std::string(NAME)+"8u").c_str()
	//#include "pytemplcalsses.h"
	///////////////////////////////////////////// TYPE /////////////////////

	class_<bc::pybarvalue<TV>>("Matrvalue")
		.add_property("x", make_getter(&bc::pybarvalue<TV>::x), make_setter(&bc::pybarvalue<TV>::x))
		.add_property("y", make_getter(&bc::pybarvalue<TV>::y), make_setter(&bc::pybarvalue<TV>::y))
		.add_property("value", make_getter(&bc::pybarvalue<TV>::value), make_setter(&bc::pybarvalue<TV>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::bar3dvalue<TV>>("Bar3dvalue")
		.add_property("count", make_getter(&bc::bar3dvalue<TV>::count), make_setter(&bc::bar3dvalue<TV>::count))
		.add_property("value", make_getter(&bc::bar3dvalue<TV>::value), make_setter(&bc::bar3dvalue<TV>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::barline<TV>>("Barline")
		.def(init<TV, TV, int>(args("start", "len", "wid")))
		.add_property("start", make_getter(&bc::barline<TV>::start), make_setter(&bc::barline<TV>::start))
		.def("len", &bc::barline<TV>::len)
		.def("end", &bc::barline<TV>::end)
		.def("getPointsInDict", &bc::barline<TV>::getPointsInDict, (arg("skipChildPoints") = false))
		.def("getPoints", &bc::barline<TV>::getPoints, (arg("skipChildPoints") = false))
		.def("getPointsSize", &bc::barline<TV>::getPointsSize)
		.def("getMatrvalue", &bc::barline<TV>::getPoint, args("index"))
		.def("getRect", &bc::barline<TV>::getRect)
		.def("getParent", &bc::barline<TV>::getParent, return_internal_reference())
		.def("getChildren", &bc::barline<TV>::getChildren)
		.def("compire3dbars", &bc::barline<TV>::compire3dbars, args("inc", "compireStrategy"))

		.def("get3dList", &bc::barline<TV>::getBarcode3d)
		.def("get3dSize", &bc::barline<TV>::getBarcode3dSize)
		.def("get3dValue", &bc::barline<TV>::getBarcode3dValue, args("index"))

		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::Baritem<TV>>("Baritem")
		.def("sum", &bc::Baritem<TV>::sum)
		.def("relen", &bc::Baritem<TV>::relen)
		.def("clone", &bc::Baritem<TV>::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Baritem<TV>::maxLen)
		.def("removePorog", &bc::Baritem<TV>::removePorog, args("porog"))
		.def("preprocessBar", &bc::Baritem<TV>::preprocessBar, args("porog", "normalize"))
		.def("cmp", &bc::Baritem<TV>::cmp, args("bitem", "compireStrategy"))
		.def("cmpOccurrence", &bc::Baritem<TV>::compareOccurrence, args("bitem", "compireStrategy"))
		.def("compireBestRes", &bc::Baritem<TV>::compireBestRes, args("bitem", "compireStrategy"))
		.def("getBarcode", &bc::Baritem<TV>::getBarcode)
		.def("SortByLineLen", &bc::Baritem<TV>::sortByLen)
		.def("SortByPointsCount", &bc::Baritem<TV>::sortBySize)
		.def("calcHistByBarlen", &bc::Baritem<TV>::calcHistByBarlen)
		.def("getRootNode", &bc::Baritem<TV>::getRootNode, return_internal_reference()/*, make_setter(&bc::Baritem::rootNode)*/)

		.def("getBettyNumbers", &bc::Baritem<TV>::PY_getBettyNumbers)

		;

	class_<bc::Barcontainer<TV>>("Barcontainer")
		.def("sum", &bc::Barcontainer<TV>::sum)
		.def("relen", &bc::Barcontainer<TV>::relen)
		.def("clone", &bc::Barcontainer<TV>::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Barcontainer<TV>::maxLen)
		.def("count", &bc::Barcontainer<TV>::count)
		.def("removePorog", &bc::Barcontainer<TV>::removePorog, args("porog"))
		.def("preprocessBar", &bc::Barcontainer<TV>::preprocessBar, args("porog", "normalize"))
		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(&bc::Barcontainer::compireCTS), args("bc"))
		.def("addItem", &bc::Barcontainer<TV>::addItem, args("Baritem"))
		.def("getItem", &bc::Barcontainer<TV>::getItem, args("index"), return_internal_reference())
		;

	class_<bc::BarConstructor<TV>>("BarConstructor")
		.def("addStructure", &bc::BarConstructor<TV>::addStructure, args("ProcType", "ColorType", "ComponentType"))
		.def("setPorogStep", &bc::BarConstructor<TV>::setStep, args("porog"))
		.def("setMaxLen", &bc::BarConstructor<TV>::setMaxLen, args("len"))
		.add_property("returnType", &bc::BarConstructor<TV>::returnType, make_setter(&bc::BarConstructor<TV>::returnType))
		.add_property("createBinaryMasks", &bc::BarConstructor<TV>::createBinaryMasks, make_setter(&bc::BarConstructor<TV>::createBinaryMasks))
		.add_property("createGraph", &bc::BarConstructor<TV>::createGraph, make_setter(&bc::BarConstructor<TV>::createGraph))
		.add_property("attachMode", &bc::BarConstructor<TV>::attachMode, make_setter(&bc::BarConstructor<TV>::attachMode))
		.add_property("killOnMaxLen", &bc::BarConstructor<TV>::killOnMaxLen, make_setter(&bc::BarConstructor<TV>::killOnMaxLen))
		;
	;

	class_<bc::BarcodeCreator<TV>>("BarcodeCreator")
		.def("createBarcode", static_cast<bc::Barcontainer<TV>*(bc::BarcodeCreator<TV>::*) (bn::ndarray&, bc::BarConstructor<TV>&)>
			(&bc::BarcodeCreator<TV>::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
		;

	//////////////////////////3d//////////////////////////////////////////////////////////////////////////////////


	class_<bc::pybarvalue<TV3d>>("Matrvalue3d")
		.add_property("x", make_getter(&bc::pybarvalue<TV3d>::x), make_setter(&bc::pybarvalue<TV3d>::x))
		.add_property("y", make_getter(&bc::pybarvalue<TV3d>::y), make_setter(&bc::pybarvalue<TV3d>::y))
		.add_property("value", make_getter(&bc::pybarvalue<TV3d>::value), make_setter(&bc::pybarvalue<TV3d>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::bar3dvalue<TV3d>>("Bar3dvalue3d")
		.add_property("count", make_getter(&bc::bar3dvalue<TV3d>::count), make_setter(&bc::bar3dvalue<TV3d>::count))
		.add_property("value", make_getter(&bc::bar3dvalue<TV3d>::value), make_setter(&bc::bar3dvalue<TV3d>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::barline<TV3d>>("Barline3d")
		.def(init<TV3d, TV3d, int>(args("start", "len", "wid")))
		.add_property("start", make_getter(&bc::barline<TV3d>::start), make_setter(&bc::barline<TV3d>::start))
		.def("len", &bc::barline<TV3d>::len)
		.def("end", &bc::barline<TV3d>::end)
		.def("getPointsInDict", &bc::barline<TV3d>::getPointsInDict, (arg("skipChildPoints") = false))
		.def("getPoints", &bc::barline<TV3d>::getPoints, (arg("skipChildPoints") = false))
		.def("getPointsSize", &bc::barline<TV3d>::getPointsSize)
		.def("getMatrvalue", &bc::barline<TV3d>::getPoint, args("index"))
		.def("getRect", &bc::barline<TV3d>::getRect)
		.def("getParent", &bc::barline<TV3d>::getParent, return_internal_reference())
		.def("getChildren", &bc::barline<TV3d>::getChildren)
		.def("compire3dbars", &bc::barline<TV3d>::compire3dbars, args("inc", "compireStrategy"))

		.def("get3dList", &bc::barline<TV3d>::getBarcode3d)
		.def("get3dSize", &bc::barline<TV3d>::getBarcode3dSize)
		.def("get3dValue", &bc::barline<TV3d>::getBarcode3dValue, args("index"))

		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::Baritem<TV3d>>("Baritem3d")
		.def("sum", &bc::Baritem<TV3d>::sum)
		.def("relen", &bc::Baritem<TV3d>::relen)
		.def("clone", &bc::Baritem<TV3d>::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Baritem<TV3d>::maxLen)
		.def("removePorog", &bc::Baritem<TV3d>::removePorog, args("porog"))
		.def("preprocessBar", &bc::Baritem<TV3d>::preprocessBar, args("porog", "normalize"))
		.def("cmp", &bc::Baritem<TV3d>::cmp, args("bitem", "compireStrategy"))
		.def("cmpOccurrence", &bc::Baritem<TV3d>::compareOccurrence, args("bitem", "compireStrategy"))
		.def("compireBestRes", &bc::Baritem<TV3d>::compireBestRes, args("bitem", "compireStrategy"))
		.def("getBarcode", &bc::Baritem<TV3d>::getBarcode)
		.def("SortByLineLen", &bc::Baritem<TV3d>::sortByLen)
		.def("SortByPointsCount", &bc::Baritem<TV3d>::sortBySize)
		.def("calcHistByBarlen", &bc::Baritem<TV3d>::calcHistByBarlen)
		.def("getRootNode", &bc::Baritem<TV3d>::getRootNode, return_internal_reference()/*, make_setter(&bc::Baritem::rootNode)*/)

		.def("getBettyNumbers", &bc::Baritem<TV3d>::PY_getBettyNumbers)

		;

	class_<bc::Barcontainer<TV3d>>("Barcontainer3d")
		.def("sum", &bc::Barcontainer<TV3d>::sum)
		.def("relen", &bc::Barcontainer<TV3d>::relen)
		.def("clone", &bc::Barcontainer<TV3d>::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Barcontainer<TV3d>::maxLen)
		.def("count", &bc::Barcontainer<TV3d>::count)
		.def("removePorog", &bc::Barcontainer<TV3d>::removePorog, args("porog"))
		.def("preprocessBar", &bc::Barcontainer<TV3d>::preprocessBar, args("porog", "normalize"))
		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(&bc::Barcontainer::compireCTS), args("bc"))
		.def("addItem", &bc::Barcontainer<TV3d>::addItem, args("Baritem"))
		.def("getItem", &bc::Barcontainer<TV3d>::getItem, args("index"), return_internal_reference())
		;

	class_<bc::BarConstructor<TV3d>>("BarConstructor3d")
		.def("addStructure", &bc::BarConstructor<TV3d>::addStructure, args("ProcType", "ColorType", "ComponentType"))
		.def("setPorogStep", &bc::BarConstructor<TV3d>::setStep, args("porog"))
		.def("setMaxLen", &bc::BarConstructor<TV3d>::setMaxLen, args("len"))
		.add_property("returnType", &bc::BarConstructor<TV3d>::returnType, make_setter(&bc::BarConstructor<TV3d>::returnType))
		.add_property("createBinaryMasks", &bc::BarConstructor<TV3d>::createBinaryMasks, make_setter(&bc::BarConstructor<TV3d>::createBinaryMasks))
		.add_property("createGraph", &bc::BarConstructor<TV3d>::createGraph, make_setter(&bc::BarConstructor<TV3d>::createGraph))
		.add_property("attachMode", &bc::BarConstructor<TV3d>::attachMode, make_setter(&bc::BarConstructor<TV3d>::attachMode))
		.add_property("killOnMaxLen", &bc::BarConstructor<TV3d>::killOnMaxLen, make_setter(&bc::BarConstructor<TV3d>::killOnMaxLen))
		;
	;

	class_<bc::BarcodeCreator<TV3d>>("BarcodeCreator3d")
		.def("createBarcode", static_cast<bc::Barcontainer<TV3d>*(bc::BarcodeCreator<TV3d>::*) (bn::ndarray&, bc::BarConstructor<TV3d>&)>
			(&bc::BarcodeCreator<TV3d>::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
		;



	//#define TV float;
	//#define TN(NAME) (std::string(NAME)+"32s").c_str()
	//#include "pytemplcalsses.h"

	//#define TV short;
	//#define TN(NAME) (std::string(NAME)+"16s").c_str()
	//#include "pytemplcalsses.h"

}

//template<> 
//PyObject* type_into_python<barvector>::convert(barvector const& map)
//{
//	//auto d = t.date();
//	//auto tod = t.time_of_day();
//	//auto usec = tod.total_microseconds() % 1000000;
//	//return PyDateTime_FromDateAndTime(d.year(), d.month(), d.day(), tod.hours(), tod.minutes(), tod.seconds(), usec);
//	return nullptr;
//}
// 
//
//template<>
//PyObject* type_into_python<std::vector<bc::barline>>::convert(std::vector<bc::barline> const& bar)
//{
//	bp::object get_iter = bp::iterator<std::vector<bc::barline> >();
//	bp::object iter = get_iter(bar);
//	bp::list l(iter);
//	//return PyObject_New(bp::list, &l);
//
//	//bp::PyDateTime_FromDateAndTime
//	//PyObject_vec
//	//PyObject* py_object = new PyObject();
//	//py_object.
//	//bp::handle<> handle(py_object);
//	//bp::object* o =new object(bar);
//	//return o;//new list(bar)
//	//bp::list* barlest = new list();
//	//for (auto line : bar)
//	//	barlest.append(line);
//	//return barlest;
//	return nullptr;
//}


#endif // _PYD