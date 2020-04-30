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
#include "ltl/common.h"
#include "ltl/label.h"
#include "ltl/rule_state.h"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"

namespace ltl {

typedef std::unordered_map<Label, bool, LabelHash> EvaluationMap;

class RuleState;

class RuleMonitor : public std::enable_shared_from_this<RuleMonitor> {
 public:
  typedef std::shared_ptr<RuleMonitor> RuleMonitorSPtr;

  static RuleMonitorSPtr make_rule(std::string ltl_formula_str, float weight,
                                   RulePriority priority,
                                   float init_belief = 1.0,
                                   float final_reward = 0.0f) {
    return RuleMonitorSPtr(new RuleMonitor(ltl_formula_str, weight, priority,
                                           init_belief, final_reward));
  }

  std::vector<RuleState> make_rule_state(
      const std::vector<int> &current_agent_ids = {},
      const std::vector<int> &existing_agent_ids = {}) const;

  float evaluate(EvaluationMap const &labels, RuleState &state) const;

  float get_final_reward(const RuleState &state) const;

  void update_belief(RuleState &state) const;

  RulePriority get_priority() const;

  void set_weight(float weight);

  void set_final_reward(float final_reward);

  void set_priority(RulePriority priority);

  bool is_agent_specific() const;

  friend std::ostream &operator<<(std::ostream &os, RuleMonitor const &d);
  const std::string &get_str_formula() const;
  float get_weight() const;
  float get_final_reward1() const;
  const float get_init_belief() const;
  void PrintToDot(const std::string &fname);

 private:
  enum BddResult { TRUE, FALSE, UNDEF };

  RuleMonitor(const std::string &ltl_formula_str, float weight,
              RulePriority priority, float init_belief = 1.0,
              float final_reward = 0.0f);
  static spot::formula parse_formula(const std::string &ltl_formula_str);
  static BddResult evaluate_bdd(bdd cond, const std::map<int, bool> &vars);

  std::string parse_agents(const std::string &ltl_formula_str);

  struct APContainer {
    std::string ap_str_;
    spot::formula ap_;
    int id_idx_;
    bool is_agent_specific;
  };

  std::string str_formula_;
  float weight_;
  float final_reward_;
  spot::twa_graph_ptr aut_;
  spot::formula ltl_formula_;
  RulePriority priority_;
  Eigen::Matrix2d observation_prob_;
  const float init_belief_;
  std::vector<APContainer> ap_alphabet_;
  bool rule_is_agent_specific_;
  std::vector<std::vector<int>> all_k_permutations(
      const std::vector<int> &values, int k) const;
  float transit(const EvaluationMap &labels, RuleState &state) const;
};
}  // namespace ltl

#endif  // LTL_RULE_MONITOR_H_
