//
// Created by eugene on 31/08/2020.
//

#include "UserTask.h"

#include <regex>

#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Detector.hpp>

using namespace ATI2;

ATI2::Branch *UserFillTask::NewBranch(const std::string &branch_name,
                                      AnalysisTree::DetType detector_type) {
  assert(UseATI2());
  AnalysisTree::BranchConfig branch_config(branch_name, detector_type);

  if (branches_out_.find(branch_name) != branches_out_.end())
    throw std::runtime_error("Branch of that name already exists");

  auto branch_ptr = std::make_unique<Branch>(branch_config);
  branch_ptr->parent_config = out_config_;
  if (out_tree_) {
    branch_ptr->ConnectOutputTree(out_tree_);
  }
  branch_ptr->SetMutable(true);
  branches_out_.emplace(branch_name, std::move(branch_ptr));
  return branches_out_.find(branch_name)->second.get();
}
void UserFillTask::ATI2_Load(std::map<std::string, void *> &map) {
  assert(UseATI2());
  for (auto &config : config_->GetBranchConfigs()) {
    auto branch_name = config.GetName();

    auto data_ptr_it = map.find(branch_name);
    if (data_ptr_it == map.end()) {
      std::cout << "Branch config with name " << branch_name
        << "has no corresponding pointer in the map";
      continue;
    }

    auto branch = std::make_unique<Branch>(config, data_ptr_it->second);
    branch->parent_config = config_;
    branch->is_connected_to_input = true;
    branch->SetMutable(false);
    branch->Freeze();
    branches_in_.emplace(branch_name, std::move(branch));
  }

}

ATI2::Variable UserFillTask::GetVar(const std::string &name) const {
  assert(UseATI2());
  auto &&[br_name, f_name] = ParseVarName(name);
  return GetInBranch(br_name)->GetFieldVar(f_name);
}

void UserFillTask::ATI2_Finish() {
  assert(UseATI2());
  *out_config_ = AnalysisTree::Configuration(GetName());
  for (auto &branch_item : branches_out_) {
    out_config_->AddBranchConfig(branch_item.second->GetConfig());
  }
  out_config_->Print();
}

std::pair<std::string, std::string> UserFillTask::ParseVarName(const std::string &variable_name) {
  const std::regex re_vname("^(\\w+)/(\\w+)$");

  std::smatch match_results;
  if (std::regex_search(variable_name, match_results, re_vname)) {
    return {match_results.str(1), match_results.str(2)};
  }
  throw std::runtime_error("Invalid format for variable name");
}
