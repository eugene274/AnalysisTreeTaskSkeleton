//
// Created by eugene on 24/04/2021.
//

#ifndef ATTASKSKELETON_ATI2_ENTITYWRAPPER_HPP_
#define ATTASKSKELETON_ATI2_ENTITYWRAPPER_HPP_

#include "Variable.hpp"
#include "ATI2_ATHelper.hpp"

namespace ATI2 {

struct BaseEntity {
 public:
  virtual void *RawPtr() = 0;
  virtual void Set(const Variable& v, int int_val) = 0;
  virtual void Set(const Variable& v, float float_val) = 0;
  virtual void Set(const Variable& v, bool bool_val) = 0;

  virtual int GetInt(const Variable& v) const = 0;
  virtual float GetFloat(const Variable &v) const = 0;
  virtual bool GetBool(const Variable& v) const = 0;
};

template<typename T>
struct EntityT : public BaseEntity {
 public:
  explicit EntityT(T *data) : data_(data) {}

  void *RawPtr() override {
    return data_;
  }
  void Set(const Variable &v, int int_val) override {
    SetImpl(v, int_val);
  }
  void Set(const Variable &v, float float_val) override {
    SetImpl(v, float_val);
  }
  void Set(const Variable &v, bool bool_val) override {
    SetImpl(v, bool_val);
  }
  int GetInt(const Variable &v) const override {
    return GetImpl<int>(v);
  }
  float GetFloat(const Variable &v) const override {
    return GetImpl<float>(v);
  }
  bool GetBool(const Variable &v) const override {
    return GetImpl<bool>(v);
  }
 private:
  template<typename V>
  void SetImpl(const Variable& v, V new_value) {
    using AnalysisTree::Types;

    if (v.GetFieldType() == Types::kFloat) {
      ATHelper::SetField(data_, v.GetId(), float(new_value));
      return;
    } else if (v.GetFieldType() == Types::kInteger) {
      ATHelper::SetField(data_, v.GetId(), int(new_value));
      return;
    } else if (v.GetFieldType() == Types::kBool) {
      ATHelper::SetField(data_, v.GetId(), bool(new_value));
      return;
    }
    /* unreachable */
    __builtin_unreachable();
    assert(false);
  }
  template<typename ValueType>
  ValueType GetImpl(const Variable& v) const {
    using AnalysisTree::Types;
    if (v.GetFieldType() == Types::kFloat) {
      return (ValueType) data_->template GetField<float>(v.GetId());
    } else if (v.GetFieldType() == Types::kInteger) {
      return (ValueType) data_->template GetField<int>(v.GetId());
    } else if (v.GetFieldType() == Types::kBool) {
      return (ValueType) data_->template GetField<bool>(v.GetId());
    } else if (v.GetFieldType() == Types::kNumberOfTypes) {
      /* Types::kNumberOfTypes */
      assert(false);
    }
    /* unreachable */
    __builtin_unreachable();
    assert(false);
  }

  T *data_{nullptr};
};

}




#endif //ATTASKSKELETON_ATI2_ENTITYWRAPPER_HPP_
