// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef LTL_RULE_MONITOR_H_
#define LTL_RULE_MONITOR_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Eigen/Core"
#include "ltl/common.h"
#include "ltl/label.h"
#include "ltl/rule_state.h"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/translate.hh"

namespace ltl {

typedef std::unordered_map<Label, bool, LabelHash> EvaluationMap;

class RuleState;

class RuleMonitor : public std::enable_shared_from_this<RuleMonitor> {
 public:
    typedef std::shared_ptr<RuleMonitor> RuleMonitorSPtr;

    static RuleMonitorSPtr MakeRule(std::string ltl_formula_str, float weight,
                                    RulePriority priority,
                                    float final_reward = 0.0f) {
        return RuleMonitorSPtr(
            new RuleMonitor(ltl_formula_str, weight, priority, final_reward));
    }

    std::vector<RuleState> MakeRuleState(
        const std::vector<int> &current_agent_ids = {},
        const std::vector<int> &existing_agent_ids = {}) const;

    float Evaluate(EvaluationMap const &labels, RuleState &state) const;

    float GetFinalReward(const RuleState &state) const;

    RulePriority GetPriority() const;

    void SetWeight(float weight);

    void SetFinalReward(float final_reward);

    void SetPriority(RulePriority priority);

    bool IsAgentSpecific() const;

    friend std::ostream &operator<<(std::ostream &os, RuleMonitor const &d);
    const std::string &GetStrFormula() const;
    float GetWeight() const;
    float GetFinalReward() const;
    void PrintToDot(const std::string &fname);

 private:
    enum BddResult { TRUE, FALSE, UNDEF };

    static spot::formula ParseFormula(const std::string &ltl_formula_str);
    static BddResult EvaluateBdd(bdd cond, const std::map<int, bool> &vars);

    RuleMonitor(const std::string &ltl_formula_str, float weight,
                RulePriority priority, float final_reward = 0.0f);
    std::string ParseAgents(const std::string &ltl_formula_str);
    std::vector<std::vector<int>> AllKPermutations(
        const std::vector<int> &values, int k) const;
    float Transit(const EvaluationMap &labels, RuleState &state) const;

    struct APContainer {
        std::string ap_str;
        spot::formula ap;
        int id_idx;
        bool is_agent_specific;
    };

    std::string str_formula_;
    float weight_;
    float final_reward_;
    spot::twa_graph_ptr aut_;
    spot::formula ltl_formula_;
    RulePriority priority_;
    Eigen::Matrix2d observation_prob_;
    std::vector<APContainer> ap_alphabet_;
    bool rule_is_agent_specific_;
};
}  // namespace ltl

#endif  // LTL_RULE_MONITOR_H_
