//
// Created by eugene on 17/02/2021.
//

#include <iostream>

#include <TTree.h>

#include "ATI2.hpp"
#include "ATI2_ATHelper.hpp"

using namespace ATI2;

namespace Impl {

inline void hash_combine(std::size_t &seed) {}

template<typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, Rest... rest) {
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  hash_combine(seed, rest...);
}

std::size_t BranchConfigHasher(const AnalysisTree::BranchConfig &config) {
  using Type = AnalysisTree::Types;

  std::size_t hash = 0;
  hash_combine(hash, config.GetType());

  auto hash_fields = [&hash](const std::map<std::string, Short_t> &fields_map, Type field_type) {
    for (auto &field : fields_map) {
      hash_combine(hash, field.first, field.second, field_type);
    }
  };

  hash_fields(config.GetMap<float>(), Type::kFloat);
  hash_fields(config.GetMap<int>(), Type::kInteger);
  hash_fields(config.GetMap<bool>(), Type::kBool);
  return hash;
}

}

void BranchChannel::Print(std::ostream &os) const {
  os << "Branch " << branch->GetBranchName() << " channel #" << i_channel << std::endl;
}

Variable Branch::GetFieldVar(const std::string &field_name) {
  ATI2::Variable v;
  v.parent_branch = this;
  v.id = v.parent_branch->config.GetFieldId(field_name);
  v.name = this->config.GetName() + "/" + field_name;
  v.field_name = field_name;
  v.field_type = config.GetFieldType(field_name);

  if (v.id == AnalysisTree::UndefValueShort)
    throw std::runtime_error("Field of name '" + v.name + "' not found");
  return v;
}

Branch::~Branch() {
  ApplyT([this](auto entry_ptr) {
    delete entry_ptr;
  });
}

void Branch::ConnectOutputTree(TTree *tree) {
  is_connected_to_output = ApplyT([this, tree](auto entity) -> bool {
    if (!tree)
      return false;
    auto new_tree_branch_ptr = tree->Branch(config.GetName().c_str(),
                                            std::add_pointer_t<decltype(entity)>(&this->data));
    return bool(new_tree_branch_ptr);
  });
}

void Branch::InitDataPtr() {
  ApplyT([this](auto entity) {
    if (entity)
      throw std::runtime_error("Data ptr is already initialized");
    auto entity_id = Hash();
    this->data = new typename std::remove_pointer<decltype(entity)>::type(entity_id);
  });
}

size_t ATI2::Branch::size() const {
  return ApplyT([](auto entity_ptr) -> size_t {
    if constexpr (is_event_header_v < decltype(entity_ptr) >) {
      throw std::runtime_error("Size is not implemented for EventHeader variable");
    } else {
      return entity_ptr->GetNumberOfChannels();
    }
  });
}

BranchChannel::BranchChannel(Branch *branch, size_t i_channel) : branch(branch), i_channel(i_channel) {
  UpdatePointer();
}

BranchChannel Branch::operator[](size_t i_channel) { return BranchChannel(this, i_channel); }
BranchChannel Branch::NewChannel() {
  CheckMutable(true);
  ApplyT([this](auto entity_ptr) {
    if constexpr (is_event_header_v < decltype(entity_ptr) >) {
      throw std::runtime_error("Not applicable for EventHeader");
    } else {
      auto channel = entity_ptr->AddChannel();
      channel->Init(this->config);
      Freeze();
    }
  });
  return operator[](size() - 1);
}

