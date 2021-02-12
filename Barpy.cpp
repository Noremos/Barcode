#ifdef _PYD
INCLUDE_PY

#include "barcodeCreator.h"

//namespace python = boost::python;

template< typename T >
struct type_from_python
{
	type_from_python()
	{
		converter::registry::push_back(convertible, construct, type_id<T>());
	}

	static void* convertible(PyObject*)
	{

	}
	static void construct(PyObject*, converter::rvalue_from_python_stage1_data*)
	{

	}
};
 
// to pytion
template< typename T >
struct type_into_python
{
	static PyObject* convert(T const&);
};

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

	class_<bc::pointHash>("PointHash")
		;
	class_<pmap>("Pmap")
		;
	class_<ppair>("Ppair")
		;
	class_<std::vector<bc::bline>>("LinesList")
		;

	class_<bc::bline>("BLine")
		.def(init<>())
		.def(init<uchar, uchar>(args("start", "len")))
		.def(init<uchar, uchar, pmap*>(args("start", "len", "pmap")))
		.add_property("start", make_getter(&bc::bline::start), make_setter(&bc::bline::start))
		.add_property("len", make_getter(&bc::bline::len), make_setter(&bc::bline::len))
		.def("getPoints", &bc::bline::getPoints)
		//.add_property("points", make_getter(&bc::bline::matr))
		;


	class_<bc::Baritem>("Baritem")
		.def("add", static_cast<void (bc::Baritem::*)(uchar, uchar)> (&bc::Baritem::add), args("start", "len"))
		.def("add", static_cast<void (bc::Baritem::*)(uchar, uchar, pmap*)> (&bc::Baritem::add), args("start", "len", "points"))
		.def("sum", &bc::Baritem::sum)
		.def("relen", &bc::Baritem::relen)
		.def("clone", &bc::Baritem::clone, return_value_policy< manage_new_object >())
		.def("maxLen", &bc::Baritem::maxLen)
		.def("removePorog", &bc::Baritem::removePorog, args("porog"))
		.def("preprocessBar", &bc::Baritem::preprocessBar, args("porog", "normalize"))
		//.def("compireCTML", &bc::Baritem::compireCTML, args("bc"))
		//.def("compireCTS", &bc::Baritem::compireCTS, args("bc"))
		.def("getBar", &bc::Baritem::getBar)

		//.add_property("bar", make_getter(&bc::Baritem::bar))
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
		.def("cmpCTML", &bc::Barcontainer::cmpCTML, args("bc"))
		.def("cmpCTS", &bc::Barcontainer::cmpCTS, args("bc"))
		.def("addItem", &bc::Barcontainer::addItem, args("Baritem"))
		.def("get", &bc::Barcontainer::get, args("index"), return_value_policy< reference_existing_object>())
		;

	enum_<bc::CompireFunction>("CompireFunction")
		.value("CommonToLen", bc::CompireFunction::CommonToLen)
		.value("CommonToSum", bc::CompireFunction::CommonToSum)
		;

	enum_<bc::ComponentType>("ComponentType")
		.value("Component", bc::ComponentType::Component)
		.value("Hole", bc::ComponentType::Hole)
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

	class_<bc::barstruct>("Barstruct")
		.def(init())
		.def(init< bc::ProcType, bc::ColorType, bc::ComponentType>(args("ProcType", "ColorType", "ComponentType")))
		.add_property("comType", &bc::barstruct::comtype)
		.add_property("procType", &bc::barstruct::proctype)
		.add_property("colType", &bc::barstruct::coltype);


	class_<bc::barcodeCreator>("BarcodeCreator")
		.def("setVisualize", &bc::barcodeCreator::setVisualize, args("value"))
		.def("setCreateBinaryMasks", &bc::barcodeCreator::setCreateBinaryMasks, args("value"))
		.def("setReturnType", &bc::barcodeCreator::setReturnType, args("value"))

		.def("createBarcode", static_cast<bc::Barcontainer * (bc::barcodeCreator::*) (bn::ndarray&, bp::list&)>
			(&bc::barcodeCreator::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
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
//PyObject* type_into_python<std::vector<bc::bline>>::convert(std::vector<bc::bline> const& bar)
//{
//	bp::object get_iter = bp::iterator<std::vector<bc::bline> >();
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

