cmake_minimum_required(VERSION 3.25)

# Set the project name
project(library_checker_solutions)

# Include CTest module
include(CTest)

# Turn on export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set global compile options
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wconversion)
add_compile_options(-std=gnu++26)
add_compile_options(-I${CMAKE_SOURCE_DIR}/ac-library)
add_compile_options(-I${CMAKE_SOURCE_DIR}/include)
add_compile_options(-I${CMAKE_SOURCE_DIR}/problems/common)
add_link_options(-L/usr/14.2.0/lib64)
add_link_options(-Wl,-rpath,/usr/14.2.0/lib64)

# Set compile options
set(DEBUG_CFLAGS -UNDEBUG -g3 -O0 -march=native -fsanitize=undefined,address -fno-sanitize-recover=all)
set(CHECK_CFLAGS -UNDEBUG -g0 -O3 -march=native -fsanitize=undefined,address -fno-sanitize-recover=all)
set(BENCH_CFLAGS -DNDEBUG -g0 -O3 -march=native -flto)

# Set link options
set(DEBUG_LFLAGS -fsanitize=undefined,address)
set(CHECK_LFLAGS -fsanitize=undefined,address)
set(BENCH_LFLAGS -flto -static)

# Get all solutions in the directory
file(GLOB_RECURSE SOURCES sol/*.cpp)

# Create executables for each solution
foreach(SOURCE ${SOURCES})
    # Get the solution directory
    get_filename_component(SOLUTION_DIR ${SOURCE} PATH)

    # Get the problem directory
    string(REPLACE ${CMAKE_SOURCE_DIR}/sol ${CMAKE_SOURCE_DIR}/problems PROBLEM_DIR ${SOLUTION_DIR})

    # Get the solution name
    get_filename_component(SOLUTION_NAME ${SOURCE} NAME_WE)

    # Get the problem name
    get_filename_component(PROBLEM_NAME ${PROBLEM_DIR} NAME)

    # Find all input files in the problem directory
    file(GLOB INPUTS ${PROBLEM_DIR}/in/*.in)

    # Ignore problems without inputs
    if(INPUTS STREQUAL "")
        continue()
    endif()

    # Set the executable name
    set(SOL_NAME ${PROBLEM_NAME}-${SOLUTION_NAME})
    set(DEBUG_EXE ${SOL_NAME}.debug)
    set(CHECK_EXE ${SOL_NAME}.check)
    set(BENCH_EXE ${SOL_NAME}.bench)

    # Add the executable target
    add_executable(${DEBUG_EXE} ${SOURCE})
    add_executable(${CHECK_EXE} ${SOURCE})
    add_executable(${BENCH_EXE} ${SOURCE})

    # Set compile options
    target_compile_options(${DEBUG_EXE} PRIVATE ${DEBUG_CFLAGS})
    target_compile_options(${CHECK_EXE} PRIVATE ${CHECK_CFLAGS})
    target_compile_options(${BENCH_EXE} PRIVATE ${BENCH_CFLAGS})

    # Set link options
    target_link_options(${DEBUG_EXE} PRIVATE ${DEBUG_LFLAGS})
    target_link_options(${CHECK_EXE} PRIVATE ${CHECK_LFLAGS})
    target_link_options(${BENCH_EXE} PRIVATE ${BENCH_LFLAGS})

    # Initialize an empty list for verify files
    set(DEPENDS "")

    # Loop through each input file
    foreach(INPUT ${INPUTS})
        get_filename_component(TEST_NAME ${INPUT} NAME_WE)
        set(ANSWER ${PROBLEM_DIR}/out/${TEST_NAME}.out)
        set(CHECKER ${PROBLEM_DIR}/checker)
        set(CHECK_OUTPUT ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.check.output)
        set(CHECK_VERIFY ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.check.verify)
        set(BENCH_OUTPUT ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.bench.output)
        set(BENCH_VERIFY ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.bench.verify)
        list(APPEND DEPENDS ${BENCH_VERIFY})

        # Create custom commands to test the executable
        add_custom_command(
            OUTPUT ${CHECK_OUTPUT}
            COMMAND timeout -v 5 ${CMAKE_BINARY_DIR}/${CHECK_EXE} < ${INPUT} > ${CHECK_OUTPUT}
            DEPENDS ${CHECK_EXE} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${CHECK_VERIFY}
            COMMAND ${CHECKER} ${INPUT} ${CHECK_OUTPUT} ${ANSWER} 2> ${CHECK_VERIFY}
            DEPENDS ${CHECKER} ${INPUT} ${CHECK_OUTPUT} ${ANSWER}
        )
        add_custom_command(
            OUTPUT ${BENCH_OUTPUT}
            COMMAND timeout -v 5 ${CMAKE_BINARY_DIR}/${BENCH_EXE} < ${INPUT} > ${BENCH_OUTPUT}
            DEPENDS ${BENCH_EXE} ${INPUT} ${CHECK_VERIFY}
        )
        add_custom_command(
            OUTPUT ${BENCH_VERIFY}
            COMMAND ${CHECKER} ${INPUT} ${BENCH_OUTPUT} ${ANSWER} 2> ${BENCH_VERIFY}
            DEPENDS ${CHECKER} ${INPUT} ${BENCH_OUTPUT} ${ANSWER}
        )
        add_test(
            NAME ${SOL_NAME}-${TEST_NAME}
            COMMAND ${DEBUG_EXE} < ${INPUT}
        )
    endforeach()

    # Add a custom target for test
    add_custom_target(${SOL_NAME}-check DEPENDS ${DEPENDS})

    # Set the benchmark summary name
    string(REPLACE cpp ${CMAKE_CXX_COMPILER_VERSION}.md SUMMARY ${SOURCE})

    # Add a custom target for benchmark
    add_custom_target(${SOL_NAME}-bench
        COMMAND ${CMAKE_SOURCE_DIR}/benchmark.py ${BENCH_EXE} ${INPUTS} ${SUMMARY}
        DEPENDS ${CMAKE_SOURCE_DIR}/benchmark.py ${BENCH_EXE} ${INPUTS} ${DEPENDS}
    )

    # Add a custom command for bundle
    add_custom_command(
        OUTPUT ${SOURCE}.bundle
        COMMAND echo "'#define'" "'NDEBUG'" > ${SOURCE}.bundle
        COMMAND echo "'#pragma'" "'GCC'" "'optimize(\"O3\")'" >> ${SOURCE}.bundle
        COMMAND oj-bundle
            -I${CMAKE_SOURCE_DIR}/ac-library
            -I${CMAKE_SOURCE_DIR}/include
            -I${CMAKE_SOURCE_DIR}/problems/common
            ${SOURCE} >> ${SOURCE}.bundle
        DEPENDS ${BENCH_EXE}
    )

    # Append targets to lists
    list(APPEND DEBUG ${DEBUG_EXE})
    list(APPEND CHECK ${SOL_NAME}-check)
    list(APPEND BENCH ${SOL_NAME}-bench)
    list(APPEND BUNDLE ${SOURCE}.bundle)
endforeach()

# Add custom targets for processing all
add_custom_target(debug DEPENDS ${DEBUG})
add_custom_target(check DEPENDS ${CHECK})
add_custom_target(bench DEPENDS ${BENCH})
add_custom_target(bundle DEPENDS ${BUNDLE})
