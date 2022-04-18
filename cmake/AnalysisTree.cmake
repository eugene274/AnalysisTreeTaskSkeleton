


if (NOT AnalysisTree_FOUND)
    if (NOT ${AnalysisTree_HOME} STREQUAL "")
        message("Looking for AnalysisTree in ${AnalysisTree_HOME} directory")
        list(APPEND CMAKE_PREFIX_PATH ${AnalysisTree_HOME})
        find_package(AnalysisTree REQUIRED)
    else ()
        include(FetchContent)
        FetchContent_Declare(AnalysisTree
                GIT_REPOSITORY "https://github.com/HeavyIonAnalysis/AnalysisTree.git"
                GIT_TAG ${AnalysisTree_GIT_TAG}
                UPDATE_DISCONNECTED ${UPDATE_DISCONNECTED}
                )

        set(AnalysisTree_BUILD_EXAMPLES OFF)

        FetchContent_GetProperties(AnalysisTree)
        if (NOT analysistree_POPULATED)
            FetchContent_Populate(AnalysisTree)
            add_subdirectory(${analysistree_SOURCE_DIR} ${analysistree_BINARY_DIR})
        endif ()

        get_target_property(AnalysisTreeBase_INCLUDE_DIR AnalysisTreeBase INCLUDE_DIRECTORIES)
        list(APPEND PROJECT_INCLUDE_DIRECTORIES ${AnalysisTreeBase_INCLUDE_DIR})
        get_target_property(AnalysisTreeInfra_INCLUDE_DIR AnalysisTreeInfra INCLUDE_DIRECTORIES)
        list(APPEND PROJECT_INCLUDE_DIRECTORIES ${AnalysisTreeInfra_INCLUDE_DIR})
        set(AnalysisTree_FOUND True)
    endif ()
endif (NOT AnalysisTree_FOUND)