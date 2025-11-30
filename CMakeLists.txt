cmake_minimum_required(VERSION 3.31)

project(toy LANGUAGES CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-std=c++26)
add_compile_options(-march=native)
add_compile_options(-I${CMAKE_SOURCE_DIR}/include)
add_compile_options(-I${CMAKE_SOURCE_DIR}/ac-library)
add_compile_options(-I${CMAKE_SOURCE_DIR}/problems/common)
add_compile_options(-Wall -Wextra -Wconversion -Wno-sign-conversion)

set(COMPILE_DEBUG -g -fsanitize=undefined,address -fno-sanitize-recover=all)
set(COMPILE_CHECK -O -fsanitize=undefined,address -fno-sanitize-recover=all)
set(COMPILE_BENCH -DNDEBUG -O3 -flto)
set(COMPILE_COVER -fprofile-instr-generate -fcoverage-mapping)

set(LINK_DEBUG -fsanitize=undefined,address)
set(LINK_CHECK -fsanitize=undefined,address)
set(LINK_BENCH -flto)
set(LINK_COVER -fprofile-instr-generate)

add_executable(sample sol/sample/aplusb/main.cpp)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS sol/*.cpp)

foreach(SOURCE ${SOURCES})
    get_filename_component(SOL ${SOURCE} NAME_WE)
    get_filename_component(SOL_DIR ${SOURCE} DIRECTORY)
    get_filename_component(PROBLEM ${SOL_DIR} NAME_WE)
    string(REPLACE /sol/ /problems/ PROBLEM_DIR ${SOL_DIR})
    set(CHECKER ${PROBLEM_DIR}/checker)

    file(SIZE ${SOURCE} FILE_SIZE)
    if(FILE_SIZE EQUAL 0)
        continue()
    endif()

    file(GLOB INPUTS CONFIGURE_DEPENDS ${PROBLEM_DIR}/in/*.in)
    if(INPUTS STREQUAL "")
        continue()
    endif()

    set(TARGET ${PROBLEM}-${SOL})
    set(TARGET_DEBUG ${TARGET}.debug)
    set(TARGET_CHECK ${TARGET}.check)
    set(TARGET_BENCH ${TARGET}.bench)
    set(TARGET_COVER ${TARGET}.cover)

    add_executable(${TARGET_DEBUG} ${SOURCE})
    add_executable(${TARGET_CHECK} ${SOURCE})
    add_executable(${TARGET_BENCH} ${SOURCE})
    add_executable(${TARGET_COVER} ${TARGET}.cpp)

    target_compile_options(${TARGET_DEBUG} PRIVATE ${COMPILE_DEBUG})
    target_compile_options(${TARGET_CHECK} PRIVATE ${COMPILE_CHECK})
    target_compile_options(${TARGET_BENCH} PRIVATE ${COMPILE_BENCH})
    target_compile_options(${TARGET_COVER} PRIVATE ${COMPILE_COVER})

    target_link_options(${TARGET_DEBUG} PRIVATE ${LINK_DEBUG})
    target_link_options(${TARGET_CHECK} PRIVATE ${LINK_CHECK})
    target_link_options(${TARGET_BENCH} PRIVATE ${LINK_BENCH})
    target_link_options(${TARGET_COVER} PRIVATE ${LINK_COVER})

    set(CHECK_RESS "")
    set(BENCH_RESS "")
    set(COVER_OUTS "")
    set(COVER_RESS analysis/coverage/${TARGET}.md)

    foreach(INPUT ${INPUTS})
        get_filename_component(TEST ${INPUT} NAME_WE)
        set(OUTPUT ${PROBLEM_DIR}/out/${TEST}.out)
        set(PREFIX ${TARGET}/${TEST})

        # ----- debug -----
        add_test(
            NAME ${TARGET}-${TEST}
            COMMAND ${TARGET_DEBUG} < ${INPUT}
        )

        # ----- check -----
        set(CHECK_OUT ${CMAKE_BINARY_DIR}/${TARGET}/${TEST}.check.out)
        set(CHECK_RES ${CMAKE_BINARY_DIR}/${TARGET}/${TEST}.check.res)
        list(APPEND CHECK_RESS ${CHECK_RES})
        add_custom_command(
            OUTPUT ${CHECK_RES}
            COMMAND rm -rf ${CHECK_RES}
            COMMAND ${TARGET_CHECK} < ${INPUT} > ${CHECK_OUT}
            COMMAND ${CHECKER} ${INPUT} ${CHECK_OUT} ${OUTPUT} 2> ${CHECK_RES}
            COMMAND rm -rf ${CHECK_OUT}
            DEPENDS ${CHECKER} ${INPUT} ${TARGET_CHECK} ${OUTPUT}
        )
        add_custom_target(
            ${TARGET}-check-${TEST}
            DEPENDS ${CHECK_RES}
        )

        # ----- judge -----
        set(BENCH_OUT ${CMAKE_BINARY_DIR}/${TARGET}/${TEST}.bench.out)
        set(BENCH_RES ${CMAKE_BINARY_DIR}/${TARGET}/${TEST}.bench.res)
        list(APPEND BENCH_RESS ${BENCH_RES})
        add_custom_command(
            OUTPUT ${BENCH_RES}
            COMMAND rm -rf ${BENCH_RES}
            COMMAND ${TARGET_BENCH} < ${INPUT} > ${BENCH_OUT}
            COMMAND ${CHECKER} ${INPUT} ${BENCH_OUT} ${OUTPUT} 2> ${BENCH_RES}
            COMMAND rm -rf ${BENCH_OUT}
            DEPENDS ${CHECKER} ${INPUT} ${TARGET_BENCH} ${OUTPUT} ${CHECK_RES}
        )
        add_custom_target(
            ${TARGET}-judge-${TEST}
            DEPENDS ${BENCH_RES}
        )

        # ----- cover -----
        set(COVER_OUT ${TARGET}/${TEST}.profraw)
        set(COVER_RES analysis/coverage/${TARGET}/${TEST}.md)
        list(APPEND COVER_OUTS ${COVER_OUT})
        list(APPEND COVER_RESS ${COVER_RES})
        add_custom_command(
            OUTPUT ${COVER_OUT}
            COMMAND LLVM_PROFILE_FILE=${COVER_OUT} ./${TARGET_COVER} < ${INPUT} > /dev/null
            DEPENDS ${TARGET_COVER} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${COVER_RES}
            COMMAND llvm-profdata merge -sparse ${COVER_OUT} -o ${TARGET}/${TEST}.profdata
            COMMAND llvm-cov show ${TARGET_COVER} -instr-profile=${TARGET}/${TEST}.profdata -format=text -output-dir=${TARGET}/${TEST}
            COMMAND sed -e '1,3d' -e '4i```cpp' -e '$$a```' ${TARGET}/${TEST}/coverage${CMAKE_BINARY_DIR}/${TARGET}.cpp.txt > ${COVER_RES}
            DEPENDS ${COVER_OUTS}
        )
    endforeach()

    # ----- check -----
    add_custom_target(
        ${TARGET}-check
        DEPENDS ${CHECK_RESS}
    )

    # ----- judge -----
    add_custom_target(
        ${TARGET}-judge
        DEPENDS ${BENCH_RESS}
    )

    # ----- bench -----
    add_custom_target(
        ${TARGET}-bench
        COMMAND ${CMAKE_SOURCE_DIR}/benchmark.py ./${TARGET_BENCH} ${INPUTS} analysis/benchmark/${TARGET}.md
        DEPENDS ${TARGET_BENCH}
    )

    # ----- cover -----
    add_custom_command(
        OUTPUT ${TARGET}.cpp
        COMMAND ${CMAKE_SOURCE_DIR}/expander.py -I${CMAKE_SOURCE_DIR}/include
            ${SOURCE} > ${TARGET}.cpp 2> /dev/null
        DEPENDS ${CMAKE_SOURCE_DIR}/expander.py ${TARGET_CHECK}
    )
    set(COVER_RES analysis/coverage/${TARGET}.md)
    add_custom_command(
        OUTPUT ${COVER_RES}
        COMMAND llvm-profdata merge -sparse ${COVER_OUTS} -o ${TARGET}.profdata
        COMMAND llvm-cov show ${TARGET_COVER} -instr-profile=${TARGET}.profdata -format=text -output-dir=${TARGET}
        COMMAND sed -e '1,3d' -e '4i```cpp' -e '$$a```' ${TARGET}/coverage${CMAKE_BINARY_DIR}/${TARGET}.cpp.txt > ${COVER_RES}
        DEPENDS ${COVER_OUTS}
    )
    add_custom_target(${TARGET}-cover DEPENDS ${COVER_RESS})

    # ----- pack -----
    add_custom_command(
        OUTPUT ${SOURCE}.cxx
        COMMAND ${CMAKE_SOURCE_DIR}/expander.py -I${CMAKE_SOURCE_DIR}/include --full
            ${SOURCE} > ${SOURCE}.cxx 2> /dev/null
        DEPENDS ${CMAKE_SOURCE_DIR}/expander.py ${TARGET_CHECK}
    )

    list(APPEND DEBUG ${TARGET}.debug)
    list(APPEND CHECK ${TARGET}-check)
    list(APPEND JUDGE ${TARGET}-judge)
    list(APPEND BENCH ${TARGET}-bench)
    list(APPEND COVER ${TARGET}-cover)
    list(APPEND PACK ${SOURCE}.cxx)
endforeach()

add_custom_target(debug DEPENDS ${DEBUG})
add_custom_target(check DEPENDS ${CHECK})
add_custom_target(judge DEPENDS ${JUDGE})
add_custom_target(bench DEPENDS ${BENCH})
add_custom_target(cover DEPENDS ${COVER})
add_custom_target(pack DEPENDS ${PACK})
add_custom_target(gh-pages
    COMMAND cp ${CMAKE_SOURCE_DIR}/mdbook.md analysis/README.md
    COMMAND ${CMAKE_SOURCE_DIR}/mdbook.py analysis
    COMMAND mdbook build ${CMAKE_SOURCE_DIR} --open
    COMMAND echo 'gitdir: ../.git/worktrees/gh-pages' > ${CMAKE_SOURCE_DIR}/gh-pages/.git
    DEPENDS ${COVER}
)

file(GLOB PROBLEM_INFOS CONFIGURE_DEPENDS problems/*/*/info.toml)

