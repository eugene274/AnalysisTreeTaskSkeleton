//
// Created by eugene on 31/08/2020.
//

#ifndef ANALYSISTREESKELETON_TASK_MAIN_USERTASK_H
#define ANALYSISTREESKELETON_TASK_MAIN_USERTASK_H

#include <boost/program_options.hpp>
#include <AnalysisTree/FillTask.hpp>
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
  virtual AnalysisTree::FillTask *FillTaskPtr() = 0;

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

class UserFillTask : public UserTask, public AnalysisTree::FillTask {
 public:
  virtual ~UserFillTask() = default;
  AnalysisTree::FillTask *FillTaskPtr() final {
    return this;
  }

  void Init(std::map<std::string, void *> &map) final {
    if (UseATI2())
      ATI2_Load(map);
    UserInit(map);
  }
  void Exec() final {
    UserExec();
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
  /**
   * @brief This function creates new branch in the out_config_
   * @param branch_name
   * @param detector_type
   * @return
   */
  ATI2::Branch *NewBranch(const std::string &branch_name, AnalysisTree::DetType detector_type);
  inline ATI2::Branch *GetInBranch(const std::string &name) const { return branches_in_.at(name).get(); }
  inline ATI2::Branch *GetOutBranch(const std::string &name) const { return branches_out_.at(name).get(); }
  ATI2::Variable GetVar(const std::string &name) const;
 private:
  void ATI2_Load(std::map<std::string, void *> &map);
  void ATI2_Finish();
  std::map<std::string, std::unique_ptr<ATI2::Branch>> branches_in_;
  std::map<std::string, std::unique_ptr<ATI2::Branch>> branches_out_;

  static std::pair<std::string, std::string> ParseVarName(const std::string &variable_name);

};


#endif //ANALYSISTREESKELETON_TASK_MAIN_USERTASK_H
