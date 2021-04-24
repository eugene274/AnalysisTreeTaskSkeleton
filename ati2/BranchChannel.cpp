//
// Created by eugene on 13/03/2021.
//

#include "BranchChannel.hpp"
#include "Branch.hpp"

using namespace ATI2;

void BranchChannel::UpdateChannel(size_t new_channel) {
  i_channel = new_channel;
  UpdateEntity();
}
void BranchChannel::UpdateEntity() {
  if (i_channel < branch->size()) {
    entity_.reset(branch->GetEntity(i_channel));
  } else {
    entity_.reset();
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

BranchChannel::BranchChannel(Branch *branch, std::size_t i_channel) : branch(branch), i_channel(i_channel) {
  UpdateEntity();
}



void BranchChannel::Print(std::ostream &os) const {
  os << "Branch " << branch->GetBranchName() << " channel #" << i_channel << std::endl;
}
BranchChannel::BranchChannel(const BranchChannel &other) {
  branch = other.branch;
  i_channel = other.i_channel;
  UpdateEntity();
}
