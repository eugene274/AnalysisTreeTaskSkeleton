
find_package(ROOT REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options filesystem)

list(APPEND SOURCES
        TaskRegistry.cpp
        UserTask.cpp
        )

list(APPEND HEADERS
        TaskRegistry.h
        UserTask.h
        Task.h
        )

add_library(at_task STATIC ${SOURCES} ${HEADERS} StaticTaskRegistry.h)
target_link_libraries(at_task  PUBLIC
        AnalysisTreeBase
        AnalysisTreeInfra
        $<TARGET_NAME_IF_EXISTS:AnalysisTreeInfraVersion1>
        ati2_static
        AnalysisTreeCutsRegistry
        Boost::program_options
        ROOT::Core)


target_include_directories(at_task
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC
        $<BUILD_INTERFACE:${ATTaskSkeleton_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        )

add_library(at_task_main STATIC task_main.cpp)
target_link_libraries(at_task_main PUBLIC at_task)