Variable Branch::NewVariable(const std::string &field_name, AnalysisTree::Types type) {
  if (field_name.empty())
    throw std::runtime_error("Field name cannot be empty");
  if (type == AnalysisTree::Types::kNumberOfTypes)
    throw std::runtime_error("Type of the field cannot be kNumberOfTypes");
  if (HasField(field_name)) {
    throw std::runtime_error("Field of name '" + field_name + "' already exists in the config");
  }

  CheckFrozen(false);
  CheckMutable(true);
  using AnalysisTree::Types;


  if (Types::kFloat == type) {
    config.template AddField<float>(field_name);
  } else if (Types::kInteger == type) {
    config.template AddField<int>(field_name);
  } else if (Types::kBool == type) {
    config.template AddField<bool>(field_name);
  } else {
    /* should never happen */
    assert(false);
  }

  UpdateConfigHash();

  /* Init EventHeader */
  if (AnalysisTree::DetType::kEventHeader == config.GetType()) {
    ((AnalysisTree::EventHeader *) data)->Init(config);
  }

  ATI2::Variable v;
  v.name = config.GetName() + "/" + field_name;
  v.field_name = field_name;
  v.parent_branch = this;
  v.id = config.GetFieldId(field_name);
  v.field_type = config.GetFieldType(field_name);
  return v;
}

void Branch::CloneVariables(const AnalysisTree::BranchConfig &other) {
  auto import_fields_from_map = [this](const std::map<std::string, short> &map, AnalysisTree::Types type) {
    for (auto &element : map) {
      auto field_name = element.first;
      if (HasField(field_name)) {
        std::cout << "Field '" << field_name << "' already exists" << std::endl;
        continue;
      }
      this->NewVariable(field_name, type);
    } // map elements
  };

  import_fields_from_map(other.GetMap<float>(), AnalysisTree::Types::kFloat);
  import_fields_from_map(other.GetMap<int>(), AnalysisTree::Types::kInteger);
  import_fields_from_map(other.GetMap<bool>(), AnalysisTree::Types::kBool);
}

void Branch::ClearChannels() {
  CheckMutable();
  ApplyT([](auto entity_ptr) -> void {
    if constexpr (is_event_header_v < decltype(entity_ptr) >) {
      throw std::runtime_error("Not applicable for EventHeader");
    } else {
      entity_ptr->ClearChannels();
    }
  });
}
bool Branch::HasField(const std::string &field_name) const {
  auto has_field = [&field_name](const std::map<std::string, Short_t> &map) {
    return map.find(field_name) != map.end();
  };
  return has_field(config.GetMap<float>()) ||
      has_field(config.GetMap<int>()) ||
      has_field(config.GetMap<bool>());
}
std::vector<std::string> Branch::GetFieldNames() const {
  std::vector<std::string> result;
  auto fill_vector_from_map = [&result](const std::map<std::string, short> &fields_map) -> void {
    for (auto &element : fields_map) {
      result.push_back(element.first);
    }
  };
  fill_vector_from_map(config.GetMap<float>());
  fill_vector_from_map(config.GetMap<int>());
  fill_vector_from_map(config.GetMap<bool>());
  return result;
}
void Branch::CopyContents(Branch *other) {
  if (this == other) {
    throw std::runtime_error("Copying contents from the same branch makes no sense");
  }
  CheckMutable();

  if (other->config.GetType() != config.GetType()) {
    throw std::runtime_error("Branch types must be the same");
  }
  if (config.GetType() != AnalysisTree::DetType::kEventHeader) {
    throw std::runtime_error("Only EventHeader is available for Branch::CopyContents");
  }

  auto mapping_it = copy_fields_mapping.find(other);
  if (mapping_it == copy_fields_mapping.end()) {
    CreateMapping(other);
    mapping_it = copy_fields_mapping.find(other);
  }

  /* evaluate mapping */
  auto src_branch = mapping_it->first;
  const auto &mapping = mapping_it->second;

  for (auto &field_pair /* src : dst */: mapping.field_pairs) {
    this->Value(field_pair.second) = src_branch->Value(field_pair.first);
  }

}
void Branch::CopyContentsRaw(Branch *other) {
  if (this == other) {
    throw std::runtime_error("Copying contents from the same branch makes no sense");
  }
  CheckMutable();
  CheckFrozen();

  if (config_hash != other->config_hash) {
    throw std::runtime_error("Branch configurations are not consistent.");
  }
  auto src_data_ptr = other->data;
  ApplyT([src_data_ptr](auto dst_data_ptr) {
    auto typed_src_data_ptr = reinterpret_cast<decltype(dst_data_ptr)>(src_data_ptr);
    *dst_data_ptr = *typed_src_data_ptr;
  });

}
void Branch::CreateMapping(Branch *other) {
  if (copy_fields_mapping.find(other) != copy_fields_mapping.end()) {
    // TODO Warning
    return;
  }

  CheckFrozen();
  other->CheckFrozen();

  const std::map<AnalysisTree::Types, std::string> types_map = {
      {AnalysisTree::Types::kFloat, "float"},
      {AnalysisTree::Types::kInteger, "integer"},
      {AnalysisTree::Types::kBool, "bool"}
  };

  std::cout << "New cached mapping " << other->config.GetName() << " --> " << config.GetName() << std::endl;
  FieldsMapping fields_mapping;
  for (auto &field_name : other->GetFieldNames()) {
    if (!HasField(field_name)) { continue; }
    fields_mapping.field_pairs.emplace_back(std::make_pair(other->GetFieldVar(field_name), GetFieldVar(field_name)));
    std::cout << "\t" << field_name
              << "\t(" << types_map.at(other->GetFieldVar(field_name).GetFieldType()) << " ---> "
              << types_map.at(GetFieldVar(field_name).GetFieldType()) << ")" << std::endl;
  }
  copy_fields_mapping.emplace(other, std::move(fields_mapping));
}
void Branch::UseFields(std::vector<std::pair<std::string, std::reference_wrapper<Variable>>> &&vars) {
  for (auto &element : vars) {
    element.second.get() = GetFieldVar(element.first);
  }
}
void Branch::UpdateConfigHash() {
  config_hash = Impl::BranchConfigHasher(config);
}

