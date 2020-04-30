// Copyright (c) 2020 Klemens Esterle, Luis Gressenbuch
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#ifndef LTL_LABEL_H_
#define LTL_LABEL_H_

#include <ostream>
#include <string>

namespace ltl {

class Label {
 public:
  Label(const std::string& label_str, int agent_id);
  explicit Label(const std::string& label_str);
  Label();
  static Label make_alive();
  const std::string& get_label_str() const;
  int get_agent_id() const;
  bool is_agent_specific() const;
  bool operator==(const Label& rhs) const;
  bool operator!=(const Label& rhs) const;
  friend std::ostream& operator<<(std::ostream& os, const Label& label);

 private:
  std::string label_str_;
  int agent_id_;
  bool is_agent_specific_;
};

class LabelHash {
 public:
  size_t operator()(const Label& label) const {
    return std::hash<std::string>()(label.get_label_str());
  }
};

}  // namespace ltl

#endif  // LTL_LABEL_H_
