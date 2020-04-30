// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "ltl/rule_state.h"

#include <utility>

namespace ltl {
RuleState::RuleState(uint32_t current_state, double rule_belief,
                     size_t violated,
                     std::shared_ptr<const RuleMonitor> automaton,
                     std::vector<int> agent_id)
    : current_state_(current_state),
      rule_belief_(rule_belief),
      violated_(violated),
      automaton_(std::move(automaton)),
      agent_ids_(std::move(agent_id)) {}
uint32_t RuleState::get_current_state() const { return current_state_; }
RulePriority RuleState::get_priority() const {
  return automaton_->get_priority();
}
double RuleState::get_rule_belief() const { return rule_belief_; }
size_t RuleState::get_violation_count() const { return violated_; }
void RuleState::reset_violations() { violated_ = 0; }
const std::shared_ptr<const RuleMonitor> &RuleState::get_automaton() const {
  return automaton_;
}
std::ostream &operator<<(std::ostream &os, const RuleState &state) {
  os << "current_state_: " << state.current_state_
     << " violated_: " << state.violated_
     << " automaton_: " << state.automaton_->get_str_formula()
     << " agent_ids: [";
  for (const auto &id : state.get_agent_ids()) {
    os << id << ", ";
  }
  os << "]";
  return os;
}
bool RuleState::is_agent_specific() const { return !agent_ids_.empty(); }
const std::vector<int> &RuleState::get_agent_ids() const { return agent_ids_; }

}  // namespace ltl
