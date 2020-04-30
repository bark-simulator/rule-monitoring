// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef PYTHON_PYTHON_BINDINGS_COMMON_HPP_
#define PYTHON_PYTHON_BINDINGS_COMMON_HPP_

#include "pybind11/complex.h"
#include "pybind11/eigen.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

#endif  // PYTHON_PYTHON_BINDINGS_COMMON_HPP_
