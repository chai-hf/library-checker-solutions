cmake_minimum_required(VERSION 3.25)

# Set the project name
project(library_checker_solutions)

# Include CTest module
include(CTest)

# Turn on export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set global compile options
add_compile_options(-std=gnu++26)
add_compile_options(-march=native)
add_compile_options(-I${CMAKE_SOURCE_DIR}/ac-library)
add_compile_options(-I${CMAKE_SOURCE_DIR}/include)
add_compile_options(-I${CMAKE_SOURCE_DIR}/problems/common)
add_link_options(-Wl,-rpath,/usr/gcc-14.2.0/lib64)

# Set compile options
set(DEBUG_CFLAGS -UNDEBUG -g3 -O0 -fsanitize=undefined,address -fno-sanitize-recover=all -Wall -Wextra -Wconversion)
set(CHECK_CFLAGS -UNDEBUG -g0 -O3 -fsanitize=undefined,address -fno-sanitize-recover=all)
set(BENCH_CFLAGS -DNDEBUG -g0 -O3 -flto)
set(COVER_CFLAGS -DNDEBUG -g0 -Og -coverage)

# Set link options
set(DEBUG_LFLAGS -fsanitize=undefined,address)
set(CHECK_LFLAGS -fsanitize=undefined,address)
set(BENCH_LFLAGS -flto -static)
set(COVER_LFLAGS -coverage)

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

    # Set a sub directory
    set(SUB_DIR ${SOLUTION_DIR}/${SOLUTION_NAME})

    # Find all input files in the problem directory
    file(GLOB INPUTS ${PROBLEM_DIR}/in/*.in)

    # Ignore problems without inputs
    if(INPUTS STREQUAL "")
        continue()
    endif()

    # Ignore empty solutions
    file(SIZE ${SOURCE} SIZE)
    if(SIZE EQUAL 0)
        continue()
    endif()

    # Set the executable name
    set(SOL_NAME ${PROBLEM_NAME}-${SOLUTION_NAME})
    set(DEBUG_EXE ${SOL_NAME}.debug)
    set(CHECK_EXE ${SOL_NAME}.check)
    set(BENCH_EXE ${SOL_NAME}.bench)
    set(GLOBL_EXE ${SOL_NAME}.globl)
    set(LOCAL_EXE ${SOL_NAME}.local)

    # Add the executable target
    add_executable(${DEBUG_EXE} ${SOURCE})
    add_executable(${CHECK_EXE} ${SOURCE})
    add_executable(${BENCH_EXE} ${SOURCE})
    add_executable(${GLOBL_EXE} ${SOURCE})
    add_executable(${LOCAL_EXE} ${SUB_DIR}/all.cc)

    # Set compile options
    target_compile_options(${DEBUG_EXE} PRIVATE ${DEBUG_CFLAGS})
    target_compile_options(${CHECK_EXE} PRIVATE ${CHECK_CFLAGS})
    target_compile_options(${BENCH_EXE} PRIVATE ${BENCH_CFLAGS})
    target_compile_options(${GLOBL_EXE} PRIVATE ${COVER_CFLAGS})
    target_compile_options(${LOCAL_EXE} PRIVATE ${COVER_CFLAGS})

    # Set link options
    target_link_options(${DEBUG_EXE} PRIVATE ${DEBUG_LFLAGS})
    target_link_options(${CHECK_EXE} PRIVATE ${CHECK_LFLAGS})
    target_link_options(${BENCH_EXE} PRIVATE ${BENCH_LFLAGS})
    target_link_options(${GLOBL_EXE} PRIVATE ${COVER_LFLAGS})
    target_link_options(${LOCAL_EXE} PRIVATE ${COVER_LFLAGS})

    # Initialize an empty list for verify files
    set(CHECK_VERIFYS "")
    set(BENCH_VERIFYS "")
    set(GLOBL_VERIFYS "")
    set(LOCAL_VERIFYS "")

    # Loop through each input file
    foreach(INPUT ${INPUTS})
        get_filename_component(TEST_NAME ${INPUT} NAME_WE)
        set(ANSWER ${PROBLEM_DIR}/out/${TEST_NAME}.out)
        set(CHECKER ${PROBLEM_DIR}/checker)

        add_custom_command(
            OUTPUT ${SUB_DIR}/${TEST_NAME}.cc
            COMMAND cp ${SUB_DIR}/all.cc ${SUB_DIR}/${TEST_NAME}.cc
            DEPENDS ${SUB_DIR}/all.cc
        )
        set(COVER_EXE ${SOL_NAME}-${TEST_NAME})
        add_executable(${COVER_EXE} ${SUB_DIR}/${TEST_NAME}.cc)
        target_compile_options(${COVER_EXE} PRIVATE ${COVER_CFLAGS})
        target_link_options(${COVER_EXE} PRIVATE ${COVER_LFLAGS})
        set(COVER_OUTPUT ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.cover.output)
        set(COVER_VERIFY ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.cover)
        list(APPEND LOCAL_VERIFYS ${COVER_VERIFY})
        add_custom_command(
            OUTPUT ${COVER_VERIFY}
            COMMAND ${COVER_EXE} < ${INPUT} > ${COVER_OUTPUT}
            COMMAND ${CHECKER} ${INPUT} ${COVER_OUTPUT} ${ANSWER} 2> ${COVER_VERIFY}
            COMMAND rm ${COVER_OUTPUT}
            DEPENDS ${COVER_EXE} ${CHECKER} ${INPUT} ${ANSWER}
        )

        macro(check_binary PREFIX prefix)
            set(${PREFIX}_OUTPUT ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.${prefix}.output)
            set(${PREFIX}_VERIFY ${CMAKE_BINARY_DIR}/${SOL_NAME}/${TEST_NAME}.${prefix})
            list(APPEND ${PREFIX}_VERIFYS ${${PREFIX}_VERIFY})
            add_custom_command(
                OUTPUT ${${PREFIX}_VERIFY}
                COMMAND ${${PREFIX}_EXE} < ${INPUT} > ${${PREFIX}_OUTPUT}
                COMMAND ${CHECKER} ${INPUT} ${${PREFIX}_OUTPUT} ${ANSWER} 2> ${${PREFIX}_VERIFY}
                COMMAND rm ${${PREFIX}_OUTPUT}
                DEPENDS ${${PREFIX}_EXE} ${CHECKER} ${INPUT} ${ANSWER}
            )
        endmacro()
        check_binary(CHECK check)
        check_binary(BENCH bench)
        check_binary(GLOBL globl)
        check_binary(LOCAL local)

        add_test(
            NAME ${SOL_NAME}-${TEST_NAME}
            COMMAND ${DEBUG_EXE} < ${INPUT}
        )
    endforeach()

    # Add a custom target for check
    add_custom_target(${SOL_NAME}-quick DEPENDS ${CHECK_VERIFYS})
    add_custom_target(${SOL_NAME}-check DEPENDS ${BENCH_VERIFYS})
    add_custom_target(${SOL_NAME}-globl DEPENDS ${GLOBL_VERIFYS})
    add_custom_target(${SOL_NAME}-local DEPENDS ${LOCAL_VERIFYS})

    # Add a custom target for benchmark
    string(REPLACE cpp ${CMAKE_CXX_COMPILER_VERSION}.md SUMMARY ${SOURCE})
    add_custom_target(${SOL_NAME}-bench
        COMMAND ${CMAKE_SOURCE_DIR}/benchmark.py ${BENCH_EXE} ${INPUTS} ${SUMMARY}
        DEPENDS ${CMAKE_SOURCE_DIR}/benchmark.py ${BENCH_EXE} ${INPUTS} ${DEPENDS}
    )

    # Add a custom command for bundle
    add_custom_command(
        OUTPUT ${SUB_DIR}/all.cc
        COMMAND oj-bundle
            -I${CMAKE_SOURCE_DIR}/ac-library
            -I${CMAKE_SOURCE_DIR}/include
            -I${CMAKE_SOURCE_DIR}/problems/common
            ${SOURCE} > ${SUB_DIR}/all.cc
        COMMAND sed -i "'s/^#line/\\/\\/ #line/'" ${SUB_DIR}/all.cc
        DEPENDS ${CHECK_EXE}
    )
    add_custom_command(
        OUTPUT ${SOURCE}.bundle
        COMMAND cat ${CMAKE_SOURCE_DIR}/include/bundle.h ${SUB_DIR}/all.cc > ${SOURCE}.bundle
        COMMAND sed -i "'s/^\\/\\/ #line/#line/'" ${SOURCE}.bundle
        DEPENDS ${SUB_DIR}/all.cc
    )

    # Append targets to lists
    list(APPEND DEBUG ${DEBUG_EXE})
    list(APPEND QUICK ${SOL_NAME}-quick)
    list(APPEND CHECK ${SOL_NAME}-check)
    list(APPEND GLOBL ${SOL_NAME}-globl)
    list(APPEND LOCAL ${SOL_NAME}-local)
    list(APPEND BENCH ${SOL_NAME}-bench)
    list(APPEND BUNDLE ${SOURCE}.bundle)
endforeach()

# Add custom targets for processing all
add_custom_target(debug DEPENDS ${DEBUG})
add_custom_target(quick DEPENDS ${QUICK})
add_custom_target(check DEPENDS ${QUICK} ${CHECK})
add_custom_target(globl
    COMMAND rm -rf global-coverage
    COMMAND lcov --gcov-tool /usr/gcc-14.2.0/bin/gcov -d . -c -o global-coverage.info --exclude "*.cc"
    COMMAND lcov -e global-coverage.info '${CMAKE_SOURCE_DIR}/*' -o global-coverage.info
    COMMAND genhtml -p ${CMAKE_SOURCE_DIR} --demangle-cpp -o global-coverage global-coverage.info
    COMMAND open global-coverage/index.html
    DEPENDS ${GLOBL}
)
add_custom_target(local
    COMMAND rm -rf local-coverage
    COMMAND lcov --gcov-tool /usr/gcc-14.2.0/bin/gcov -d . -c -o local-coverage.info --include "*.cc"
    COMMAND lcov -e local-coverage.info '${CMAKE_SOURCE_DIR}/*' -o local-coverage.info
    COMMAND genhtml -p ${CMAKE_SOURCE_DIR}/sol --demangle-cpp -o local-coverage local-coverage.info
    COMMAND open local-coverage/index.html
    DEPENDS ${LOCAL}
)
add_custom_target(bench DEPENDS ${BENCH})
add_custom_target(bundle DEPENDS ${BUNDLE})
