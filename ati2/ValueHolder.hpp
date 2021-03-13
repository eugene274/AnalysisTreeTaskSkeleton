//
// Created by eugene on 13/03/2021.
//

#ifndef ATTASKSKELETON_ATI2_VALUEHOLDER_HPP_
#define ATTASKSKELETON_ATI2_VALUEHOLDER_HPP_


#include "ATI2_fwd.hpp"
#include "Variable.hpp"

namespace ATI2 {

class ValueHolder {
 public:
  float GetVal() const;
  int GetInt() const;
  bool GetBool() const;
  void SetVal(float val) const;
  void SetVal(int int_val) const;
  void SetVal(bool bool_val) const;

  operator float() const;

  template<typename T>
  ValueHolder &operator=(T new_val) {
    SetVal(new_val);
    return *this;
  }
  ValueHolder &operator=(const ValueHolder &other);

 private:
  friend Branch;
  friend BranchChannel;

  ValueHolder(const Variable &v, void *data_ptr)
      : v(v), data_ptr(data_ptr) {}

  const Variable &v;
  void *data_ptr;
};

}
#endif //ATTASKSKELETON_ATI2_VALUEHOLDER_HPP_