foreach(PROBLEM_INFO ${PROBLEM_INFOS})
    get_filename_component(PROBLEM_DIR ${PROBLEM_INFO} DIRECTORY)
    get_filename_component(PROBLEM ${PROBLEM_DIR} NAME_WE)
    string(REPLACE /problems/ /sol/ SOL_DIR ${PROBLEM_DIR})

    if("${PROBLEM_DIR}" MATCHES "/test/")
        continue()
    endif()

    add_custom_command(
        OUTPUT ${PROBLEM_DIR}/params.h
        COMMAND mkdir -p ${SOL_DIR}
        COMMAND touch ${SOL_DIR}/main.cpp
        COMMAND ${CMAKE_SOURCE_DIR}/problems/generate.py --clean ${PROBLEM_INFO} 2> /dev/null
        DEPENDS ${PROBLEM_INFO}
    )
    list(APPEND PARAM_OUTS ${PROBLEM_DIR}/params.h)

    add_custom_command(
        OUTPUT ${PROBLEM_DIR}/gen/params.h
        COMMAND ${CMAKE_SOURCE_DIR}/problems/generate.py ${PROBLEM_INFO}
        COMMAND touch ${PROBLEM_DIR}/gen/params.h
        DEPENDS ${PROBLEM_INFO}
    )
    add_custom_target(gentests-${PROBLEM} DEPENDS ${PROBLEM_DIR}/gen/params.h)

    if(NOT EXISTS ${SOL_DIR}/main.cpp)
        continue()
    endif()
    file(SIZE ${SOL_DIR}/main.cpp FILE_SIZE)
    if(FILE_SIZE EQUAL 0)
        continue()
    endif()

    list(APPEND TESTS_OUTS ${PROBLEM_DIR}/gen/params.h)
endforeach()

add_custom_target(genparam DEPENDS ${PARAM_OUTS})
add_custom_target(gentests DEPENDS ${TESTS_OUTS})
