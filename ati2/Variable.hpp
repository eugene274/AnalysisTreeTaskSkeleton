//
// Created by eugene on 13/03/2021.
//

#ifndef ATTASKSKELETON_ATI2_VARIABLE_HPP_
#define ATTASKSKELETON_ATI2_VARIABLE_HPP_

#include "ATI2_fwd.hpp"

#include <string>
#include <AnalysisTree/Constants.hpp>

namespace ATI2 {

class Variable {

 public:
  ValueHolder operator*() const;
  void Print(std::ostream &os = std::cout) const;

  Branch *GetParentBranch() const {
    return parent_branch;
  }
  int GetId() const {
    return id;
  }
  AnalysisTree::Types GetFieldType() const {
    return field_type;
  }
  const std::string &GetName() const {
    return name;
  }
  const std::string &GetFieldName() const {
    return field_name;
  }
  bool IsInitialized() const {
    return is_initialized;
  }
  explicit operator bool() const {
    return IsInitialized();
  }
 private:
//  Variable() = default;
  friend Branch;

  Branch *parent_branch{nullptr};

  bool is_initialized{false};
  int id{0};
  AnalysisTree::Types field_type{AnalysisTree::Types::kNumberOfTypes};
  std::string name;
  std::string field_name;
};


}

#endif //ATTASKSKELETON_ATI2_VARIABLE_HPP_
