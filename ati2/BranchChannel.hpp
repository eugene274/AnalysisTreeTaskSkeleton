//
// Created by eugene on 13/03/2021.
//

#ifndef ATTASKSKELETON_ATI2_BRANCHCHANNEL_HPP_
#define ATTASKSKELETON_ATI2_BRANCHCHANNEL_HPP_

#include "ATI2_fwd.hpp"
#include "ValueHolder.hpp"
#include "EntityWrapper.hpp"

#include <iostream>
#include <cassert>


namespace ATI2 {

class BranchChannel {
 public:
  BranchChannel(const BranchChannel& other);
  /* Getting value */
  inline ValueHolder Value(const Variable &v) const {
    assert(v.GetParentBranch() == branch);
    assert(v.IsInitialized());
    return ValueHolder(v, entity_.get());
  }
  inline ValueHolder operator[](const Variable &v) const { return Value(v); };
  inline std::size_t GetNChannel() const { return i_channel; }

  /* usage of this functions is highly discouraged */
  void *Data() { return entity_->RawPtr(); }
  const void *Data() const { return entity_->RawPtr(); }
  template<typename T>
  T *DataT() { return reinterpret_cast<T *>(entity_->RawPtr()); }
  template<typename T>
  const T *DataT() const { return reinterpret_cast<T *>(entity_->RawPtr()); }

  /**
   * @brief Copy contents of other branch channel
   * @param other
   * @return
   */
  void CopyContents(const BranchChannel &other);
  void CopyContents(Branch &other);

  void Print(std::ostream &os = std::cout) const;

 private:
  friend Branch;
  friend BranchChannelsIter;

  BranchChannel(Branch *branch, std::size_t i_channel);
  void UpdateEntity();
  void UpdateChannel(std::size_t new_channel);

  std::unique_ptr<BaseEntity> entity_{nullptr};
  Branch *branch;
  std::size_t i_channel;
};


}

#endif //ATTASKSKELETON_ATI2_BRANCHCHANNEL_HPP_
