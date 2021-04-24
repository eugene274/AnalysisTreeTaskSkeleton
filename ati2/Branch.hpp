//
// Created by eugene on 13/03/2021.
//

#ifndef ATTASKSKELETON_ATI2_BRANCH_HPP_
#define ATTASKSKELETON_ATI2_BRANCH_HPP_

#include <AnalysisTree/BranchConfig.hpp>
#include <AnalysisTree/Configuration.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Detector.hpp>

#include <map>
#include <vector>
#include <cassert>

#include "BranchChannel.hpp"
#include <TTree.h>

class TTree;

namespace ATI2 {

struct BranchChannelsIter {
  BranchChannelsIter(Branch *branch, size_t i_channel) :
      branch(branch), i_channel(i_channel) {
    current_channel.reset(new BranchChannel(branch, i_channel));
  }

  bool operator==(const BranchChannelsIter &rhs) const {
    return i_channel == rhs.i_channel &&
        branch == rhs.branch;
  }
  bool operator!=(const BranchChannelsIter &rhs) const {
    return !(rhs == *this);
  }
  BranchChannel operator*() const {
    return current_channel.operator*();
  }
  BranchChannel &operator*() {
    return current_channel.operator*();
  }
  BranchChannelsIter &operator++();

  std::unique_ptr<BranchChannel> current_channel;
  Branch *branch;
  size_t i_channel;
};

struct Branch {
  struct BranchChannelsLoop {
    explicit BranchChannelsLoop(Branch *branch) : branch(branch) {}
    Branch *branch{nullptr};

    inline BranchChannelsIter begin() const { return branch->ChannelsBegin(); };
    inline BranchChannelsIter end() const { return branch->ChannelsEnd(); };
  };

  struct FieldsMapping {
    std::vector<std::pair<Variable /* src */, Variable /* dst */>> field_pairs;
  };

  ~Branch();

 protected:
  AnalysisTree::BranchConfig config;
 private:
  bool is_mutable{false};
  bool is_frozen{false};
  std::size_t config_hash{0};

 public:
  AnalysisTree::Configuration *parent_config{nullptr};
  bool is_connected_to_input{false};
  bool is_connected_to_output{false};

  std::map<const Branch * /* other branch */, FieldsMapping> copy_fields_mapping;

  /* c-tors */
  explicit Branch(AnalysisTree::BranchConfig config) : config(std::move(config)) {
    UpdateConfigHash();
  }

  /* Accessors to branch' main parameters, used very often */
  inline auto GetBranchName() const { return config.GetName(); }
  inline auto GetBranchType() const { return config.GetType(); }
  inline const AnalysisTree::BranchConfig &GetConfig() const { return config; }

  virtual void InitDataPtr() = 0;
  virtual void ConnectOutputTree(TTree *tree) = 0;

  Variable GetFieldVar(const std::string &field_name);
  /**
   * @brief Gets variables according to variable names specified in the arguments.
   * Returns tuple of variables which is suitable for unpacking with std::tie()
   * @tparam Args
   * @param field_name variable names convertible to std::string
   * @return tuple of variables
   */
  template<typename ... Args>
  auto GetVars(Args ... field_name) {
    return GetVarsImpl(std::array<std::string, sizeof...(Args)>{{std::string(field_name)...}},
                       std::make_index_sequence<sizeof...(Args)>());
  }
  /**
   * @brief Initializes ATI2::Variable objects
   * @param vars - vector of pairs with name and reference to the ATI2::Variable object
   */
  void UseFields(std::vector<std::pair<std::string, std::reference_wrapper<Variable>>> &&vars,
                 bool ignore_missing = false);
  bool HasField(const std::string &field_name) const;
  std::vector<std::string> GetFieldNames() const;

  /* Getting value */
  inline ValueHolder Value(const Variable &v) const {
    assert(v.IsInitialized());
    assert(v.GetParentBranch() == this);
    if (config.GetType() == AnalysisTree::DetType::kEventHeader) {
      return ValueHolder(v, data);
    }
    throw std::runtime_error("Not implemented for iterable branch");
  }

  inline ValueHolder operator[](const Variable &v) const { return Value(v); };

