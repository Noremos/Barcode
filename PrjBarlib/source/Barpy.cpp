#ifdef _PYD

#include "barcodeCreator.h"
#include "include_py.h"
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#define PY_SILENS
using namespace py;


PYBIND11_MODULE(barpy, m)
{
	enum_<bc::AttachMode>(m, "AttachMode")
		.value("firstEatSecond", bc::AttachMode::firstEatSecond)
		.value("secondEatFirst", bc::AttachMode::secondEatFirst)
		.value("createNew", bc::AttachMode::createNew)
		.value("dontTouch", bc::AttachMode::dontTouch)
		.value("morePointsEatLow", bc::AttachMode::morePointsEatLow)
		;

	enum_<bc::CompireStrategy>(m, "CompireStrategy")
		.value("CommonToLen", bc::CompireStrategy::CommonToLen)
		.value("CommonToSum", bc::CompireStrategy::CommonToSum)
		.value("compire3d", bc::CompireStrategy::compire3dHist)
		.value("compire3dBrightless", bc::CompireStrategy::compire3dBrightless)
		;

	enum_<bc::ComponentType>(m,  "ComponentType")
		.value("Component", bc::ComponentType::Component)
		.value("Hole", bc::ComponentType::Hole)
		;

	enum_<bc::ProcType>(m,  "ProcType")
		.value("f0t255", bc::ProcType::f0t255)
		.value("f255t0", bc::ProcType::f255t0)
		.value("Radius", bc::ProcType::Radius)
		;

	enum_<bc::ColorType>(m,  "ColorType")
		.value("gray", bc::ColorType::gray)
		.value("native", bc::ColorType::native)
		.value("rgb", bc::ColorType::rgb)
		;

	enum_<bc::ReturnType>(m,  "ReturnType")
		.value("barcode2d", bc::ReturnType::barcode2d)
		.value("barcode3d", bc::ReturnType::barcode3d)
		;

	enum_<BarType>(m, "BarType")
		.value("BYTE8_1", BarType::BYTE8_1)
		.value("BYTE8_3", BarType::BYTE8_3)
		.value("FLOAT32_1", BarType::FLOAT32_1)
		;


	
	class_<bc::point>(m,  "Point")
		.def(init<int, int>())
		.def_readwrite("x", &bc::point::x)
		.def_readwrite("y", & bc::point::y)
		;


	class_<Barscalar>(m, "Barscalar")
		.def(init<int>())
		.def(init<float, BarType>())
		.def(init<int, BarType>())
		.def(init<int, int, int>())
		.def(init<int, int, int, int>())
		.def("getByte8", &Barscalar::getByte8)
		.def("getFloat", &Barscalar::getFloat)
		.def("getRGB", &Barscalar::getRGB)
		.def("value", &Barscalar::pyvalue)
		.def("getAvgFloat", &Barscalar::getAvgFloat)
		.def("getAvgUchar", &Barscalar::getAvgUchar)
		.def("absDiff", &Barscalar::absDiff)
		.def(py::self == py::self)
		.def(py::self != py::self)
		.def(py::self > py::self)
		.def(py::self >= py::self)
		.def(py::self < py::self)
		.def(py::self <= py::self)
		.def(py::self + py::self)
		.def(py::self - py::self)
		.def(py::self * py::self)
		.def(py::self / py::self)
		;

	py::implicitly_convertible<char, Barscalar>();
	py::implicitly_convertible<uchar, Barscalar>();
	py::implicitly_convertible<int, Barscalar>();
	py::implicitly_convertible<float, Barscalar>();

	//py::implicitly_convertible<Barscalar, char>();
	//py::implicitly_convertible<Barscalar, uchar>();
	//py::implicitly_convertible<Barscalar, py::object>();
	//py::implicitly_convertible<Barscalar, float>();

	class_<bc::barvalue>(m, "Matrvalue")
		.def(py::init<>())
		.def_readwrite("x", &bc::barvalue::x)
		.def_readwrite("y", &bc::barvalue::y)
		.def_readwrite("value", &bc::barvalue::value)
		.def("getIndex", &bc::barvalue::getIndex)
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::bar3dvalue>(m,  "Bar3dvalue")
		.def_readwrite("count", &bc::bar3dvalue::count)
		.def_readwrite("value", &bc::bar3dvalue::value)
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::barline>(m,  "Barline")
		.def("start", &bc::barline::pystart)
		.def("len", &bc::barline::len)
		.def("end", &bc::barline::end)
		.def("getPoints", &bc::barline::getPoints, (arg("skipChildPoints") = false))
		.def("getPointsSize", &bc::barline::getPointsSize)
		.def("getMatrvalue", &bc::barline::getPoint)
		.def("getRect", &bc::barline::getRect)
		.def("getParent", &bc::barline::getParent, return_value_policy::reference_internal)
		.def("getChildren", &bc::barline::getChildren)
		.def("compire3dbars", &bc::barline::compire3dbars)

		.def("get3dList", &bc::barline::getBarcode3d)
		.def("get3dSize", &bc::barline::getBarcode3dSize)
		.def("get3dValue", &bc::barline::getBarcode3dValue)

		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::Baritem>(m,  "Baritem")
		.def("sum", &bc::Baritem::sum)
		.def("relen", &bc::Baritem::relen)
		.def("clone", &bc::Baritem::clone, return_value_policy::take_ownership)
		.def("maxLen", &bc::Baritem::maxLen)
		.def("removePorog", &bc::Baritem::removePorog)
		.def("preprocessBar", &bc::Baritem::preprocessBar)
		.def("cmp", &bc::Baritem::cmp, "bitem"_a, "compireStrategy"_a)
		.def("cmpOccurrence", &bc::Baritem::compareOccurrence, "bitem"_a, "compireStrategy"_a)
		.def("compireBestRes", &bc::Baritem::compireBestRes, "bitem"_a, "compireStrategy"_a)
		.def("getBarcode", &bc::Baritem::getBarcode, return_value_policy::reference_internal)
		.def("SortByLineLen", &bc::Baritem::sortByLen)
		.def("SortByPointsCount", &bc::Baritem::sortBySize)
		.def("calcHistByBarlen", &bc::Baritem::calcHistByBarlen)
		.def("getRootNode", &bc::Baritem::getRootNode, return_value_policy::reference_internal)
		.def("getBettyNumbers", &bc::Baritem::PY_getBettyNumbers)

		;

	class_<bc::Barcontainer>(m,  "Barcontainer")
		.def("sum", &bc::Barcontainer::sum)
		.def("relen", &bc::Barcontainer::relen)
		.def("clone", &bc::Barcontainer::clone, return_value_policy::take_ownership)
		.def("maxLen", &bc::Barcontainer::maxLen)
		.def("count", &bc::Barcontainer::count)
		.def("removePorog", &bc::Barcontainer::removePorog, "porog"_a)
		.def("preprocessBar", &bc::Barcontainer::preprocessBar, "porog"_a, "normalize"_a)
		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(m,  &bc::Barcontainer::compireCTS), args("bc"))
		.def("addItem", &bc::Barcontainer::addItem, "Baritem"_a)
		.def("getItem", &bc::Barcontainer::getItem, "index"_a, return_value_policy::reference_internal)
		;


	class_<bc::BarConstructor>(m,  "BarConstructor")
		.def(py::init<>())
		.def("addStructure", &bc::BarConstructor::addStructure, "ProcType"_a, "ColorType"_a, "ComponentType"_a)
		.def("setPorogStep", &bc::BarConstructor::setStep, "porog"_a)
		.def("setMaxLen", &bc::BarConstructor::setMaxLen, "len"_a)
		.def_readwrite("maxRadius", &bc::BarConstructor::maxRadius)
		.def_readwrite("returnType", &bc::BarConstructor::returnType)
		.def_readwrite("createBinaryMasks", &bc::BarConstructor::createBinaryMasks)
		.def_readwrite("createGraph", &bc::BarConstructor::createGraph)
		.def_readwrite("attachMode", &bc::BarConstructor::attachMode)
		.def_readwrite("killOnMaxLen", &bc::BarConstructor::killOnMaxLen)
		;
	;

	class_<bc::BarcodeCreator>(m,  "BarcodeCreator")
		.def(py::init<>())
		.def("createBarcode", &bc::BarcodeCreator::createPysBarcode, "image"_a, "structure"_a, return_value_policy::take_ownership)
		;

#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
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