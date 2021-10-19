//
// Created by eugene on 31/08/2020.
//

#ifndef ANALYSISTREESKELETON_TASK_MAIN_USERTASK_H
#define ANALYSISTREESKELETON_TASK_MAIN_USERTASK_H

#include <boost/program_options.hpp>
#include <AnalysisTree/AnalysisTreeVersion.hpp>

#if ANALYSISTREE_VERSION_MAJOR == 1
# include <AnalysisTree/FillTask.hpp>
# define ANALYSISTREE_FILLTASK ::AnalysisTree::FillTask
#elif ANALYSISTREE_VERSION_MAJOR == 2
# include <AnalysisTree/infra-1.0/FillTask.hpp>
# define ANALYSISTREE_FILLTASK ::AnalysisTree::Version1::FillTask
#endif

#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Detector.hpp>
#include <TTree.h>

#include "ATI2.hpp"

class TaskRegistry;

class UserTask {

 public:
  virtual ~UserTask() = default;
  virtual std::string GetName() const { return ""; }
  virtual size_t GetPriority() const { return 0l; }
  virtual ANALYSISTREE_FILLTASK *FillTaskPtr() = 0;

  virtual boost::program_options::options_description GetBoostOptions() { return {}; }
  virtual void ProcessBoostVM(const boost::program_options::variables_map &vm) { (void) vm; }

  virtual void PreInit() {}
  virtual void PostFinish() {}

  size_t GetOrderNo() const {
    return order_no_;
  }

  bool IsEnabled() const {
    return is_enabled_;
  }

 private:
  friend TaskRegistry;

  bool is_enabled_{true};
  std::size_t order_no_{0};
};

class UserFillTask : public UserTask, public ANALYSISTREE_FILLTASK {
 public:
  virtual ~UserFillTask() = default;
  ANALYSISTREE_FILLTASK *FillTaskPtr() final {
    return this;
  }

  void Init(std::map<std::string, void *> &map) final {
    if (UseATI2())
      ATI2_Load(map);
    UserInit(map);
    if (UseATI2()) {
      ATI2_InitBranchesBypass();
    }
  }
  void Exec() final {
    UserExec();
    if (UseATI2()) {
      ATI2_ExecBypass();
    }
  }
  void Finish() final {
    UserFinish();
    if (UseATI2())
      ATI2_Finish();
  }

 protected:
  virtual bool UseATI2() const { return true; }
  virtual void UserInit(std::map<std::string, void *> &map) {};
  virtual void UserExec() {};
  virtual void UserFinish() {};


  ATI2::Branch *NewBranch(const AnalysisTree::BranchConfig& config);;
  /**
   * @brief This function creates new empty branch in the out_config_
   * @param branch_name
   * @param detector_type
   * @return
   */
  ATI2::Branch *NewBranch(const std::string &branch_name, AnalysisTree::DetType detector_type);
  ATI2::Branch *NewBranch(const std::string &branch_name, const AnalysisTree::BranchConfig &config);
  inline ATI2::Branch *GetInBranch(const std::string &name) const { return branches_in_.at(name).get(); }
  inline ATI2::Branch *GetOutBranch(const std::string &name) const { return branches_out_.at(name).get(); }
  ATI2::Variable GetVar(const std::string &name) const;
  void BypassBranches() {
    assert(UseATI2());
    enable_bypass = true;
  }

  /* AnalysisTree::Types */
  constexpr static AnalysisTree::Types INTEGER = AnalysisTree::Types::kInteger;
  constexpr static AnalysisTree::Types FLOAT = AnalysisTree::Types::kFloat;
  constexpr static AnalysisTree::Types BOOLEAN = AnalysisTree::Types::kBool;
  /* AnalysisTree::DetType */
  constexpr static AnalysisTree::DetType EVENT_HEADER = AnalysisTree::DetType::kEventHeader;
  constexpr static AnalysisTree::DetType TRACKS = AnalysisTree::DetType::kTrack;
  constexpr static AnalysisTree::DetType PARTICLES = AnalysisTree::DetType::kParticle;
  constexpr static AnalysisTree::DetType MODULES = AnalysisTree::DetType::kModule;
  constexpr static AnalysisTree::DetType HITS = AnalysisTree::DetType::kHit;

 private:
  void ATI2_Load(std::map<std::string, void *> &map);
  void ATI2_InitBranchesBypass();
  void ATI2_ExecBypass();
  void ATI2_Finish();

  std::map<std::string, std::unique_ptr<ATI2::Branch>> branches_in_;
  std::map<std::string, std::unique_ptr<ATI2::Branch>> branches_out_;

  bool enable_bypass{false};
  std::vector<std::pair<ATI2::Branch*, ATI2::Branch*>> branches_bypass_;



  static std::pair<std::string, std::string> ParseVarName(const std::string &variable_name);

};


#endif //ANALYSISTREESKELETON_TASK_MAIN_USERTASK_H
