// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "ltl/rule_state.h"

#include <utility>

namespace ltl {
RuleState::RuleState(uint32_t current_state,
                     size_t violated,
                     std::shared_ptr<const RuleMonitor> automaton,
                     std::vector<int> agent_id)
    : current_state_(current_state),
      violated_(violated),
      automaton_(std::move(automaton)),
      agent_ids_(std::move(agent_id)) {}
uint32_t RuleState::GetCurrentState() const { return current_state_; }
RulePriority RuleState::GetPriority() const {
  return automaton_->GetPriority();
}
size_t RuleState::GetViolationCount() const { return violated_; }
void RuleState::ResetViolations() { violated_ = 0; }
const std::shared_ptr<const RuleMonitor> &RuleState::GetAutomaton() const {
  return automaton_;
}
std::ostream &operator<<(std::ostream &os, const RuleState &state) {
  os << "current_state_: " << state.current_state_
     << " violated_: " << state.violated_
     << " automaton_: " << state.automaton_->GetStrFormula()
     << " agent_ids: [";
  for (const auto &id : state.GetAgentIds()) {
    os << id << ", ";
  }
  os << "]";
  return os;
}
bool RuleState::IsAgentSpecific() const { return !agent_ids_.empty(); }
const std::vector<int> &RuleState::GetAgentIds() const { return agent_ids_; }

}  // namespace ltl
