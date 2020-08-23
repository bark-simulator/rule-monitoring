// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "bark/world/evaluation/ltl/label/label.h"
#include "bark/world/evaluation/ltl/label_functions/constant_label_function.hpp"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "gtest/gtest.h"
#include "ltl/rule_monitor.h"

using namespace ltl;
using namespace bark::world::evaluation;
using RuleMonitorSPtr = RuleMonitor::RuleMonitorSPtr;

TEST(AutomatonTest, safety) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("G label", -1.0f, 0);
  auto lf = std::make_shared<ConstantLabelFunction>("label");
  EvaluationMap labels(nullptr, {lf});
  RuleState state = aut->MakeRuleState()[0];
  double res = state.GetAutomaton()->Evaluate(&labels, state);
  ASSERT_EQ(0.0, res);
  lf->SetValue(false);
  labels.Clear();
  res = state.GetAutomaton()->Evaluate(&labels, state);
  ASSERT_EQ(-1.0, res);
}

TEST(AutomatonTest, guarantee) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("F label", -1.0f, 0);
  RuleState state = aut->MakeRuleState()[0];
  auto lf = std::make_shared<ConstantLabelFunction>("label");
  EvaluationMap labels(nullptr, {lf});
  lf->SetValue(false);
  labels.Clear();
  ASSERT_EQ(state.GetAutomaton()->Evaluate(&labels, state), 0.0);
  ASSERT_EQ(state.GetAutomaton()->Evaluate(&labels, state), 0.0);
  ASSERT_EQ(state.GetAutomaton()->FinalTransit(state), -1.0);
  lf->SetValue(true);
  labels.Clear();
  ASSERT_EQ(state.GetAutomaton()->Evaluate(&labels, state), 0.0);
  ASSERT_EQ(state.GetAutomaton()->FinalTransit(state), 0.0);
}

TEST(AutomatonTest, parse_agent) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("G agent#0", -1.0f, 0);
  aut = RuleMonitor::MakeRule("G agent_1_test#0", -1.0f, 0);
  aut = RuleMonitor::MakeRule("G agent_1_test#0 & agent2#1 & env", -1.0f, 0);
  // TODO: add checks
}

TEST(AutomatonTest, agent_specific_rule_state) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("G (a#0 & b#1)", -1.0f, 0);
  auto rule_states = aut->MakeRuleState({1, 2});
  EXPECT_EQ(2, rule_states.size());
  EXPECT_EQ(1, rule_states[0].GetAgentIds()[0]);
  EXPECT_EQ(2, rule_states[0].GetAgentIds()[1]);
  EXPECT_EQ(2, rule_states[1].GetAgentIds()[0]);
  EXPECT_EQ(1, rule_states[1].GetAgentIds()[1]);

  aut = RuleMonitor::MakeRule("G a", -1.0f, 0);
  rule_states = aut->MakeRuleState();
  EXPECT_EQ(1, rule_states.size());
  EXPECT_FALSE(rule_states[0].IsAgentSpecific());
}

TEST(AutomatonTest, agent_specific_rule_transition) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("G label#0", -1.0f, 0);
    auto lf = std::make_shared<ConstantLabelFunction>("label");
  EvaluationMap labels(nullptr, {lf});
  RuleState state = aut->MakeRuleState({1})[0];
  double res = state.GetAutomaton()->Evaluate(&labels, state);
  ASSERT_EQ(0.0, res);
  lf->SetValue(false);
  labels.Clear();
  res = state.GetAutomaton()->Evaluate(&labels, state);
  ASSERT_EQ(-1.0, res);
}

TEST(AutomatonTest, undefined_label) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("G label", -1.0f, 0);
  EvaluationMap labels(nullptr, {});
  // The rule would need this label
  // labels.insert({Label("label"), true});
  RuleState state = aut->MakeRuleState()[0];
  // Process should die for undefined labels
  ASSERT_DEATH({ state.GetAutomaton()->Evaluate(&labels, state); },
               "Rule \"G label\" undefined! Aborting!");
}

TEST(AutomatonTest, persistence) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("F (G a)", -1.0f, 0);
  RuleState state = aut->MakeRuleState()[0];
  auto lf = std::make_shared<ConstantLabelFunction>("a");
  EvaluationMap labels(nullptr, {lf});
  lf->SetValue(false);
  labels.Clear();
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&labels, state));
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&labels, state));
  ASSERT_EQ(-1.0f, state.GetAutomaton()->FinalTransit(state));
  lf->SetValue(true);
  labels.Clear();
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&labels, state));
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&labels, state));
  ASSERT_EQ(0.0f, state.GetAutomaton()->FinalTransit(state));
  lf->SetValue(false);
  labels.Clear();
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&labels, state));
  ASSERT_EQ(-1.0f, state.GetAutomaton()->FinalTransit(state));
}

TEST(AutomatonTest, boolean_constants) {
  RuleMonitorSPtr aut = RuleMonitor::MakeRule("G true", -1.0f, 0);
  RuleState state = aut->MakeRuleState()[0];
  EvaluationMap map(nullptr, {});
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&map, state));

  aut = RuleMonitor::MakeRule("G !true", -1.0f, 0);
  state = aut->MakeRuleState()[0];
  ASSERT_EQ(-1.0f, state.GetAutomaton()->Evaluate(&map, state));

  aut = RuleMonitor::MakeRule("G !false", -1.0f, 0);
  state = aut->MakeRuleState()[0];
  ASSERT_EQ(0.0f, state.GetAutomaton()->Evaluate(&map, state));

  aut = RuleMonitor::MakeRule("G false", -1.0f, 0);
  state = aut->MakeRuleState()[0];
  ASSERT_EQ(-1.0f, state.GetAutomaton()->Evaluate(&map, state));
}

int main(int argc, char** argv) {
  google::AllowCommandLineReparsing();
  google::ParseCommandLineFlags(&argc, &argv, false);
  google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest(&argc, argv);
  FLAGS_logtostderr = true;
  return RUN_ALL_TESTS();
}