// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef LTL_RULE_STATE_H_
#define LTL_RULE_STATE_H_

#include <memory>
#include <ostream>
#include <vector>

#include "ltl/common.h"
#include "ltl/rule_monitor.h"

namespace ltl {

class RuleMonitor;

class RuleState {
 public:
  friend class RuleMonitor;
  uint32_t GetCurrentState() const;
  RulePriority GetPriority() const;
  size_t GetViolationCount() const;
  void ResetViolations();
  const std::shared_ptr<const RuleMonitor> &GetAutomaton() const;
  bool IsAgentSpecific() const;
  const std::vector<int> &GetAgentIds() const;
  friend std::ostream &operator<<(std::ostream &os, const RuleState &state);

 private:
  RuleState(uint32_t current_state, size_t violated,
            std::shared_ptr<const RuleMonitor> automaton,
            std::vector<int> agent_ids = {});
  uint32_t current_state_;
  size_t violated_;
  std::shared_ptr<const RuleMonitor> automaton_;
  std::vector<int> agent_ids_;
};

}  // namespace ltl

#endif  // LTL_RULE_STATE_H_
