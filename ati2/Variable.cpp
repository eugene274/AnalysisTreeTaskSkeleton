//
// Created by eugene on 05/04/2021.
//

#include <Variable.hpp>
#include <ValueHolder.hpp>
#include <Branch.hpp>

using namespace ATI2;

ValueHolder Variable::operator*() const { return parent_branch->Value(*this); }

void Variable::Print(std::ostream &os) const {
  os << name << "(id = " << id << ")" << std::endl;
}