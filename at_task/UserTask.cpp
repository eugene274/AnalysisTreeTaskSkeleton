//
// Created by eugene on 31/08/2020.
//

#include "UserTask.h"

#include <regex>

#include <TFile.h>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Detector.hpp>

using namespace ATI2;

ATI2::Branch *UserFillTask::NewBranch(const AnalysisTree::BranchConfig &config) {
  assert(UseATI2());

  auto branch_name = config.GetName();

  if (branches_out_.find(branch_name) != branches_out_.end())
    throw std::runtime_error("Branch of that name already exists");
  if (branch_name.empty())
    throw std::runtime_error("Branch name cannot be empty");
  auto branch_ptr = std::make_unique<Branch>(config);

  branch_ptr->parent_config = out_config_;
  if (out_tree_) {
    branch_ptr->ConnectOutputTree(out_tree_);
  }
  branch_ptr->SetMutable(true);
  branches_out_.emplace(branch_name, std::move(branch_ptr));
  return branches_out_.find(branch_name)->second.get();
}
ATI2::Branch *UserFillTask::NewBranch(const std::string &branch_name,
                                      AnalysisTree::DetType detector_type) {
  return NewBranch(AnalysisTree::BranchConfig(branch_name, detector_type));
}

ATI2::Branch *UserFillTask::NewBranch(const std::string &branch_name, const AnalysisTree::BranchConfig &config) {
  assert(UseATI2());

  std::cout << "Cloning branch config " << config.GetName() << " to " << branch_name << std::endl;
  auto new_branch = NewBranch(branch_name, config.GetType());
  new_branch->CloneVariables(config);
  return new_branch;
}

void UserFillTask::ATI2_Load(std::map<std::string, void *> &map) {
  assert(UseATI2());
  for (auto &config : config_->GetBranchConfigs()) {
    auto branch_name =
#if ANALYSISTREE_VERSION_MAJOR == 1
      config.GetName();
#elif ANALYSISTREE_VERSION_MAJOR == 2
      config.second.GetName();
#endif

    auto data_ptr_it = map.find(branch_name);
    if (data_ptr_it == map.end()) {
      std::cout << "Branch config with name " << branch_name
        << "has no corresponding pointer in the map" << std::endl;
      continue;
    }

    auto branch = std::make_unique<Branch>(
#if ANALYSISTREE_VERSION_MAJOR == 1
        config
#elif ANALYSISTREE_VERSION_MAJOR == 2
        config.second
#endif
            , data_ptr_it->second);
    branch->parent_config = config_;
    branch->is_connected_to_input = true;
    branch->SetMutable(false);
    branch->Freeze();
    branches_in_.emplace(branch_name, std::move(branch));
  }

}

void UserFillTask::ATI2_InitBranchesBypass() {
  if (enable_bypass) {
    for (auto &in_branch : branches_in_) {
      auto& in_branch_name = in_branch.first;
      /* if already found in branches_out, skipping */
      if (branches_out_.find(in_branch_name) != branches_out_.end()) {
        continue;
      }

      auto src_branch = in_branch.second.get();
      auto tgt_branch = NewBranch(src_branch->GetConfig());
      tgt_branch->Freeze();
      std::cout << "Bypassing branch: " << in_branch_name << std::endl;
      branches_bypass_.emplace_back(std::make_pair(src_branch, tgt_branch));
    }
  }
}
void UserFillTask::ATI2_ExecBypass() {
  for (auto &bypass_pair : branches_bypass_) {
    auto &src = bypass_pair.first;
    auto &dst = bypass_pair.second;
    dst->CopyContentsRaw(src);
  }
}
void UserFillTask::ATI2_Finish() {
  assert(UseATI2());
  if (out_config_) {
    *out_config_ = AnalysisTree::Configuration(GetName());
    for (auto &branch_item : branches_out_) {
      out_config_->AddBranchConfig(branch_item.second->GetConfig());
      // overriding id with hash
      // see: https://github.com/HeavyIonAnalysis/AnalysisTree/issues/57
      auto &config = out_config_->GetBranchConfig(branch_item.first);
#if ANALYSISTREE_VERSION_MAJOR == 1
      config.SetId(branch_item.second->Hash());
#endif
    }
    out_config_->Print();
    if (out_file_) {
      auto cwd = gDirectory;
      out_file_->cd();
      out_config_->Write("Configuration", TObject::kOverwrite);
      cwd->cd();
    }
  } else {
    std::cout << "ATI2_Finish: out_config_ is NULL, so no output config is going to be produced..." << std::endl;
    std::cout << "ATI2_Finish: Ignore this message if you don't expect any AT output" << std::endl;
  }
}
ATI2::Variable UserFillTask::GetVar(const std::string &name) const {
  assert(UseATI2());
  auto &&[br_name, f_name] = ParseVarName(name);
  return GetInBranch(br_name)->GetFieldVar(f_name);
}
std::pair<std::string, std::string> UserFillTask::ParseVarName(const std::string &variable_name) {
  const std::regex re_vname("^(\\w+)/(\\w+)$");

  std::smatch match_results;
  if (std::regex_search(variable_name, match_results, re_vname)) {
    return {match_results.str(1), match_results.str(2)};
  }
  throw std::runtime_error("Invalid format for variable name");
}