  /* iterating */
  virtual size_t size() const = 0;
  BranchChannel operator[](size_t i_channel);
  inline BranchChannelsLoop Loop() { return BranchChannelsLoop(this); };
  inline BranchChannelsIter ChannelsBegin() { return BranchChannelsIter(this, 0); };
  inline BranchChannelsIter ChannelsEnd() { return BranchChannelsIter(this, size()); };

  /* Modification */
  void Freeze(bool freeze = true) { is_frozen = freeze; };
  void SetMutable(bool is_mutable = true) { Branch::is_mutable = is_mutable; }
  /* Checks are used very often */
  inline void CheckFrozen(bool expected = true) const {
    if (is_frozen != expected)
      throw std::runtime_error("Branch is frozen");
  }
  inline void CheckMutable(bool expected = true) const {
    if (is_mutable != expected)
      throw std::runtime_error("Branch is not mutable");
  }
  BranchChannel NewChannel();
  virtual void ClearChannels() = 0;
  Variable NewVariable(const std::string &field_name, AnalysisTree::Types type);
  void CloneVariables(const AnalysisTree::BranchConfig &other);
  void CopyContents(Branch *br);

  /**
   * @brief Copies contents from other branch 'as-is'. Faster than CopyContents() since it creates no mapping
   * @param other
   */
  void CopyContentsRaw(Branch *other);

  void CreateMapping(Branch *other);

  void UpdateConfigHash();

  template<typename EntityPtr>
  constexpr static const bool is_event_header_v =
      std::is_same_v<AnalysisTree::EventHeader, std::remove_const_t<std::remove_pointer_t<EntityPtr>>>;


  AnalysisTree::ShortInt_t Hash() const {
    const auto hasher = std::hash<std::string>();
    return AnalysisTree::ShortInt_t(hasher(config.GetName()));
  }
 private:

  template<size_t ... Idx>
  auto GetVarsImpl(std::array<std::string, sizeof ... (Idx)> &&field_names, std::index_sequence<Idx...>) {
    return std::make_tuple(GetFieldVar(field_names[Idx])...);
  }

 public:
  /* factory functions */
  static Branch *MakeFrom(const AnalysisTree::BranchConfig &config);
  static Branch *MakeFrom(const AnalysisTree::BranchConfig &config, void *ptr);

};

template<typename EntityType>
struct BranchT : public Branch {
  typedef EntityType entity_type;
  typedef entity_type *entity_pointer;
  typedef entity_type &entity_reference;
  // TODO channel type


 private:
  entity_pointer data_;

  friend Branch * Branch::MakeFrom(const AnalysisTree::BranchConfig &config);
  friend Branch * Branch::MakeFrom(const AnalysisTree::BranchConfig &config, void *ptr);

  explicit BranchT(const AnalysisTree::BranchConfig& config) : Branch(config) {
    InitDataPtr();
  }
  BranchT(const AnalysisTree::BranchConfig& config, entity_pointer data) : Branch(config), data_(data) {}

 public:
  void InitDataPtr() final {
    if (data_) {
      throw std::runtime_error("Data ptr is already initialized");
    }
    auto entity_id = Hash();
    data_ = new entity_type(entity_id);
  }
  void ConnectOutputTree(TTree *tree) final {
    if (tree) {
      auto tree_branch = tree->Branch(config.GetName().c_str(), &data_);
      is_connected_to_output = bool(tree_branch);
    }
  }

  size_t size() const final {
    return SizeImpl(data_);
  }

  void ClearChannels() override {
    CheckMutable();
    ClearChannelsImpl(data_);
  }

 private:
  /* Implementations */
  template<typename T>
  static
  size_t  SizeImpl(const T *entity) { return entity->GetNumberOfChannels(); }
  static
  size_t  SizeImpl(const AnalysisTree::EventHeader* /* entity */) { throw std::runtime_error("Size is not implemented for EventHeader branch"); }


  template<typename T>
  static
  void  ClearChannelsImpl(T* entity) { entity->ClearChannels(); }

  static
  void  ClearChannelsImpl(AnalysisTree::EventHeader */* entity */) { throw std::runtime_error("ClearChannels is not implemented for EventHeader branch"); }
};



} // namespace ATI2

#endif //ATTASKSKELETON_ATI2_BRANCH_HPP_
