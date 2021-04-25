//
// Created by eugene on 31/08/2020.
//

#ifndef ANALYSISTREESKELETON_TASK_MAIN_TASK_H
#define ANALYSISTREESKELETON_TASK_MAIN_TASK_H

#include "UserTask.h"
#include "TaskRegistry.h"

class UserTask;
class TaskRegistry;

#define TASK_DEF(TASK_CLASS, PRIORITY) \
private:                          \
public:                      \
  static int REGISTRY_FLAG; \
  static const char* Name() { return #TASK_CLASS; }    \
  static std::size_t Priority() { return PRIORITY; }   \
  static TASK_CLASS* Instance() { return dynamic_cast<TASK_CLASS*>(TaskRegistry::Instance().TaskInstance(#TASK_CLASS)); } \
  std::size_t GetPriority() const override { return TASK_CLASS::Priority(); } \
  std::string GetName() const override { return TASK_CLASS::Name(); }

#define TASK_IMPL(TASK_CLASS) \
int TASK_CLASS::REGISTRY_FLAG = TaskRegistry::Instance().RegisterTask<TASK_CLASS>(TASK_CLASS::Name());

template<typename T>
[[deprecated("Use T::Instance() instead")]]
T* GetTaskPtr() {
  return T::Instance();
}

#endif //ANALYSISTREESKELETON_TASK_MAIN_TASK_H
