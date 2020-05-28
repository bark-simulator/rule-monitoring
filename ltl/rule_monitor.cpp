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
                         RulePriority priority)
    : str_formula_(ltl_formula_str),
      weight_(weight),
      priority_(priority),
      rule_is_agent_specific_(false) {

    const std::string agent_free_formula = ParseAgents(ltl_formula_str);
    ltl_formula_ = ParseFormula(agent_free_formula);
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
}
std::string RuleMonitor::ParseAgents(const std::string &ltl_formula_str) {
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
        ap_alphabet_.insert({ap_name, spot::formula::ap(ap_name),
                                agent_id_placeholder, ap_is_agent_specific});
        remaining = sm.suffix();
    }
    ap_alphabet_.insert({"alive", spot::formula::ap("alive"), -1, false});
    agent_free_formula += remaining;
    VLOG(2) << "Cleaned formula: " << agent_free_formula;
    return agent_free_formula;
}

std::vector<RuleState> RuleMonitor::MakeRuleState(
    const std::vector<int> &new_agent_ids,
    const std::vector<int> &existing_agent_ids) const {
    int num_other_agents =
        std::max_element(ap_alphabet_.begin(), ap_alphabet_.end(),
                         [](const APContainer &a, const APContainer &b) {
                             return (a.placeholder_idx < b.placeholder_idx);
                         })
            ->placeholder_idx +
            1;
    num_other_agents = std::max(num_other_agents, 0);

    std::vector<int> current_agent_ids;
    std::set_union(new_agent_ids.begin(), new_agent_ids.end(),
                   existing_agent_ids.begin(), existing_agent_ids.end(),
                   std::back_inserter(current_agent_ids));
    std::vector<RuleState> l;
    if (IsAgentSpecific() && current_agent_ids.size() >= num_other_agents) {
        std::vector<std::vector<int>> existing_permutations =
            AllKPermutations(existing_agent_ids, num_other_agents);
        std::vector<std::vector<int>> all_permutations =
            AllKPermutations(current_agent_ids, num_other_agents);
        // Permutations to create
        std::vector<std::vector<int>> new_permutations;
        std::set_difference(all_permutations.begin(), all_permutations.end(),
                            existing_permutations.begin(),
                            existing_permutations.end(),
                            std::back_inserter(new_permutations));
        for (const auto &perm : new_permutations) {
            l.push_back(RuleState(aut_->get_init_state_number(), 0,
                                  shared_from_this(), perm));
        }
    } else if (!IsAgentSpecific()) {
        l.push_back(RuleState(aut_->get_init_state_number(), 0,
                              shared_from_this(), {}));
    }
    return l;
}
std::vector<std::vector<int>> RuleMonitor::AllKPermutations(
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

float RuleMonitor::Evaluate(const EvaluationMap &labels,
                            RuleState &state) const {
#ifdef PROFILING
    EASY_FUNCTION();
#endif

    EvaluationMap alive_labels = labels;
    alive_labels.insert({Label::MakeAlive(), true});
    return Transit(alive_labels, state);
}

float RuleMonitor::Transit(const EvaluationMap &labels,
                           RuleState &state) const {
    std::map<int, bool> bddvars;
    spot::bdd_dict_ptr bddDictPtr = aut_->get_dict();
    for (const auto &ap : ap_alphabet_) {
        Label label;
        if (ap.is_agent_specific) {
            label = Label(ap.ap_str, state.GetAgentIds()[ap.placeholder_idx]);
        } else {
            label = Label(ap.ap_str);
        }
        auto it = labels.find(label);
        if (it != labels.end()) {
            int bdd_var = bddDictPtr->has_registered_proposition(ap.ap, aut_);
            bddvars.insert({bdd_var, it->second});
        } else if(labels.at(Label::MakeAlive())) {
            // We ware alive but the label is undefined
            LOG(FATAL) << "Rule " << str_formula_ << " undefined! Missing label \"" << ap.ap_str <<"\"! Aborting!";
        }
    }

    BddResult transition_found = BddResult::FALSE;
    // Indicates if we have found undefined transitions
    bool undef_trans_found = false;
    for (const auto &transition : aut_->out(state.current_state_)) {
        transition_found = EvaluateBdd(transition.cond, bddvars);
        if (transition_found == BddResult::TRUE) {
            state.current_state_ = transition.dst;
            break;
        }
        if (transition_found == BddResult::UNDEF) {
            undef_trans_found = true;
        }
    }

    float penalty = 0.0f;
    if ((transition_found == BddResult::FALSE && !undef_trans_found)
        || (transition_found != BddResult::TRUE && !labels.at(Label::MakeAlive()))) {
        ++state.violated_;
        // Reset automaton if rule has been violated
        state.current_state_ = aut_->get_init_state_number();
        penalty = weight_;
    } else if (transition_found != BddResult::TRUE && undef_trans_found) {
        LOG(FATAL) << "Rule " << str_formula_ << " undefined!";
    }
    return penalty;
}

RuleMonitor::BddResult RuleMonitor::EvaluateBdd(
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

float RuleMonitor::FinalTransit(const RuleState &state) const {
    float penalty = 0.0f;
    EvaluationMap not_alive;
    not_alive.insert({Label::MakeAlive(), false});
    RuleState final_state = state;
    Transit(not_alive, final_state);
    if (!aut_->state_is_accepting(final_state.current_state_)) {
        penalty = weight_;
    }
    return penalty;
}

std::ostream &operator<<(std::ostream &os, RuleMonitor const &d) {
    os << "\"";
    spot::print_psl(os, d.ltl_formula_);
    os << "\", weight: " << d.weight_ << ", priority: " << d.priority_;
    return os;
}

spot::formula RuleMonitor::ParseFormula(const std::string &ltl_formula_str) {
    spot::parsed_formula pf = spot::parse_infix_psl(ltl_formula_str);
    if (!pf.errors.empty()) {
        pf.format_errors(LOG(FATAL));
    }
    return pf.f;
}

RulePriority RuleMonitor::GetPriority() const { return priority_; }
bool RuleMonitor::IsAgentSpecific() const { return rule_is_agent_specific_; }
const std::string &RuleMonitor::GetStrFormula() const { return str_formula_; }
float RuleMonitor::GetWeight() const { return weight_; }
void RuleMonitor::PrintToDot(const std::string &fname) {
    std::ofstream os;
    os.open(fname);
    spot::print_dot(os, aut_);
    os.close();
}

bool RuleMonitor::APContainer::operator==(const RuleMonitor::APContainer &rhs) const {
    return ap_str == rhs.ap_str &&
        ap == rhs.ap &&
        placeholder_idx == rhs.placeholder_idx &&
        is_agent_specific == rhs.is_agent_specific;
}
bool RuleMonitor::APContainer::operator!=(const RuleMonitor::APContainer &rhs) const {
    return !(rhs == *this);
}
}  // namespace ltl
