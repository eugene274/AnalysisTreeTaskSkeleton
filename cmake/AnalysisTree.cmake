find_package(AnalysisTree QUIET)

if(NOT AnalysisTree_FOUND)

include(FetchContent)
FetchContent_Declare(
        AnalysisTree
        GIT_REPOSITORY https://github.com/HeavyIonAnalysis/AnalysisTree.git
        GIT_TAG        ${AnalysisTree_GIT_TAG}
)

FetchContent_GetProperties(AnalysisTree)

if (NOT analysistree_POPULATED)
    FetchContent_Populate(AnalysisTree)

    set(AnalysisTree_BUILD_EXAMPLES OFF)
    # there is no machinery to set variable for nested subdirectory only :C
    set(CMAKE_BUILD_TYPE_SAVED ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE ${AnalysisTree_BUILD_TYPE})
    add_subdirectory(${analysistree_SOURCE_DIR} ${analysistree_BINARY_DIR})
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_SAVED})

endif ()

endif()
