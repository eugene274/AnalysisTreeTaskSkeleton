include(FetchContent)
FetchContent_Declare(
        AnalysisTreeCuts
        GIT_REPOSITORY https://github.com/HeavyIonAnalysis/AnalysisTreeCuts.git
        GIT_TAG "73f21801fcc8fbc5bb898d195bcf4edaa5330cb9"
)

FetchContent_GetProperties(AnalysisTreeCuts)

if (NOT analysistreecuts_POPULATED)
    FetchContent_Populate(AnalysisTreeCuts)
    add_subdirectory(${analysistreecuts_SOURCE_DIR} ${analysistreecuts_BINARY_DIR})
endif ()