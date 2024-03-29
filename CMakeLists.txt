cmake_minimum_required(VERSION 3.12)
project(ATTaskSkeleton)

set(CMAKE_CXX_STANDARD 17)



set(ATTaskSkeleton_BUILD_SAMPLE OFF CACHE BOOL "Build sample task")


list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if (NOT AnalysisTree_FOUND)
    set(AnalysisTree_BUILD_TYPE RELEASE CACHE STRING "Build type of the AnalysisTree")
    set(AnalysisTree_GIT_TAG v2.1.1 CACHE STRING "Git tag of the AnalysisTree")
    include(AnalysisTree)
else()
    message("Found external AnalysisTree version: ${AnalysisTree_VERSION}")
endif()

include(AnalysisTreeCuts)


set(ATTaskSkeleton_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ati2)
add_subdirectory(at_task)

if (ATTaskSkeleton_BUILD_SAMPLE)
    add_subdirectory(sample_task)
endif()
