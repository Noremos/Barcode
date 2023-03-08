#ifdef _PYD

#include "barcodeCreator.h"
#include "include_py.h"

#define PY_SILENS
#define TN(NAME) NAME


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
		.value("Hole", bc::ComponentType::Hole)
		;

	enum_<bc::ProcType>("ProcType")
		.value("f0t255", bc::ProcType::f0t255)
		.value("f255t0", bc::ProcType::f255t0)
		.value("Radius", bc::ProcType::Radius)
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

	class_<bc::pybarvalue>("Matrvalue")
		.add_property("x", make_getter(&bc::pybarvalue::x), make_setter(&bc::pybarvalue::x))
		.add_property("y", make_getter(&bc::pybarvalue::y), make_setter(&bc::pybarvalue::y))
		.add_property("value", make_getter(&bc::pybarvalue::value), make_setter(&bc::pybarvalue::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::bar3dvalue>("Bar3dvalue")
		.add_property("count", make_getter(&bc::bar3dvalue::count), make_setter(&bc::bar3dvalue::count))
		.add_property("value", make_getter(&bc::bar3dvalue::value), make_setter(&bc::bar3dvalue::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::barline>("Barline")
		.def("start", &bc::barline::pystart)
		.def("len", &bc::barline::pylen)
		.def("end", &bc::barline::pyend)
		.def("getPointsInDict", &bc::barline::getPointsInDict, (arg("skipChildPoints") = false))
		.def("getPoints", &bc::barline::getPoints, (arg("skipChildPoints") = false))
		.def("getPointsSize", &bc::barline::getPointsSize)
		.def("getMatrvalue", &bc::barline::getPoint, args("index"))
		.def("getRect", &bc::barline::getRect)
		.def("getParent", &bc::barline::getParent, return_internal_reference())
		.def("getChildren", &bc::barline::getChildren)
		.def("compire3dbars", &bc::barline::compire3dbars, args("inc", "compireStrategy"))

		.def("get3dList", &bc::barline::getBarcode3d)
		.def("get3dSize", &bc::barline::getBarcode3dSize)
		.def("get3dValue", &bc::barline::getBarcode3dValue, args("index"))

		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::Baritem>("Baritem")
		.def("sum", &bc::Baritem::sum)
		.def("relen", &bc::Baritem::relen)
		.def("clone", &bc::Baritem::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Baritem::maxLen)
		.def("removePorog", &bc::Baritem::removePorog, args("porog"))
		.def("preprocessBar", &bc::Baritem::preprocessBar, args("porog", "normalize"))
		.def("cmp", &bc::Baritem::cmp, args("bitem", "compireStrategy"))
		.def("cmpOccurrence", &bc::Baritem::compareOccurrence, args("bitem", "compireStrategy"))
		.def("compireBestRes", &bc::Baritem::compireBestRes, args("bitem", "compireStrategy"))
		.def("getBarcode", &bc::Baritem::getBarcode)
		.def("SortByLineLen", &bc::Baritem::sortByLen)
		.def("SortByPointsCount", &bc::Baritem::sortBySize)
		.def("calcHistByBarlen", &bc::Baritem::calcHistByBarlen)
		.def("getRootNode", &bc::Baritem::getRootNode, return_internal_reference()/*, make_setter(&bc::Baritem::rootNode)*/)

		.def("getBettyNumbers", &bc::Baritem::PY_getBettyNumbers)

		;

	class_<bc::Barcontainer>("Barcontainer")
		.def("sum", &bc::Barcontainer::sum)
		.def("relen", &bc::Barcontainer::relen)
		.def("clone", &bc::Barcontainer::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Barcontainer::maxLen)
		.def("count", &bc::Barcontainer::count)
		.def("removePorog", &bc::Barcontainer::removePorog, args("porog"))
		.def("preprocessBar", &bc::Barcontainer::preprocessBar, args("porog", "normalize"))
		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(&bc::Barcontainer::compireCTS), args("bc"))
		.def("addItem", &bc::Barcontainer::addItem, args("Baritem"))
		.def("getItem", &bc::Barcontainer::getItem, args("index"), return_internal_reference())
		;

	class_<bc::BarConstructor>("BarConstructor")
		.def("addStructure", &bc::BarConstructor::addStructure, args("ProcType", "ColorType", "ComponentType"))
		.def("setPorogStep", &bc::BarConstructor::setStep, args("porog"))
		.def("setMaxLen", &bc::BarConstructor::setMaxLen, args("len"))
		.add_property("returnType", &bc::BarConstructor::returnType, make_setter(&bc::BarConstructor::returnType))
		.add_property("createBinaryMasks", &bc::BarConstructor::createBinaryMasks, make_setter(&bc::BarConstructor::createBinaryMasks))
		.add_property("createGraph", &bc::BarConstructor::createGraph, make_setter(&bc::BarConstructor::createGraph))
		.add_property("attachMode", &bc::BarConstructor::attachMode, make_setter(&bc::BarConstructor::attachMode))
		.add_property("killOnMaxLen", &bc::BarConstructor::killOnMaxLen, make_setter(&bc::BarConstructor::killOnMaxLen))
		;
	;

	class_<bc::BarcodeCreator>("BarcodeCreator")
		.def("createBarcode", &bc::BarcodeCreator::createPysBarcode, args("image", "structure"), return_value_policy< manage_new_object >())
		;
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