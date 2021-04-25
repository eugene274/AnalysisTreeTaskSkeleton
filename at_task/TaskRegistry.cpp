//
// Created by eugene on 31/08/2020.
//

#include "TaskRegistry.h"

TaskRegistry &TaskRegistry::Instance() {
  static TaskRegistry registry;
  return registry;
}

std::vector<std::string> TaskRegistry::GetTaskNames() {
  std::vector<std::string> result;
  std::transform(task_singletons_.begin(), task_singletons_.end(),
                 std::back_inserter(result), [] (decltype(task_singletons_)::value_type &ele) { return ele.first; });
  return result;
}

void TaskRegistry::UnloadAllTasks() {
  for (auto &entry : task_singletons_) {
    entry.second.Reset();
  }
}
void TaskRegistry::LoadAll() {
  for (auto &t : task_singletons_) {
    t.second.Load();
  }
}
void TaskRegistry::Unload(const std::string &name) {
  task_singletons_.at(name).Reset();
}
void TaskRegistry::Load(const std::string &name) {
  task_singletons_.at(name).Load();
}
