// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include <memory>
#include <string>

#include "define_rule_monitor.hpp"

#include "ltl/label.h"
#include "ltl/rule_monitor.h"

namespace py = pybind11;
using namespace ltl;
void define_rule_monitor(py::module m) {
  py::class_<RuleMonitor, std::shared_ptr<RuleMonitor>>(m, "RuleMonitor")
      .def(py::init(&RuleMonitor::MakeRule))
      .def("MakeRule", &RuleMonitor::MakeRule)
      .def("MakeRuleState", &RuleMonitor::MakeRuleState)
      .def("__repr__",
           [](const RuleMonitor &m) {
             std::stringstream os;
             os << m;
             return os.str();
           })
      .def(py::pickle(
          [](const RuleMonitor &b) {
            return py::make_tuple(b.GetStrFormula(), b.GetWeight(),
                                  b.GetPriority());
          },
          [](py::tuple t) {
            if (t.size() != 3)
              throw std::runtime_error("Invalid RuleMonitor evaluator state!");
            return RuleMonitor::MakeRule(
                t[0].cast<std::string>(), t[1].cast<float>(),
                t[2].cast<RulePriority>());
          }));

  py::class_<RuleState, std::shared_ptr<RuleState>>(m, "RuleState")
      .def_property_readonly("violation_count", &RuleState::GetViolationCount);

  py::class_<Label, std::shared_ptr<Label>>(m, "Label")
      .def(py::init<const std::string &, int>())
      .def(py::init<const std::string &>())
      .def("__repr__", [](const Label &l) {
        std::stringstream os;
        os << l;
        return os.str();
      });
}