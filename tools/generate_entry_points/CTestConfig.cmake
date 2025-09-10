# CTest configuration for XGL Entry Point Generator

# CDash/CTest settings
set(CTEST_PROJECT_NAME "XGL-EntryPointGenerator")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EST")

# Test timeout settings
set(CTEST_TEST_TIMEOUT 300)

# Parallel test execution
include(ProcessorCount)
ProcessorCount(CPU_COUNT)
if(CPU_COUNT EQUAL 0)
    set(CPU_COUNT 1)
endif()

# Use up to 4 parallel jobs or CPU count, whichever is smaller
math(EXPR PARALLEL_JOBS "${CPU_COUNT} > 4 ? 4 : ${CPU_COUNT}")
set(CTEST_BUILD_FLAGS "-j${PARALLEL_JOBS}")
set(CTEST_PARALLEL_LEVEL ${PARALLEL_JOBS})

# Coverage settings (if available)
find_program(PYTHON_COVERAGE_EXECUTABLE 
    NAMES coverage 
    DOC "Python coverage tool")

if(PYTHON_COVERAGE_EXECUTABLE)
    set(CTEST_COVERAGE_COMMAND ${PYTHON_COVERAGE_EXECUTABLE})
    set(CTEST_COVERAGE_EXTRA_FLAGS "xml")
endif()

# Memory checking (if available)  
find_program(PYTHON_MEMORY_CHECK
    NAMES valgrind
    DOC "Memory checking tool")

if(PYTHON_MEMORY_CHECK)
    set(CTEST_MEMORYCHECK_COMMAND ${PYTHON_MEMORY_CHECK})
    set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=50")
endif()

# Test result formatting
set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 1024)
set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 8192)
set(CTEST_OUTPUT_ON_FAILURE TRUE)