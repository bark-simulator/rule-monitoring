// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef PYTHON_BINDINGS_DEFINE_RULE_MONITOR_HPP_
#define PYTHON_BINDINGS_DEFINE_RULE_MONITOR_HPP_

#include "python/bindings/common.hpp"

namespace py = pybind11;

void define_rule_monitor(py::module m);

#endif  // PYTHON_BINDINGS_DEFINE_RULE_MONITOR_HPP_
