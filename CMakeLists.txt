cmake_minimum_required (VERSION 3.11)

project(PerfUtils VERSION 0.1.0 LANGUAGES CXX C)

################################################################################
## Target Definiton ############################################################
################################################################################
add_library(PerfUtils
    src/CacheTrace.cc
    src/Cycles.cc
    src/mkdir.cc
    src/Stats.cc
    src/TimeTrace.cc
    src/Util.cc
    cwrapper/timetrace_wrapper.cc
    cwrapper/cycles_wrapper.cc
)
target_include_directories(PerfUtils PUBLIC
    $<BUILD_INTERFACE:src cwrappers>
    $<INSTALL_INTERFACE:include>
)
target_compile_definitions(PerfUtils
    PRIVATE NDEBUG
)
target_compile_options(PerfUtils
    PRIVATE -O3
)
target_compile_features(PerfUtils
    PUBLIC
        cxx_std_11
)
set_target_properties(PerfUtils PROPERTIES
    VERSION ${PerfUtils_VERSION}
)

################################################################################
## Install & Config ############################################################
################################################################################
install(TARGETS PerfUtils EXPORT PerfUtilsTargets 
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(
    FILES
        src/Atomic.h
        src/CacheTrace.h
        src/Cycles.h
        src/Initialize.h
        src/mkdir.h
        src/Stats.h
        src/TimeTrace.h
        src/Util.h
        cwrapper/cycles_wrapper.h
        cwrapper/timetrace_wrapper.h
    DESTINATION
        include/PerfUtils
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PerfUtils/PerfUtilsConfigVersion.cmake"
    VERSION ${PerfUtils_VERSION}
    COMPATIBILITY ExactVersion
)

configure_file(cmake/PerfUtilsConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/PerfUtils/PerfUtilsConfig.cmake"
    COPYONLY
)

set(ConfigPackageLocation lib/cmake/PerfUtils)
install(EXPORT PerfUtilsTargets
    DESTINATION
        ${ConfigPackageLocation}
)
install(
    FILES
        cmake/PerfUtilsConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/PerfUtils/PerfUtilsConfigVersion.cmake"
    DESTINATION
        ${ConfigPackageLocation}
)

################################################################################
## Tests #######################################################################
################################################################################
enable_testing()

add_executable(TimeTraceTest src/TimeTraceTest.cc)
target_link_libraries(TimeTraceTest PerfUtils)
add_test(TimeTraceTest TimeTraceTest)

add_executable(timetrace_wrapper_test cwrapper/timetrace_wrapper_test.c)
target_compile_features(timetrace_wrapper_test PRIVATE c_std_99)
target_link_libraries(timetrace_wrapper_test PerfUtils)
add_test(timetrace_wrapper_test timetrace_wrapper_test)

add_executable(cycles_wrapper_test cwrapper/cycles_wrapper_test.c)
target_link_libraries(cycles_wrapper_test PerfUtils)
add_test(cycles_wrapper_test cycles_wrapper_test)

################################################################################
## Check #######################################################################
################################################################################
add_custom_target(check
    COMMAND scripts/cpplint.py --filter=-runtime/threadsafe_fn,-readability/streams,-whitespace/blank_line,-whitespace/braces,-whitespace/comments,-runtime/arrays,-build/include_what_you_use,-whitespace/semicolon,-build/include src/*.h src/*.cc cwrapper/*.h cwrapper/*.cc
    COMMAND ! grep '.\\{81\\}' src/*.h src/*.cc cwrapper/*.h cwrapper/*.cc cwrapper/*.c
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
