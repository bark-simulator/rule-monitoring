// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "ltl/label.h"
#include "ltl/rule_monitor.h"

using namespace ltl;
using RuleMonitorSPtr = RuleMonitor::RuleMonitorSPtr;

enum RewardPriority {
  SAFETY = 0,
  LEGAL_RULE,
  LEGAL_RULE_B,
  LEGAL_RULE_C,
  GOAL,
};

TEST(AutomatonTest, simple) {
  RuleMonitorSPtr aut =
      RuleMonitor::make_rule("G label", -1.0f, RewardPriority::SAFETY);
  EvaluationMap labels;
  labels.insert({Label("label"), true});
  RuleState state = aut->make_rule_state()[0];
  double res = state.get_automaton()->evaluate(labels, state);
  ASSERT_EQ(0.0, res);
  labels.clear();
  labels.insert({Label("label"), false});
  res = state.get_automaton()->evaluate(labels, state);
  ASSERT_EQ(-1.0, res);
}

TEST(AutomatonTest, liveness) {
  RuleMonitorSPtr aut =
      RuleMonitor::make_rule("F label", -1.0f, RewardPriority::SAFETY);
  EvaluationMap labels;
  labels.insert({Label("label"), false});
  RuleState state = aut->make_rule_state()[0];
  ASSERT_EQ(state.get_automaton()->evaluate(labels, state), 0.0);
  ASSERT_EQ(state.get_automaton()->evaluate(labels, state), 0.0);
  ASSERT_EQ(state.get_automaton()->get_final_reward(state), -1.0);
  labels.clear();
  labels.insert({Label("label"), true});
  ASSERT_EQ(state.get_automaton()->evaluate(labels, state), 0.0);
  ASSERT_EQ(state.get_automaton()->get_final_reward(state), 0.0);
}

TEST(AutomatonTest, parse_agent) {
  RuleMonitorSPtr aut =
      RuleMonitor::make_rule("G agent#0", -1.0f, RewardPriority::SAFETY);
  aut =
      RuleMonitor::make_rule("G agent_1_test#0", -1.0f, RewardPriority::SAFETY);
  aut = RuleMonitor::make_rule("G agent_1_test#0 & agent2#1 & env", -1.0f,
                               RewardPriority::SAFETY);
  // TODO: add checks
}

TEST(AutomatonTest, agent_specific_rule_state) {
  RuleMonitorSPtr aut = RuleMonitor::make_rule("G (a#0 & b#1)", -1.0f, 0);
  auto rule_states = aut->make_rule_state({1, 2});
  EXPECT_EQ(2, rule_states.size());
  EXPECT_EQ(1, rule_states[0].get_agent_ids()[0]);
  EXPECT_EQ(2, rule_states[0].get_agent_ids()[1]);
  EXPECT_EQ(2, rule_states[1].get_agent_ids()[0]);
  EXPECT_EQ(1, rule_states[1].get_agent_ids()[1]);

  aut = RuleMonitor::make_rule("G a", -1.0f, 0);
  rule_states = aut->make_rule_state();
  EXPECT_EQ(1, rule_states.size());
  EXPECT_FALSE(rule_states[0].is_agent_specific());
}

TEST(AutomatonTest, agent_specific_rule_transition) {
  RuleMonitorSPtr aut =
      RuleMonitor::make_rule("G label#0", -1.0f, RewardPriority::SAFETY);
  EvaluationMap labels;
  labels.insert({Label("label", 1), true});
  RuleState state = aut->make_rule_state({1})[0];
  double res = state.get_automaton()->evaluate(labels, state);
  ASSERT_EQ(0.0, res);
  labels.clear();
  labels.insert({Label("label", 1), false});
  res = state.get_automaton()->evaluate(labels, state);
  ASSERT_EQ(-1.0, res);
}

int main(int argc, char **argv) {
  google::AllowCommandLineReparsing();
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  FLAGS_logtostderr = true;
  return RUN_ALL_TESTS();
}