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

BOOST_PYTHON_MODULE(barpy)
{
	bn::initialize();
	Py_Initialize();

	class_<bc::point>("Point")
		.def(init<int, int>(args("x", "y")))
		.add_property("x", make_getter(&bc::point::x), make_setter(&bc::point::x))
		.add_property("y", make_getter(&bc::point::y), make_setter(&bc::point::y))
		//.def("y", &bc::point::y)
		.def("init", static_cast<void (bc::point::*)(int, int)>(&bc::point::init), args("x", "y"))
		;

	//class_<bc::pointHash>("PointHash")
	//	;
	//class_<pmap>("Pmap")
	//	;
	//class_<ppair>("Ppair")
	//	;
	//class_<std::vector<bc::barline>>("LinesList")
	//	;
	class_<bc::ppair<TV>>("Matrvalue")
		.add_property("x", &bc::ppair<TV>::getX, &bc::ppair<TV>::setX)
		.add_property("y", &bc::ppair<TV>::getY, &bc::ppair<TV>::setY)
		.add_property("point", make_getter(&bc::ppair<TV>::point), make_setter(&bc::ppair<TV>::point))
		.add_property("value", make_getter(&bc::ppair<TV>::value), make_setter(&bc::ppair<TV>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::bar3dpair<TV>>("Bar3dvalue")
		.add_property("count", make_getter(&bc::bar3dpair<TV>::count), make_setter(&bc::bar3dpair<TV>::count))
		.add_property("value", make_getter(&bc::bar3dpair<TV>::value), make_setter(&bc::bar3dpair<TV>::value))
		//.add_property("points", make_getter(&bc::barline::matr))
		;

	class_<bc::barline<TV>>("Barline")
		.def(init<TV, TV>(args("start", "len")))
		.add_property("start", make_getter(&bc::barline<TV>::start), make_setter(&bc::barline<TV>::start))
		.add_property("len", make_getter(&bc::barline<TV>::len), make_setter(&bc::barline<TV>::len))
		.def("end", &bc::barline<TV>::end)
		.def("getPointsInDict", &bc::barline<TV>::getPointsInDict)
		.def("getPoints", &bc::barline<TV>::getPoints)
		.def("getPointsSize", &bc::barline<TV>::getPointsSize)
		.def("getMatrvalue", &bc::barline<TV>::getPoint, args("index"))
		.def("getRect", &bc::barline<TV>::getRect)
		.def("getParent", &bc::barline<TV>::getParent, return_internal_reference())
		.def("getChildren", &bc::barline<TV>::getChildren)

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
		.def("cmpCTML", &bc::Baritem<TV>::cmpCTML, args("bitem"))
		.def("cmpCTS", &bc::Baritem<TV>::cmpCTS, args("bitem"))
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
		.def("cmpCTML", &bc::Barcontainer<TV>::cmpCTML, args("bc"))
		.def("cmpCTS", &bc::Barcontainer<TV>::cmpCTS, args("bc"))
		.def("addItem", &bc::Barcontainer<TV>::addItem, args("Baritem"))
		.def("getItem", &bc::Barcontainer<TV>::getItem, args("index"), return_internal_reference())
		;

	enum_<bc::CompireFunction>("CompireFunction")
		.value("CommonToLen", bc::CompireFunction::CommonToLen)
		.value("CommonToSum", bc::CompireFunction::CommonToSum)
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

	//class_<bc::barstruct>("Barstruct")
	//	.def(init())
	//	.def(init< bc::ProcType, bc::ColorType, bc::ComponentType>(args("ProcType", "ColorType", "ComponentType")))
	//	.add_property("comType", &bc::barstruct::comtype)
	//	.add_property("procType", &bc::barstruct::proctype)
	//	.add_property("colType", &bc::barstruct::coltype);

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
		.def("createBarcode", static_cast<bc::Barcontainer<TV> * (bc::BarcodeCreator<TV>::*) (bn::ndarray&, bc::BarConstructor<TV>&)>
			(&bc::BarcodeCreator<TV>::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
		;
	;


/* class_<bc::Barcode>("Barcode")
		.def("bar", return_value_policy<reference_existing_object>())
		.def("add", &bc::Barcode::add, args("start","len"))
		.def("sum", &bc::Barcode::sum)
		.def("relen", &bc::Barcode::relen)
		.def("clone", &bc::Barcode::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Barcode::maxLen)
		.def("count", &bc::Barcode::count)
		.def("removePorog", &bc::Barcode::removePorog, args("porog"))
		.def("preprocessBar", &bc::Barcode::preprocessBar, args("porog", "normalize"))
		.def("compireCTML", &bc::Barcode::compireCTML, args("bc"))
		.def("compireCTS", &bc::Barcode::compireCTS, args("bc"))
		;*/
}

//template<> 
//PyObject* type_into_python<pmap>::convert(pmap const& map)
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