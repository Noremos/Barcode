# import pybind11
# from pybind11 import arg, class_, return_value_policy

# import barpy as bc

# def init_barline_module(m):
#     class_<bc.barline>(m, "Barline")
#         .def("start", &bc.barline.pystart)
#         .def("len", &bc.barline.pylen)
#         .def("end", &bc.barline.pyend)
#         .def("getPoints", &bc.barline.getPoints, (arg("skipChildPoints") = False))
#         .def("getPointsSize", &bc.barline.getPointsSize)
#         .def("getMatrvalue", &bc.barline.getPoint)
#         .def("getRect", &bc.barline.getRect)
#         .def("getParent", &bc.barline.getParent, return_value_policy::reference_internal)
#         .def("getChildren", &bc.barline.getChildren)
#         .def("compire3dbars", &bc.barline.compire3dbars)
#         .def("get3dList", &bc.barline.getBarcode3d)
#         .def("get3dSize", &bc.barline.getBarcode3dSize)
#         .def("get3dValue", &bc.barline.getBarcode3dValue)

# # Create the main package
# from pybind11 import module

# # Create the main package
# m = module.Module(__name__, "Barpy")

# # Initialize the barline module
# init_barline_module(m)