void BranchChannel::UpdateChannel(size_t new_channel) {
  i_channel = new_channel;
  UpdatePointer();
}
void BranchChannel::UpdatePointer() {
  if (i_channel < branch->size()) {
    data_ptr = branch->ApplyT([this](auto entity_ptr) -> void * {
      if constexpr (Branch::is_event_header_v<decltype(entity_ptr)>) {
        throw std::runtime_error("Getting channel of the EventHeader is not implemented");
      } else {
        return &(entity_ptr->Channels()->data()[i_channel]);
      }
    });
  } else {
    data_ptr = nullptr;
  }
}
void BranchChannel::CopyContents(const BranchChannel &other) {
  branch->CheckMutable();

  auto mapping_it = branch->copy_fields_mapping.find(other.branch);
  if (mapping_it == branch->copy_fields_mapping.end()) {
    branch->CreateMapping(other.branch);
    mapping_it = branch->copy_fields_mapping.find(other.branch);
  }

  /* Eval mapping */
  const auto &field_pairs = mapping_it->second.field_pairs;

  for (auto &field_pair /* src : dst */ : field_pairs) {
    this->Value(field_pair.second) = other.Value(field_pair.first);
  }

}
void BranchChannel::CopyContents(Branch &other) {
  branch->CheckMutable();

  if (other.GetBranchType() != AnalysisTree::DetType::kEventHeader) {
    throw std::runtime_error("This operation is allowed only between iterable and non-iterable types");
  }

  auto mapping_it = branch->copy_fields_mapping.find(&other);
  if (mapping_it == branch->copy_fields_mapping.end()) {
    branch->CreateMapping(&other);
    mapping_it = branch->copy_fields_mapping.find(&other);
  }

  /* Eval mapping */
  const auto &field_pairs = mapping_it->second.field_pairs;

  for (auto &field_pair /* src : dst */ : field_pairs) {
    this->Value(field_pair.second) = other.Value(field_pair.first);
  }

}

BranchChannelsIter &BranchChannelsIter::operator++() {
  i_channel++;
  current_channel->UpdateChannel(i_channel);
  return *this;
}

ValueHolder Variable::operator*() const { return parent_branch->Value(*this); }

void Variable::Print(std::ostream &os) const {
  os << name << "(id = " << id << ")" << std::endl;
}

