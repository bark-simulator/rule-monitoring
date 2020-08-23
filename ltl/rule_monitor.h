// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef LTL_RULE_MONITOR_H_
#define LTL_RULE_MONITOR_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Eigen/Core"
#include "bark/world/evaluation/ltl/label/label.h"
#include "bark/world/evaluation/ltl/label/evaluation_map.hpp"
#include "ltl/common.h"
#include "ltl/rule_state.h"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"

namespace ltl {
using bark::world::evaluation::EvaluationMap;
using bark::world::evaluation::Label;

class RuleState;

class RuleMonitor : public std::enable_shared_from_this<RuleMonitor> {
 public:
  typedef std::shared_ptr<RuleMonitor> RuleMonitorSPtr;

  static RuleMonitorSPtr MakeRule(std::string ltl_formula_str, float weight,
                                  RulePriority priority) {
    return RuleMonitorSPtr(new RuleMonitor(ltl_formula_str, weight, priority));
  }

  std::vector<RuleState> MakeRuleState(
      const std::vector<int>& current_agent_ids = {},
      const std::vector<int>& existing_agent_ids = {}) const;

  float Evaluate(EvaluationMap* labels, RuleState& state) const;

  float FinalTransit(const RuleState& state) const;

  RulePriority GetPriority() const;

  bool IsAgentSpecific() const;

  friend std::ostream& operator<<(std::ostream& os, RuleMonitor const& d);
  const std::string& GetStrFormula() const;
  float GetWeight() const;
  void PrintToDot(const std::string& fname);

 private:
  enum BddResult { TRUE, FALSE, UNDEF };

  static spot::formula ParseFormula(const std::string& ltl_formula_str);
  static BddResult EvaluateBdd(bdd cond, const std::map<int, Label>& vars, EvaluationMap* value_map);

  RuleMonitor(const std::string& ltl_formula_str, float weight,
              RulePriority priority);
  std::string ParseAgents(const std::string& ltl_formula_str);
  std::vector<std::vector<int>> AllKPermutations(const std::vector<int>& values,
                                                 int k) const;
  float Transit(EvaluationMap* labels, RuleState& state) const;

  struct APContainer {
    bool operator==(const APContainer& rhs) const;
    bool operator!=(const APContainer& rhs) const;
    std::string ap_str;
    spot::formula ap;
    int placeholder_idx;
    bool is_agent_specific;
  };

  struct APContainerHash {
   public:
    size_t operator()(const APContainer& apcont) const {
      return std::hash<std::string>()(apcont.ap_str);
    }
  };

  std::string str_formula_;
  float weight_;
  spot::twa_graph_ptr aut_;
  spot::formula ltl_formula_;
  RulePriority priority_;
  std::unordered_set<APContainer, APContainerHash> ap_alphabet_;
  bool rule_is_agent_specific_;
};
}  // namespace ltl

#endif  // LTL_RULE_MONITOR_H_
