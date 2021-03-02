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
	
	class_<bc::barline<uchar>>("Barline")
		.def(init<uchar, uchar>(args("start", "len")))
		.add_property("start", make_getter(&bc::barline<uchar>::start), make_setter(&bc::barline<uchar>::start))
		.add_property("len", make_getter(&bc::barline<uchar>::len), make_setter(&bc::barline<uchar>::len))
		.def("end", &bc::barline<uchar>::end)
		.def("getPoints", &bc::barline<uchar>::getPoints)
		.def("getRect", &bc::barline<uchar>::getRect)
		//.add_property("points", make_getter(&bc::barline::matr))
		;


	class_<bc::Baritem<uchar>>("Baritem")
		.def("sum", &bc::Baritem<uchar>::sum)
		.def("relen", &bc::Baritem<uchar>::relen)
		.def("clone", &bc::Baritem<uchar>::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Baritem<uchar>::maxLen)
		.def("removePorog", &bc::Baritem<uchar>::removePorog, args("porog"))
		.def("preprocessBar", &bc::Baritem<uchar>::preprocessBar, args("porog", "normalize"))
		.def("cmpCTML", &bc::Baritem<uchar>::cmpCTML, args("bitem"))
		.def("cmpCTS", &bc::Baritem<uchar>::cmpCTS, args("bitem"))
		.def("getBarcode", &bc::Baritem<uchar>::getBarcode)
		.def("SortByLineLen", &bc::Baritem<uchar>::sortByLen)
		.def("SortByPointsCount", &bc::Baritem<uchar>::sortBySize)


		//.add_property("bar", make_getter(&bc::Baritem::bar))
		;

	class_<bc::Barcontainer<uchar>>("Barcontainer")
		.def("sum", &bc::Barcontainer<uchar>::sum)
		.def("relen", &bc::Barcontainer<uchar>::relen)
		.def("clone", &bc::Barcontainer<uchar>::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Barcontainer<uchar>::maxLen)
		.def("count", &bc::Barcontainer<uchar>::count)
		.def("removePorog", &bc::Barcontainer<uchar>::removePorog, args("porog"))
		.def("preprocessBar", &bc::Barcontainer<uchar>::preprocessBar, args("porog", "normalize"))
		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(&bc::Barcontainer::compireCTS), args("bc"))
		.def("cmpCTML", &bc::Barcontainer<uchar>::cmpCTML, args("bc"))
		.def("cmpCTS", &bc::Barcontainer<uchar>::cmpCTS, args("bc"))
		.def("addItem", &bc::Barcontainer<uchar>::addItem, args("Baritem"))
		.def("getItem", &bc::Barcontainer<uchar>::getItem, args("index"), return_value_policy< reference_existing_object>())
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
	
	class_<bc::BarConstructor<uchar>>("BarConstructor")
		.def("addStructire", &bc::BarConstructor<uchar>::addStructire, args("ProcType", "ColorType", "ComponentType"))
		.def("setPorogStep", &bc::BarConstructor<uchar>::setStep, args("porog"))
		.add_property("returnType", &bc::BarConstructor<uchar>::returnType, make_setter(&bc::BarConstructor<uchar>::returnType))
		.add_property("createBinayMasks", &bc::BarConstructor<uchar>::createBinayMasks, make_setter(&bc::BarConstructor<uchar>::createBinayMasks))
		.add_property("createGraph", &bc::BarConstructor<uchar>::createGraph, make_setter(&bc::BarConstructor<uchar>::createGraph))
		.add_property("createNewComponentOnAttach", &bc::BarConstructor<uchar>::createGraph, make_setter(&bc::BarConstructor<uchar>::createNewComponentOnAttach))
		;
	;

	class_<bc::BarcodeCreator<uchar>>("BarcodeCreator")
		.def("createBarcode", static_cast<bc::Barcontainer<uchar> * (bc::BarcodeCreator<uchar>::*) (bn::ndarray&, bc::BarConstructor<uchar>&)>
			(&bc::BarcodeCreator<uchar>::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
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
#endif // _PYD

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

