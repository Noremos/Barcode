#ifdef _PYD

#include "barcodeCreator.h"
#include "BarImg.h"
#include "include_py.h"
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#define PY_SILENS
using namespace py;
#define VERSION_INFO "1.0.5"

void bc::barstruct::setMask(bn::array& img, int maskValueId)
{
	holder.reset(new bc::BarNdarray(img);)
	mask = holder.get();

	this->maskValueId = maskValueId;
}

PYBIND11_MODULE(libbarpy, m)
{
	enum_<bc::AttachMode>(m, "AttachMode")
		.value("firstEatSecond", bc::AttachMode::firstEatSecond)
		.value("secondEatFirst", bc::AttachMode::secondEatFirst)
		.value("createNew", bc::AttachMode::createNew)
		.value("dontTouch", bc::AttachMode::dontTouch)
		.value("morePointsEatLow", bc::AttachMode::morePointsEatLow)
		;

	enum_<bc::CompareStrategy>(m, "CompareStrategy")
		.value("CommonToLen", bc::CompareStrategy::CommonToLen)
		.value("CommonToSum", bc::CompareStrategy::CommonToSum)
		.value("compare3d", bc::CompareStrategy::compare3dHist)
		.value("compare3dBrightless", bc::CompareStrategy::compare3dBrightless)
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
		.def(init<uchar>())
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

	class_<bc::barvalue>(m, "MatrixValue")
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
		.def("start", &bc::barline::getStart)
		.def("len", &bc::barline::getLength)
		.def("end", &bc::barline::getEnd)
		.def("getPoints", &bc::barline::getPoints, (arg("skipChildPoints") = false))
		.def("getMatrixSize", &bc::barline::getPointsSize)
		.def("getMatrixValue", &bc::barline::getPoint)
		.def("getRect", &bc::barline::getRect)
		.def("getParent", &bc::barline::getParent, return_value_policy::reference_internal)
		.def("getChildren", &bc::barline::getChildren)
		.def("compare3dbars", &bc::barline::compare3dbars)

		.def("get3dList", &bc::barline::getBarcode3d)
		.def("get3dSize", &bc::barline::getBarcode3dSize)
		.def("get3dValue", &bc::barline::getBarcode3dValue)
		.def("getBettyNumbers", &bc::barline::PY_getBettyNumbers)
		.def("__len__", &bc::barline::getPointsSize)

		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::Baritem>(m,  "Baritem")
		.def("sum", &bc::Baritem::sum)
		.def("relength", &bc::Baritem::relength)
		.def("normalize", &bc::Baritem::normalize)
		.def("clone", &bc::Baritem::clone, return_value_policy::take_ownership)
		.def("maxLen", &bc::Baritem::maxLen)
		.def("removeByThreshold", &bc::Baritem::removeByThreshold)
		.def("preprocessBarcode", &bc::Baritem::preprocessBarcode)
		.def("cmp", &bc::Baritem::cmp, "bitem"_a, "CompareStrategy"_a)
		.def("compareOccurrence", &bc::Baritem::compareOccurrence, "bitem"_a, "CompareStrategy"_a)
		.def("compareBestRes", &bc::Baritem::compareBestRes, "bitem"_a, "CompareStrategy"_a)
		.def("getBarcodeLines", &bc::Baritem::getBarcodeLines, return_value_policy::reference_internal,
		     "Returns a list of barline")
		.def("SortByLineLen", &bc::Baritem::sortByLen)
		.def("SortByPointsCount", &bc::Baritem::sortBySize)
		.def("calcHistByBarlen", &bc::Baritem::calcHistByBarlen)
		.def("getRootNode", &bc::Baritem::getRootNode, return_value_policy::reference_internal)
		.def("getBettyNumbers", &bc::Baritem::PY_getBettyNumbers)
		.def("__len__", &bc::Baritem::getBarcodeLinesCount)
		;

	class_<bc::Barcontainer>(m,  "Barcontainer")
		.def("sum", &bc::Barcontainer::sum)
		.def("relength", &bc::Barcontainer::relength)
		.def("clone", &bc::Barcontainer::clone, return_value_policy::take_ownership)
		.def("maxLen", &bc::Barcontainer::maxLen)
		.def("count", &bc::Barcontainer::count)
		.def("removeByThreshold", &bc::Barcontainer::removeByThreshold, "threshold"_a)
		.def("preprocessBarcode", &bc::Barcontainer::preprocessBarcode, "threshold"_a, "normalize"_a)
		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(m,  &bc::Barcontainer::compireCTS), args("bc"))
		.def("addItem", &bc::Barcontainer::addItem, "Baritem"_a)
		.def("getItem", &bc::Barcontainer::getItem, "index"_a, return_value_policy::reference_internal)
		.def("__len__", &bc::Barcontainer::getBarcodesCount)
		;

	class_<bc::barstruct>(m,  "barstruct")
		.def(py::init<>())
		.def_readwrite("comtype", &bc::barstruct::comtype)
		.def_readwrite("proctype", &bc::barstruct::proctype)
		.def_readwrite("coltype", &bc::barstruct::coltype)
		.def_readwrite("maxRadius", &bc::barstruct::maxRadius)
		.def_readwrite("returnType", &bc::barstruct::returnType)
		.def_readwrite("createBinaryMasks", &bc::barstruct::createBinaryMasks)
		.def_readwrite("createGraph", &bc::barstruct::createGraph)
		.def_readwrite("attachMode", &bc::barstruct::attachMode)
		.def_readwrite("killOnMaxLen", &bc::barstruct::killOnMaxLen)
		.def_readwrite("colorRange", &bc::barstruct::colorRange)
		.def_readwrite("trueSort", &bc::barstruct::trueSort)
		.def("setMask", &bc::barstruct::setMask)
		.def("removeMask", &bc::barstruct::removeMask)
		.def("setMaxLen", &bc::barstruct::setMaxLen, "val"_a)
	;

	class_<bc::BarConstructor>(m,  "BarConstructor")
		.def(py::init<>())
		.def("addStructure", &bc::BarConstructor::addStructure, "ProcType"_a, "ColorType"_a, "ComponentType"_a)
		;
	;

	m.def("create", &bc::BarcodeCreator::pycreate, R"pbdoc(
			Create a single barcode

			Some other explanation about the add function.
		)pbdoc");

	m.def("createMultiple", &bc::BarcodeCreator::pycreateMultiple, R"pbdoc(
			Create multiple barcodes from a single image

			Some other explanation about the add function.
		)pbdoc");


#ifdef VERSION_INFO
	m.attr("__version__") = VERSION_INFO;
#else
	m.attr("__version__") = "dev";
#endif
}


#endif // _PYD