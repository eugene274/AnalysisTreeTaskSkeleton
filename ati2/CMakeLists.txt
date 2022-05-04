
find_package(ROOT REQUIRED COMPONENTS Core Tree)

include_directories(${CMAKE_BINARY_DIR}/_deps/analysistree-build/include)
set(ATI2_SOURCES Branch.cpp ValueHolder.cpp BranchChannel.cpp Variable.cpp)
list(APPEND ATI2_RELEASE_OPTIONS "-O2")
list(APPEND ATI2_DEBUG_OPTIONS "-Wall" "-g" "-ggdb")

add_library(ati2 SHARED ${ATI2_SOURCES})
target_link_libraries(ati2 PUBLIC AnalysisTreeBase
        ROOT::Core ROOT::Tree)
target_include_directories(ati2
        PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_options(ati2
        PRIVATE
        "$<$<CONFIG:RELEASE>:${ATI2_RELEASE_OPTIONS}>"
        "$<$<CONFIG:DEBUG>:${ATI2_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Debug>:${ATI2_DEBUG_OPTIONS}>"
        )

add_library(ati2_static ${ATI2_SOURCES})
target_link_libraries(ati2_static PUBLIC AnalysisTreeBase
        ROOT::Core ROOT::Tree)
target_include_directories(ati2_static
        PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_options(ati2_static
        PRIVATE
        "$<$<CONFIG:RELEASE>:${ATI2_RELEASE_OPTIONS}>"
        "$<$<CONFIG:DEBUG>:${ATI2_DEBUG_OPTIONS}>"
        "$<$<CONFIG:Debug>:${ATI2_DEBUG_OPTIONS}>"
        )
