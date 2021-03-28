//#ifndef PY_SILENS
//#include "barcodeCreator.h"
//#include "include_py.h"
//#define TN(NAME) NAME
//#define TV uchar
//#endif // !#PY_SILENS
//
//
//
//
//#ifndef PY_SILENS
//BOOST_PYTHON_MODULE(barpy)
//{
//	bn::initialize();
//	Py_Initialize();
//#endif // !PY_SILENS!!!!!!!!!!!!!!!!!!
//
//
//	class_<bc::barvalue<uchar>>(TN("Matrvalue"))
//		.add_property("x", &bc::barvalue<TV>::getX, &bc::barvalue<TV>::setX)
//		.add_property("y", &bc::barvalue<TV>::getY, &bc::barvalue<TV>::setY)
//		.add_property("point", make_getter(&bc::barvalue<TV>::point), make_setter(&bc::barvalue<TV>::point))
//		.add_property("value", make_getter(&bc::barvalue<TV>::value), make_setter(&bc::barvalue<TV>::value))
//		//.add_property("points", make_getter(&bc::barline::matr))
//		;
//
//	class_<bc::bar3dvalue<TV>>("Bar3dvalue")
//		.add_property("count", make_getter(&bc::bar3dvalue<TV>::count), make_setter(&bc::bar3dvalue<TV>::count))
//		.add_property("value", make_getter(&bc::bar3dvalue<TV>::value), make_setter(&bc::bar3dvalue<TV>::value))
//		//.add_property("points", make_getter(&bc::barline::matr))
//		;
//
//	class_<bc::barline<TV>>("Barline")
//		.def(init<TV, TV>(args("start", "len")))
//		.add_property("start", make_getter(&bc::barline<TV>::start), make_setter(&bc::barline<TV>::start))
//		.add_property("len", make_getter(&bc::barline<TV>::len), make_setter(&bc::barline<TV>::len))
//		.def("end", &bc::barline<TV>::end)
//		.def("getPointsInDict", &bc::barline<TV>::getPointsInDict)
//		.def("getPoints", &bc::barline<TV>::getPoints)
//		.def("getPointsSize", &bc::barline<TV>::getPointsSize)
//		.def("getMatrvalue", &bc::barline<TV>::getPoint, args("index"))
//		.def("getRect", &bc::barline<TV>::getRect)
//		.def("getParent", &bc::barline<TV>::getParent, return_internal_reference())
//		.def("getChildren", &bc::barline<TV>::getChildren)
//
//		.def("get3dList", &bc::barline<TV>::getBarcode3d)
//		.def("get3dSize", &bc::barline<TV>::getBarcode3dSize)
//		.def("get3dValue", &bc::barline<TV>::getBarcode3dValue, args("index"))
//
//		//.add_property("points", make_getter(&bc::barline::matr))
//		;
//
//	class_<bc::Baritem<TV>>("Baritem")
//		.def("sum", &bc::Baritem<TV>::sum)
//		.def("relen", &bc::Baritem<TV>::relen)
//		.def("clone", &bc::Baritem<TV>::clone, return_value_policy< manage_new_object >())
//		.def("maxLen", &bc::Baritem<TV>::maxLen)
//		.def("removePorog", &bc::Baritem<TV>::removePorog, args("porog"))
//		.def("preprocessBar", &bc::Baritem<TV>::preprocessBar, args("porog", "normalize"))
//		.def("cmp", &bc::Baritem<TV>::cmp, args("bitem", "compireStrategy"))
//		.def("cmpOccurrence", &bc::Baritem<TV>::compareOccurrence, args("bitem", "compireStrategy"))
//		.def("compireBestRes", &bc::Baritem<TV>::compireBestRes, args("bitem", "compireStrategy"))
//		.def("getBarcode", &bc::Baritem<TV>::getBarcode)
//		.def("SortByLineLen", &bc::Baritem<TV>::sortByLen)
//		.def("SortByPointsCount", &bc::Baritem<TV>::sortBySize)
//		.def("calcHistByBarlen", &bc::Baritem<TV>::calcHistByBarlen)
//		.def("getRootNode", &bc::Baritem<TV>::getRootNode, return_internal_reference()/*, make_setter(&bc::Baritem::rootNode)*/)
//		;
//
//	class_<bc::Barcontainer<TV>>("Barcontainer")
//		.def("sum", &bc::Barcontainer<TV>::sum)
//		.def("relen", &bc::Barcontainer<TV>::relen)
//		.def("clone", &bc::Barcontainer<TV>::clone, return_value_policy< manage_new_object >())
//		.def("maxLen", &bc::Barcontainer<TV>::maxLen)
//		.def("count", &bc::Barcontainer<TV>::count)
//		.def("removePorog", &bc::Barcontainer<TV>::removePorog, args("porog"))
//		.def("preprocessBar", &bc::Barcontainer<TV>::preprocessBar, args("porog", "normalize"))
//		//.def("compireCTML", &bc::Barcontainer::compireCTML, args("bc"))
//		//.def("compireCTS", &bc::Barcontainer::compireCTS, args("bc"))
//		//.def("compireCTML", static_cast<float (bc::Barcontainer::*)(const bc::Barbase*) const> (&bc::Barcontainer::compireCTML), args("bc"))
//		//.def("compireCTS", static_cast<float (bc::Barcontainer::*)(bc::Barbase const*) const>(&bc::Barcontainer::compireCTS), args("bc"))
//		.def("addItem", &bc::Barcontainer<TV>::addItem, args("Baritem"))
//		.def("getItem", &bc::Barcontainer<TV>::getItem, args("index"), return_internal_reference())
//		;
//
//	class_<bc::BarConstructor<TV>>("BarConstructor")
//		.def("addStructure", &bc::BarConstructor<TV>::addStructure, args("ProcType", "ColorType", "ComponentType"))
//		.def("setPorogStep", &bc::BarConstructor<TV>::setStep, args("porog"))
//		.def("setMaxLen", &bc::BarConstructor<TV>::setMaxLen, args("len"))
//		.add_property("returnType", &bc::BarConstructor<TV>::returnType, make_setter(&bc::BarConstructor<TV>::returnType))
//		.add_property("createBinaryMasks", &bc::BarConstructor<TV>::createBinayMasks, make_setter(&bc::BarConstructor<TV>::createBinayMasks))
//		.add_property("createGraph", &bc::BarConstructor<TV>::createGraph, make_setter(&bc::BarConstructor<TV>::createGraph))
//		.add_property("createNewComponentOnAttach", &bc::BarConstructor<TV>::createNewComponentOnAttach, make_setter(&bc::BarConstructor<TV>::createNewComponentOnAttach))
//		;
//	;
//
//	class_<bc::BarcodeCreator<TV>>("BarcodeCreator")
//		.def("createBarcode", static_cast<bc::Barcontainer<TV>*(bc::BarcodeCreator<TV>::*) (bn::ndarray&, bc::BarConstructor<TV>&)>
//			(&bc::BarcodeCreator<TV>::createBarcode), args("image", "structure"), return_value_policy< manage_new_object >())
//		;
//	;
//#ifndef PY_SILENS
//}
//#endif // !PY_SILENS
