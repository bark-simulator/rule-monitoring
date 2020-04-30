// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "ltl/rule_monitor.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <numeric>
#include <regex>
#include <vector>

#include "glog/logging.h"
#include "ltl/label.h"
#include "spot/tl/apcollect.hh"
#include "spot/tl/hierarchy.hh"
#include "spot/tl/ltlf.hh"
#include "spot/tl/print.hh"
#include "spot/twa/bddprint.hh"
#include "spot/twaalgos/dot.hh"

#ifdef PROFILING
#include <easy/profiler.h>
#endif

namespace ltl {
RuleMonitor::RuleMonitor(const std::string &ltl_formula_str, float weight,
                         RulePriority priority, float init_belief,
                         float final_reward)
    : str_formula_(ltl_formula_str),
      weight_(weight),
      final_reward_(final_reward),
      priority_(priority),
      init_belief_(init_belief),
      rule_is_agent_specific_(false) {
  assert(init_belief <= 1.0 && init_belief >= 0.0);

  const std::string agent_free_formula = parse_agents(ltl_formula_str);
  ltl_formula_ = parse_formula(agent_free_formula);
  spot::translator trans;
  trans.set_pref(spot::postprocessor::Deterministic);
  trans.set_type(spot::postprocessor::BA);
  aut_ = trans.run(spot::from_ltlf(ltl_formula_));

  // If formula has the safety property, also accept empty words.
  if (spot::mp_class(ltl_formula_) == 'S') {
    // Find unique accepting state
    size_t final_state;
    for (final_state = 0; final_state < aut_->num_states(); ++final_state) {
      if (aut_->state_is_accepting(final_state)) {
        break;
      }
    }
    // Create transition from init state to final state, accepting empty words
    bdd alive_bdd = bdd_ithvar(aut_->get_dict()->has_registered_proposition(
        spot::formula::ap("alive"), aut_));
    aut_->new_edge(aut_->get_init_state_number(), final_state, !alive_bdd);
  }
  observation_prob_ << 0.9, 0.1, 0.5, 0.5;
}
std::string RuleMonitor::parse_agents(const std::string &ltl_formula_str) {
  std::string remaining = ltl_formula_str;
  std::string agent_free_formula;
  // Use #[0-9]+ as suffix of any AP to indicate agent specific rules.
  // Same numbers are instantiated with same agents.
  std::regex r("([[:lower:][:digit:]_]+)(#([[:digit:]])+)?");
  std::smatch sm;
  while (std::regex_search(remaining, sm, r)) {
    std::string ap_name = sm[1];
    bool ap_is_agent_specific = false;
    for (const auto &a : sm) {
      DVLOG(2) << a;
    }
    int agent_id_placeholder = -1;
    if (sm[3] != "") {
      agent_id_placeholder = std::stoi(sm[3]);
      rule_is_agent_specific_ = true;
      ap_is_agent_specific = true;
    }
    agent_free_formula += sm.prefix();
    agent_free_formula += ap_name;
    // TODO: Make ap_alphabet a set, so AP containers are unique
    ap_alphabet_.push_back({ap_name, spot::formula::ap(ap_name),
                            agent_id_placeholder, ap_is_agent_specific});
    remaining = sm.suffix();
  }
  ap_alphabet_.push_back({"alive", spot::formula::ap("alive"), -1, false});
  agent_free_formula += remaining;
  VLOG(2) << "Cleaned formula: " << agent_free_formula;
  return agent_free_formula;
}

std::vector<RuleState> RuleMonitor::make_rule_state(
    const std::vector<int> &new_agent_ids,
    const std::vector<int> &existing_agent_ids) const {
  int num_other_agents =
      std::max_element(ap_alphabet_.begin(), ap_alphabet_.end(),
                       [](const APContainer &a, const APContainer &b) {
                         return (a.id_idx_ < b.id_idx_);
                       })
          ->id_idx_ +
      1;
  num_other_agents = std::max(num_other_agents, 0);

  std::vector<int> current_agent_ids;
  std::set_union(new_agent_ids.begin(), new_agent_ids.end(),
                 existing_agent_ids.begin(), existing_agent_ids.end(),
                 std::back_inserter(current_agent_ids));
  std::vector<RuleState> l;
  if (is_agent_specific() && current_agent_ids.size() >= num_other_agents) {
    std::vector<std::vector<int>> existing_permutations =
        all_k_permutations(existing_agent_ids, num_other_agents);
    std::vector<std::vector<int>> all_permutations =
        all_k_permutations(current_agent_ids, num_other_agents);
    // Permutations to create
    std::vector<std::vector<int>> new_permutations;
    std::set_difference(all_permutations.begin(), all_permutations.end(),
                        existing_permutations.begin(),
                        existing_permutations.end(),
                        std::back_inserter(new_permutations));
    for (const auto &perm : new_permutations) {
      l.push_back(RuleState(aut_->get_init_state_number(), init_belief_, 0,
                            shared_from_this(), perm));
    }
  } else if (!is_agent_specific()) {
    l.push_back(RuleState(aut_->get_init_state_number(), init_belief_, 0,
                          shared_from_this(), {}));
  }
  return l;
}
std::vector<std::vector<int>> RuleMonitor::all_k_permutations(
    const std::vector<int> &values, int k) const {
  if (values.empty()) {
    return {};
  }
  std::vector<std::vector<int>> permutations;
  std::vector<int> value_permutation(k, 0);
  std::vector<int> idx_permutation(values.size());
  std::iota(idx_permutation.begin(), idx_permutation.end(), 0);
  // Create all k-permutations of the agent ids
  do {
    for (int i = 0; i < k; i++) {
      value_permutation[i] = values[idx_permutation[i]];
    }
    permutations.emplace_back(value_permutation);
    std::reverse(idx_permutation.begin() + k, idx_permutation.end());
  } while (
      std::next_permutation(idx_permutation.begin(), idx_permutation.end()));
  return permutations;
}

float RuleMonitor::evaluate(const EvaluationMap &labels,
                            RuleState &state) const {
#ifdef PROFILING
  EASY_FUNCTION();
#endif

  EvaluationMap alive_labels = labels;
  alive_labels.insert({Label::make_alive(), true});
  return transit(alive_labels, state);
}

float RuleMonitor::transit(const EvaluationMap &labels,
                           RuleState &state) const {
  std::map<int, bool> bddvars;
  spot::bdd_dict_ptr bddDictPtr = aut_->get_dict();
  for (const auto &ap : ap_alphabet_) {
    Label label;
    if (ap.is_agent_specific) {
      label = Label(ap.ap_str_, state.get_agent_ids()[ap.id_idx_]);
    } else {
      label = Label(ap.ap_str_);
    }
    auto it = labels.find(label);
    if (it != labels.end()) {
      int bdd_var = bddDictPtr->has_registered_proposition(ap.ap_, aut_);
      bddvars.insert({bdd_var, it->second});
    }
  }

  BddResult transition_found = BddResult::FALSE;
  // Indicates if we have found undefined transitions
  bool undef_trans_found = false;
  for (const auto &transition : aut_->out(state.current_state_)) {
    transition_found = evaluate_bdd(transition.cond, bddvars);
    if (transition_found == BddResult::TRUE) {
      state.current_state_ = transition.dst;
      break;
    }
    if (transition_found == BddResult::UNDEF) {
      undef_trans_found = true;
    }
  }

  float penalty = 0.0f;
  if (transition_found == BddResult::FALSE && !undef_trans_found) {
    ++state.violated_;
    // Reset automaton if rule has been violated
    state.current_state_ = aut_->get_init_state_number();
    penalty = static_cast<float>(state.rule_belief_) * weight_;
  } else if (transition_found != BddResult::TRUE && undef_trans_found) {
    VLOG(2) << "Rule" << str_formula_ << " undefined!";
  }
  return penalty;
}

RuleMonitor::BddResult RuleMonitor::evaluate_bdd(
    bdd cond, const std::map<int, bool> &vars) {
  bdd bdd_node = cond;
  while (bdd_node != bddtrue && bdd_node != bddfalse) {
    auto it = vars.find(bdd_var(bdd_node));
    if (it != vars.end()) {
      bdd_node = it->second ? bdd_high(bdd_node) : bdd_low(bdd_node);
    } else {
      // Undefined AP
      return BddResult::UNDEF;
    }
  }
  return bdd_node == bddtrue ? BddResult::TRUE : BddResult::FALSE;
}

float RuleMonitor::get_final_reward(const RuleState &state) const {
  float penalty = final_reward_;
  EvaluationMap not_alive;
  not_alive.insert({Label::make_alive(), false});
  RuleState final_state = state;
  transit(not_alive, final_state);
  if (!aut_->state_is_accepting(final_state.current_state_)) {
    penalty = weight_;
  }
  return static_cast<float>(state.rule_belief_) * penalty;
}

std::ostream &operator<<(std::ostream &os, RuleMonitor const &d) {
  os << "\"";
  spot::print_psl(os, d.ltl_formula_);
  os << "\", weight: " << d.weight_ << ", priority: " << d.priority_;
  return os;
}

void RuleMonitor::update_belief(RuleState &state) const {
  Eigen::Vector2d belief_v(state.rule_belief_, 1.0 - state.rule_belief_);
  if (!ltl_formula_.is_syntactic_safety() &&
      !aut_->state_is_accepting(state.current_state_)) {
    ++state.violated_;
  }
  if (state.violated_ > 0) {
    int observation = 1;
    double eta =
        1.0 / (observation_prob_.col(observation).transpose() * belief_v)(0);
    belief_v = eta * observation_prob_.col(observation).cwiseProduct(belief_v);
    state.rule_belief_ = belief_v(0);
  }
}

spot::formula RuleMonitor::parse_formula(const std::string &ltl_formula_str) {
  spot::parsed_formula pf = spot::parse_infix_psl(ltl_formula_str);
  if (!pf.errors.empty()) {
    pf.format_errors(LOG(FATAL));
  }
  return pf.f;
}
void RuleMonitor::set_weight(float weight) { weight_ = weight; }
void RuleMonitor::set_final_reward(float final_reward) {
  final_reward_ = final_reward;
}
void RuleMonitor::set_priority(RulePriority priority) { priority_ = priority; }
RulePriority RuleMonitor::get_priority() const { return priority_; }
bool RuleMonitor::is_agent_specific() const { return rule_is_agent_specific_; }
const std::string &RuleMonitor::get_str_formula() const { return str_formula_; }
float RuleMonitor::get_weight() const { return weight_; }
float RuleMonitor::get_final_reward1() const { return final_reward_; }
const float RuleMonitor::get_init_belief() const { return init_belief_; }
void RuleMonitor::PrintToDot(const std::string &fname) {
  std::ofstream os;
  os.open(fname);
  spot::print_dot(os, aut_);
  os.close();
}
}  // namespace ltl
