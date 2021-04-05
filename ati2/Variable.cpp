//
// Created by eugene on 05/04/2021.
//

#include <Variable.hpp>
#include <ValueHolder.hpp>
#include <Branch.hpp>

using namespace ATI2;

ValueHolder Variable::operator*() const {
  assert(IsInitialized());
  return parent_branch->Value(*this);
}

void Variable::Print(std::ostream &os) const {
  if (IsInitialized()) {
    os << name << "(id = " << id << ")" << std::endl;
  } else {
    os << "Variable not initialized!!!" << std::endl;
  }
}