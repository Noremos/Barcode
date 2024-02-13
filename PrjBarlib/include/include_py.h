#ifndef SKIP_M_INC

#pragma once

#ifdef INCLUDE_PY
//#include "boost/python.hpp"
//#include "boost/python/numpy.hpp"
//using namespace boost::python;
//namespace bp = boost::python;
//namespace bn = boost::python::numpy;

#include <pybind11/pybind11.h>
#include <pybind11/embed.h> // <= You need this header
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;
namespace bn = pybind11;
namespace bp = pybind11;

#endif

#endif
