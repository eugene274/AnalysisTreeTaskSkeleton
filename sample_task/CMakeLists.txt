add_executable(sample_task SampleTask.cpp)
target_link_libraries(sample_task PUBLIC at_task_main)

#add_executable(static_registry StaticTaskRegistryTest.cpp)
#target_link_libraries(static_registry  PUBLIC at_task)
