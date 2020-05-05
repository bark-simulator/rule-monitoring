// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "ltl/label.h"

namespace ltl {
Label::Label(const std::string &label_str, int agent_id)
    : label_str_(label_str), agent_id_(agent_id), is_agent_specific_(true) {}
Label::Label(const std::string &label_str)
    : label_str_(label_str), agent_id_(-1), is_agent_specific_(false) {}
const std::string &Label::GetLabelStr() const { return label_str_; }
int Label::GetAgentId() const { return agent_id_; }
bool Label::IsAgentSpecific() const { return is_agent_specific_; }
bool Label::operator==(const Label &rhs) const {
    bool equal = label_str_ == rhs.label_str_;
    equal &= is_agent_specific_ == rhs.is_agent_specific_;
    if (is_agent_specific_) {
        equal &= agent_id_ == rhs.agent_id_;
    }
    return equal;
}
bool Label::operator!=(const Label &rhs) const { return !(rhs == *this); }
Label::Label() {}
std::ostream &operator<<(std::ostream &os, const Label &label) {
    os << "label_str_: " << label.label_str_ << " agent_id_: " << label.agent_id_
       << " is_agent_specific_: " << label.is_agent_specific_;
    return os;
}
Label Label::MakeAlive() { return Label("alive"); }
}  // namespace ltl
