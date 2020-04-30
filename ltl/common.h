// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef LTL_COMMON_H_
#define LTL_COMMON_H_

namespace ltl {

typedef unsigned int RulePriority;
enum RewardPriority {
  SAFETY = 0,
  LEGAL_RULE,
  LEGAL_RULE_B,
  LEGAL_RULE_C,
  GOAL,
};
}  // namespace ltl

#endif  // LTL_COMMON_H_
