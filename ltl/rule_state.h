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
  uint32_t get_current_state() const;
  RulePriority get_priority() const;
  size_t get_violation_count() const;
  void reset_violations();
  const std::shared_ptr<const RuleMonitor> &get_automaton() const;
  bool is_agent_specific() const;
  const std::vector<int> &get_agent_ids() const;
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
