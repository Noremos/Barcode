#ifdef _PYD

#include "barcodeCreator.h"

#include "include_py.h"

//namespace python = boost::python;
//
//template< typename T >
//struct type_from_python
//{
//	type_from_python()
//	{
//		converter::registry::push_back(convertible, construct, type_id<T>());
//	}
//
//	static void* convertible(PyObject*)
//	{
//
//	}
//	static void construct(PyObject*, converter::rvalue_from_python_stage1_data*)
//	{
//
//	}
//};
// 
// to pytion
//template< typename T >
//struct type_into_python
//{
//	static PyObject* convert(T const&);
//};
//
//class BarlineWrapper
//{
//	bc::barline line;
//	template<class T>
//	BarlineWrapper()
//	{
//	}
//
//	template<class T>
//	BarlineWrapper(T start, T len)
//	{
//		line::
//	}
//};


//typedef float TV;
typedef uchar TV;

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


	enum_<bc::CompireStrategy>("CompireStrategy")
		.value("CommonToLen", bc::CompireStrategy::CommonToLen)
		.value("CommonToSum", bc::CompireStrategy::CommonToSum)
		.value("compire3d", bc::CompireStrategy::compire3dHist)
		.value("compire3d", bc::CompireStrategy::compire3dBrightless)
		;

	enum_<bc::ComponentType>("ComponentType")
		.value("Component", bc::ComponentType::Component)
		//.value("Hole", bc::ComponentType::Hole)
		;

	enum_<bc::ProcType>("ProcType")
		.value("f0t255", bc::ProcType::f0t255)
		.value("f255t0", bc::ProcType::f255t0)
		;

	enum_<bc::ColorType>("ColorType")
		.value("gray", bc::ColorType::gray)
		//.value("native", bc::ColorType::native)
		//.value("rgb", bc::ColorType::rgb)
		;

	enum_<bc::ReturnType>("ReturnType")
		.value("betty", bc::ReturnType::betty)
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

	class_<bc::barvalue<TV>>("Matrvalue")
		.add_property("x", &bc::barvalue<TV>::getX, &bc::barvalue<TV>::setX)
		.add_property("y", &bc::barvalue<TV>::getY, &bc::barvalue<TV>::setY)
		.add_property("point", make_getter(&bc::barvalue<TV>::point), make_setter(&bc::barvalue<TV>::point))
		.add_property("value", make_getter(&bc::barvalue<TV>::value), make_setter(&bc::barvalue<TV>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::bar3dvalue<TV>>("Bar3dvalue")
		.add_property("count", make_getter(&bc::bar3dvalue<TV>::count), make_setter(&bc::bar3dvalue<TV>::count))
		.add_property("value", make_getter(&bc::bar3dvalue<TV>::value), make_setter(&bc::bar3dvalue<TV>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::barline<TV>>("Barline")
		.def(init<TV, TV>(args("start", "len")))
		.add_property("start", make_getter(&bc::barline<TV>::start), make_setter(&bc::barline<TV>::start))
		.add_property("len", make_getter(&bc::barline<TV>::len), make_setter(&bc::barline<TV>::len))
		.def("end", &bc::barline<TV>::end)
		.def("getPointsInDict", &bc::barline<TV>::getPointsInDict, (arg("skipChildPoints")=false))
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
		.add_property("createBinaryMasks", &bc::BarConstructor<TV>::createBinayMasks, make_setter(&bc::BarConstructor<TV>::createBinayMasks))
		.add_property("createGraph", &bc::BarConstructor<TV>::createGraph, make_setter(&bc::BarConstructor<TV>::createGraph))
		.add_property("createNewComponentOnAttach", &bc::BarConstructor<TV>::createNewComponentOnAttach, make_setter(&bc::BarConstructor<TV>::createNewComponentOnAttach))
		;
	;

	class_<bc::BarcodeCreator<TV>>("BarcodeCreator")
		.def("createBarcode", static_cast<bc::Barcontainer<TV>*(bc::BarcodeCreator<TV>::*) (bn::ndarray&, bc::BarConstructor<TV>&)>
			(&bc::BarcodeCreator<TV>::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
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