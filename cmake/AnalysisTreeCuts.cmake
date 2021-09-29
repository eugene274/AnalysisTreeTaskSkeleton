include(FetchContent)
FetchContent_Declare(
        AnalysisTreeCuts
        GIT_REPOSITORY https://github.com/HeavyIonAnalysis/AnalysisTreeCuts.git
)

FetchContent_GetProperties(AnalysisTreeCuts)

if (NOT analysistreecuts_POPULATED)
    FetchContent_Populate(AnalysisTreeCuts)
    add_subdirectory(${analysistreecuts_SOURCE_DIR} ${analysistreecuts_BINARY_DIR})
endif ()