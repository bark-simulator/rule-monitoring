// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "ltl/rule_monitor.h"

using namespace ltl;
using RuleMonitorSPtr = RuleMonitor::RuleMonitorSPtr;

class ZipperMergeFormula : public testing::TestWithParam<std::string> {};

/// Return true on violation
/// \param rs Current rule state
/// \param labels Input labels
/// \return violated?
inline bool check_violation(RuleState *rs, const EvaluationMap &labels) {
  float res = rs->GetAutomaton()->Evaluate(labels, *rs);
  return (res != 0.0);
}

inline bool check_final(const RuleState &rs) {
  float res = rs.GetAutomaton()->GetFinalReward(rs);
  return (res != 0.0);
}

// Ego violated
TEST_P(ZipperMergeFormula, false_alternation_l) {
  RuleMonitorSPtr rule = RuleMonitor::MakeRule(GetParam(), -1.0, 0);
  auto rs = rule->MakeRuleState()[0];
  EvaluationMap labels;
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = false;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = true;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_TRUE(check_violation(&rs, labels) || check_final(rs));
}

// Correct alternation, beginning ego lane
TEST_P(ZipperMergeFormula, true_alternation_l) {
  RuleMonitorSPtr rule = RuleMonitor::MakeRule(GetParam(), -1.0, 0);
  auto rs = rule->MakeRuleState()[0];
  EvaluationMap labels;
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = false;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = true;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels) || check_final(rs));
}

// Correct alternation, but other merged in early
TEST_P(ZipperMergeFormula, true_early_merge) {
  RuleMonitorSPtr rule = RuleMonitor::MakeRule(GetParam(), -1.0, 0);
  auto rs = rule->MakeRuleState()[0];
  EvaluationMap labels;
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = false;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = false;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = true;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels) || check_final(rs));
}

// Wrong alternation, ego comes from an ending lane
TEST_P(ZipperMergeFormula, false_merge_with_lane_change) {
  RuleMonitorSPtr rule = RuleMonitor::MakeRule(GetParam(), -1.0, 0);
  auto rs = rule->MakeRuleState()[0];
  EvaluationMap labels;
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = false;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = false;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = true;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = false;
  EXPECT_FALSE(check_violation(&rs, labels));
  labels[Label("merged_e")] = true;
  labels[Label("merged_x")] = true;
  labels[Label("in_direct_front_x")] = true;
  EXPECT_TRUE(check_violation(&rs, labels) || check_final(rs));
}

const std::string formulas[] = {
};

INSTANTIATE_TEST_CASE_P(ZipperMergeFormulaTest, ZipperMergeFormula,
                        testing::ValuesIn(formulas));

int main(int argc, char **argv) {
  google::AllowCommandLineReparsing();
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}