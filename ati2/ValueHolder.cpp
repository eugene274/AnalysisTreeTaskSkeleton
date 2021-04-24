//
// Created by eugene on 13/03/2021.
//


#include "ValueHolder.hpp"
#include "BranchChannel.hpp"
#include "Branch.hpp"

#include "ATI2_ATHelper.hpp"

#include <AnalysisTree/Particle.hpp>
#include <AnalysisTree/Track.hpp>
#include <AnalysisTree/Module.hpp>
#include <AnalysisTree/Hit.hpp>
#include <cassert>

using namespace ATI2;

float ValueHolder::GetVal() const {
  return entity_->GetFloat(v);
}

int ValueHolder::GetInt() const {
  return entity_->GetInt(v);
}

bool ValueHolder::GetBool() const {
  return entity_->GetBool(v);
}
ValueHolder::operator float() const {
  return GetVal();
}
void ValueHolder::SetVal(float val) const {
  v.GetParentBranch()->CheckMutable(true);
  entity_->Set(v, val);
}
void ValueHolder::SetVal(int val) const {
  v.GetParentBranch()->CheckMutable(true);
  entity_->Set(v, val);
}
void ValueHolder::SetVal(bool val) const {
  v.GetParentBranch()->CheckMutable(true);
  entity_->Set(v, val);
}

ValueHolder &ValueHolder::operator=(const ValueHolder &other) {
  if (this == &other) {
    return *this;
  }

  v.GetParentBranch()->CheckMutable(true);

  using AnalysisTree::Types;
  if (other.v.GetFieldType() == Types::kFloat) {
    this->SetVal(other.GetVal());
  } else if (other.v.GetFieldType() == Types::kInteger) {
    this->SetVal(other.GetInt());
  } else if (other.v.GetFieldType() == Types::kBool) {
    this->SetVal(other.GetBool());
  } else {
    /* unreachable */
    assert(false);
  }

  return *this;
}