namespace Impl {

template<typename Entity>
inline Entity *DataT(void *data_ptr) { return reinterpret_cast<Entity *>(data_ptr); }
template<typename Entity>
inline const Entity *DataT(const void *data_ptr) { return reinterpret_cast<const Entity *>(data_ptr); }

template<typename Functor, typename DataPtr>
auto ApplyToEntity(AnalysisTree::DetType det_type, DataPtr ptr, Functor &&functor) {
  using AnalysisTree::DetType;
  if (DetType::kParticle == det_type) {
    return functor(DataT<AnalysisTree::Particle>(ptr));
  } else if (DetType::kTrack == det_type) {
    return functor(DataT<AnalysisTree::Track>(ptr));
  } else if (DetType::kModule == det_type) {
    return functor(DataT<AnalysisTree::Module>(ptr));
  } else if (DetType::kHit == det_type) {
    return functor(DataT<AnalysisTree::Hit>(ptr));
  } else if (DetType::kEventHeader == det_type) {
    return functor(DataT<AnalysisTree::EventHeader>(ptr));
  }
  /* unreachable */
  __builtin_unreachable();
  assert(false);
}

template<typename Entity, typename ValueType>
ValueType ReadValue(const Variable &v, const Entity *e) {
  using AnalysisTree::Types;

  if (v.GetFieldType() == Types::kFloat) {
    return (ValueType) e->template GetField<float>(v.GetId());
  } else if (v.GetFieldType() == Types::kInteger) {
    return (ValueType) e->template GetField<int>(v.GetId());
  } else if (v.GetFieldType() == Types::kBool) {
    return (ValueType) e->template GetField<bool>(v.GetId());
  } else if (v.GetFieldType() == Types::kNumberOfTypes) {
    /* Types::kNumberOfTypes */
    assert(false);
  }
  /* unreachable */
  __builtin_unreachable();
  assert(false);
}

template<typename Entity, typename ValueType>
inline
void SetValue(const Variable &v, Entity *e, ValueType new_value) {
  using AnalysisTree::Types;

  if (v.GetFieldType() == Types::kFloat) {
    ATHelper::SetField(e, v.GetId(), float(new_value));
    return;
  } else if (v.GetFieldType() == Types::kInteger) {
    ATHelper::SetField(e, v.GetId(), int(new_value));
    return;
  } else if (v.GetFieldType() == Types::kBool) {
    ATHelper::SetField(e, v.GetId(), bool(new_value));
    return;
  }
  /* unreachable */
  __builtin_unreachable();
  assert(false);
}

} // namespace Impl
float ValueHolder::GetVal() const {
  return Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(),
                             data_ptr, [this](auto entity_ptr) {
        using Entity = std::remove_const_t<std::remove_pointer_t<decltype(entity_ptr)>>;
        return Impl::ReadValue<Entity, float>(this->v, entity_ptr);
      });
}

int ValueHolder::GetInt() const {
  return Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(),
                             data_ptr, [this](auto entity_ptr) {
        using Entity = std::remove_const_t<std::remove_pointer_t<decltype(entity_ptr)>>;
        return Impl::ReadValue<Entity, int>(this->v, entity_ptr);
      });
}

bool ValueHolder::GetBool() const {
  return Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(),
                             data_ptr, [this](auto entity_ptr) {
        using Entity = std::remove_const_t<std::remove_pointer_t<decltype(entity_ptr)>>;
        return Impl::ReadValue<Entity, bool>(this->v, entity_ptr);
      });
}
ValueHolder::operator float() const {
  return GetVal();
}
void ValueHolder::SetVal(float val) const {
  v.GetParentBranch()->CheckMutable(true);
  Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(), data_ptr, [this, val](auto entity_ptr) {
    Impl::SetValue(v, entity_ptr, val);
  });
}
void ValueHolder::SetVal(int val) const {
  v.GetParentBranch()->CheckMutable(true);
  Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(), data_ptr, [this, val](auto entity_ptr) {
    Impl::SetValue(v, entity_ptr, val);
  });
}
void ValueHolder::SetVal(bool val) const {
  v.GetParentBranch()->CheckMutable(true);
  Impl::ApplyToEntity(v.GetParentBranch()->GetBranchType(), data_ptr, [this, val](auto entity_ptr) {
    Impl::SetValue(v, entity_ptr, val);
  });
}

ValueHolder &ValueHolder::operator=(const ValueHolder &other) {
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
