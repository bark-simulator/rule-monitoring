// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "python/bindings/define_rule_monitor.hpp"

namespace py = pybind11;

PYBIND11_MODULE(test_module_rule_monitor, m) {
  define_rule_monitor(m);
}