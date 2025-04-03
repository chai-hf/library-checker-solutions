cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD a9e1cf81-9932-4810-974b-6eccaf14e457)

set(CMAKE_CXX_MODULE_STD ON)

project(toy LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-I${CMAKE_SOURCE_DIR}/ac-library)
add_compile_options(-I${CMAKE_SOURCE_DIR}/include)
add_compile_options(-I${CMAKE_SOURCE_DIR}/problems/common)

set(COMPILE_DEBUG -DLOCAL -UNDEBUG -g3 -O0 -fsanitize=undefined,address -fno-sanitize-recover=all)
set(COMPILE_CHECK -DLOCAL -UNDEBUG -g0 -O3 -fsanitize=undefined,address -fno-sanitize-recover=all)
set(COMPILE_BENCH -DLOCAL -DNDEBUG -g0 -O3 -flto)
set(COMPILE_COVER -ULOCAL -DNDEBUG -g0 -O0 -fprofile-instr-generate -fcoverage-mapping)

set(LINK_DEBUG -fsanitize=undefined,address)
set(LINK_CHECK -fsanitize=undefined,address)
set(LINK_BENCH -flto -static)
set(LINK_COVER -fprofile-instr-generate)

set(COVER_OPTIONS -format=text --show-created-time=0 --show-branches=percent --show-line-counts-or-regions --show-expansions --show-instantiations --show-mcdc)

add_executable(prelude sol/sample/aplusb/main.cpp)
target_compile_options(prelude PRIVATE -DLOCAL -DCLANGD -Wall -Wextra -Wconversion -Wno-sign-conversion)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS sol/*.cpp)

foreach(SOURCE ${SOURCES})
    get_filename_component(SOL ${SOURCE} NAME_WE)
    get_filename_component(SOL_DIR ${SOURCE} DIRECTORY)
    get_filename_component(PROBLEM ${SOL_DIR} NAME_WE)
    string(REPLACE /sol/ /problems/ PROBLEM_DIR ${SOL_DIR})
    set(CHECKER ${PROBLEM_DIR}/checker)

    list(APPEND GENPARAM_PROBLEMS ${PROBLEM_DIR})

    file(SIZE ${SOURCE} FILE_SIZE)
    if(FILE_SIZE EQUAL 0)
        continue()
    endif()

    list(APPEND GENTESTS_PROBLEMS ${PROBLEM_DIR})

    file(GLOB INPUTS CONFIGURE_DEPENDS ${PROBLEM_DIR}/in/*.in)
    if(INPUTS STREQUAL "")
        continue()
    endif()

    set(TARGET ${PROBLEM}-${SOL})
    set(TARGET_DEBUG ${TARGET}.debug)
    set(TARGET_CHECK ${TARGET}.check)
    set(TARGET_BENCH ${TARGET}.bench)
    set(TARGET_COVER ${TARGET}.cover)
    set(TARGET_CPACK ${TARGET}.cpack)

    add_executable(${TARGET_DEBUG} ${SOURCE})
    add_executable(${TARGET_CHECK} ${SOURCE})
    add_executable(${TARGET_BENCH} ${SOURCE})
    add_executable(${TARGET_COVER} ${SOURCE})
    add_executable(${TARGET_CPACK} ${SOURCE}.cxx)

    target_compile_options(${TARGET_DEBUG} PRIVATE ${COMPILE_DEBUG})
    target_compile_options(${TARGET_CHECK} PRIVATE ${COMPILE_CHECK})
    target_compile_options(${TARGET_BENCH} PRIVATE ${COMPILE_BENCH})
    target_compile_options(${TARGET_COVER} PRIVATE ${COMPILE_COVER})
    target_compile_options(${TARGET_CPACK} PRIVATE ${COMPILE_COVER})

    target_link_options(${TARGET_DEBUG} PRIVATE ${LINK_DEBUG})
    target_link_options(${TARGET_CHECK} PRIVATE ${LINK_CHECK})
    target_link_options(${TARGET_BENCH} PRIVATE ${LINK_BENCH})
    target_link_options(${TARGET_COVER} PRIVATE ${LINK_COVER})
    target_link_options(${TARGET_CPACK} PRIVATE ${LINK_COVER})

    set(CHECK_CMDS "")
    set(CHECK_OUTS "")
    set(CHECK_RESS ${TARGET_CHECK}.res)
    set(BENCH_CMDS "")
    set(BENCH_OUTS "")
    set(BENCH_RESS ${TARGET_CHECK}.res ${TARGET_BENCH}.res)
    set(COVER_OUTS "")
    set(COVER_RESS "")
    set(CPACK_OUTS "")
    set(CPACK_RESS "")

    foreach(INPUT ${INPUTS})
        get_filename_component(TEST ${INPUT} NAME_WE)
        set(OUTPUT ${PROBLEM_DIR}/out/${TEST}.out)
        set(PREFIX ${CMAKE_BINARY_DIR}/${TARGET}/${TEST})
        set(CHECK_OUT ${PREFIX}.check.out)
        set(CHECK_TMP ${PREFIX}.check.tmp)
        set(CHECK_RES ${PREFIX}.check.res)
        set(BENCH_OUT ${PREFIX}.bench.out)
        set(BENCH_TMP ${PREFIX}.bench.tmp)
        set(BENCH_RES ${PREFIX}.bench.res)
        set(COVER_OUT ${PREFIX}.cover.profraw)
        set(COVER_TMP ${PREFIX}.cover/coverage${SOURCE}.txt)
        set(COVER_RES ${CMAKE_SOURCE_DIR}/coverage/cover/${PROBLEM}/${SOL}/${TEST}.md)
        set(CPACK_OUT ${PREFIX}.cpack.profraw)
        set(CPACK_TMP ${PREFIX}.cpack/coverage${SOURCE}.cxx.txt)
        set(CPACK_RES ${CMAKE_SOURCE_DIR}/coverage/cpack/${PROBLEM}/${SOL}/${TEST}.md)

        list(APPEND CHECK_CMDS echo ${INPUT} >> ${TARGET_CHECK}.res &&)
        list(APPEND CHECK_CMDS ${CHECKER} ${INPUT} ${CHECK_OUT} ${OUTPUT} 2>> ${TARGET_CHECK}.res &&)
        list(APPEND CHECK_OUTS ${CHECK_OUT})
        list(APPEND CHECK_RESS ${CHECK_RES})
        list(APPEND BENCH_CMDS echo ${INPUT} >> ${TARGET_BENCH}.res &&)
        list(APPEND BENCH_CMDS ${CHECKER} ${INPUT} ${BENCH_OUT} ${OUTPUT} 2>> ${TARGET_BENCH}.res &&)
        list(APPEND BENCH_OUTS ${BENCH_OUT})
        list(APPEND BENCH_RESS ${BENCH_RES})
        list(APPEND COVER_OUTS ${COVER_OUT})
        list(APPEND COVER_RESS ${COVER_RES})
        list(APPEND CPACK_OUTS ${CPACK_OUT})
        list(APPEND CPACK_RESS ${CPACK_RES})

        # ----- debug -----
        add_test(NAME ${TARGET}-${TEST} COMMAND ${TARGET_DEBUG} ${INPUT})

        # ----- check -----
        add_custom_command(
            OUTPUT ${CHECK_RES}
            COMMAND rm -rf ${CHECK_RES}
            COMMAND ${TARGET_CHECK} ${INPUT} > ${CHECK_TMP}
            COMMAND ${CHECKER} ${INPUT} ${CHECK_TMP} ${OUTPUT} 2> ${CHECK_RES}
            COMMAND rm -rf ${CHECK_TMP}
            DEPENDS ${CHECKER} ${INPUT} ${TARGET_CHECK} ${OUTPUT}
        )

        # ----- judge -----
        add_custom_command(
            OUTPUT ${BENCH_RES}
            COMMAND rm -rf ${BENCH_RES}
            COMMAND ${TARGET_BENCH} ${INPUT} > ${BENCH_TMP}
            COMMAND ${CHECKER} ${INPUT} ${BENCH_TMP} ${OUTPUT} 2> ${BENCH_RES}
            COMMAND rm -rf ${BENCH_TMP}
            DEPENDS ${CHECKER} ${INPUT} ${TARGET_BENCH} ${OUTPUT} ${CHECK_RES}
        )

        # ----- cover -----
        add_custom_command(
            OUTPUT ${COVER_OUT}
            COMMAND LLVM_PROFILE_FILE=${COVER_OUT} ./${TARGET_COVER} < ${INPUT} > /dev/null
            DEPENDS ${TARGET_COVER} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${COVER_TMP}
            COMMAND llvm-profdata merge -sparse ${COVER_OUT} -o ${PREFIX}.cover.profdata
            COMMAND llvm-cov show ${TARGET_COVER} -instr-profile=${PREFIX}.cover.profdata ${COVER_OPTIONS} -output-dir=${PREFIX}.cover
            DEPENDS ${COVER_OUT}
        )
        add_custom_command(
            OUTPUT ${COVER_RES}
            COMMAND sed -e '3i```cpp' -e '$$a```' ${COVER_TMP} > ${COVER_RES}
            DEPENDS ${COVER_TMP}
        )

        # ----- cpack -----
        add_custom_command(
            OUTPUT ${CPACK_OUT}
            COMMAND LLVM_PROFILE_FILE=${CPACK_OUT} ./${TARGET_CPACK} < ${INPUT} > /dev/null
            DEPENDS ${TARGET_CPACK} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${CPACK_TMP}
            COMMAND llvm-profdata merge -sparse ${CPACK_OUT} -o ${PREFIX}.cpack.profdata
            COMMAND llvm-cov show ${TARGET_CPACK} -instr-profile=${PREFIX}.cpack.profdata ${COVER_OPTIONS} -output-dir=${PREFIX}.cpack
            DEPENDS ${CPACK_OUT}
        )
        add_custom_command(
            OUTPUT ${CPACK_RES}
            COMMAND sed -e '3i```cpp' -e '$$a```' ${CPACK_TMP} > ${CPACK_RES}
            DEPENDS ${CPACK_TMP}
        )
    endforeach()

    # ----- check -----
    add_custom_command(
        OUTPUT ${TARGET_CHECK}.res
        COMMAND rm -rf ${TARGET_CHECK}.res
        COMMAND ${TARGET_CHECK} ${INPUTS} ${CHECK_OUTS}
        COMMAND ${CHECK_CMDS} rm -rf ${CHECK_OUTS}
        DEPENDS ${CHECKER} ${INPUT} ${TARGET_CHECK} ${OUTPUT}
    )
    add_custom_target(${TARGET}-check DEPENDS ${CHECK_RESS})

    # ----- judge -----
    add_custom_command(
        OUTPUT ${TARGET_BENCH}.res
        COMMAND rm -rf ${TARGET_BENCH}.res
        COMMAND ${TARGET_BENCH} ${INPUTS} ${BENCH_OUTS}
        COMMAND ${BENCH_CMDS} rm -rf ${BENCH_OUTS}
        DEPENDS ${CHECKER} ${INPUT} ${TARGET_BENCH} ${OUTPUT}
    )
    add_custom_target(${TARGET}-judge DEPENDS ${BENCH_RESS})

    # ----- bench -----
    add_custom_target(${TARGET}-bench
        COMMAND ${CMAKE_SOURCE_DIR}/benchmark.py ${TARGET_BENCH} ${INPUTS} ${SOURCE}.md
        DEPENDS ${TARGET_BENCH}
    )

    # ----- cover -----
    add_custom_target(${TARGET}-cover DEPENDS ${COVER_RESS} ${CPACK_RESS})
    add_custom_command(
        OUTPUT ${TARGET}.info
        COMMAND llvm-profdata merge -sparse ${COVER_OUTS} -o ${TARGET}.cover.profdata
        COMMAND llvm-cov show ${TARGET_COVER} -instr-profile=${TARGET}.cover.profdata ${COVER_OPTIONS} -output-dir=${TARGET}-cover
        COMMAND sed -e '3i```cpp' -e '$$a```' ${TARGET}-cover/coverage${SOURCE}.txt > ${CMAKE_SOURCE_DIR}/coverage/cover/${PROBLEM}/${SOL}.md
        COMMAND llvm-profdata merge -sparse ${CPACK_OUTS} -o ${TARGET}.cpack.profdata
        COMMAND llvm-cov show ${TARGET_CPACK} -instr-profile=${TARGET}.cpack.profdata ${COVER_OPTIONS} -output-dir=${TARGET}-cpack
        COMMAND sed -e '3i```cpp' -e '$$a```' ${TARGET}-cpack/coverage${SOURCE}.cxx.txt > ${CMAKE_SOURCE_DIR}/coverage/cpack/${PROBLEM}/${SOL}.md
        COMMAND llvm-cov export --format=lcov --instr-profile=${TARGET}.cover.profdata ${TARGET_COVER} > ${TARGET}.info
        DEPENDS ${COVER_OUTS} ${CPACK_OUTS}
    )

    # ----- cpack -----
    add_custom_command(
        OUTPUT ${SOURCE}.cxx
        COMMAND oj-bundle
            -I${CMAKE_SOURCE_DIR}/ac-library
            -I${CMAKE_SOURCE_DIR}/include
            -I${CMAKE_SOURCE_DIR}/problems/common
            ${SOURCE} > ${SOURCE}.c++ 2> /dev/null
        COMMAND sed -E "'s@^#line [0-9]+ \".*\"@// &@'" ${SOURCE}.c++ > ${SOURCE}.cxx
        COMMAND cat ${CMAKE_SOURCE_DIR}/include/pack.h ${SOURCE}.cxx > ${SOURCE}.c++
        DEPENDS ${TARGET_DEBUG}
    )

    list(APPEND DEBUG ${TARGET}.debug)
    list(APPEND CHECK ${TARGET}-check)
    list(APPEND JUDGE ${TARGET}-judge)
    list(APPEND BENCH ${TARGET}-bench)
    list(APPEND COVER ${TARGET}-cover)
    list(APPEND CINFO ${TARGET}.info)
    list(APPEND CPACK ${SOURCE}.cxx)
endforeach()

add_custom_target(debug DEPENDS ${DEBUG})
add_custom_target(check DEPENDS ${CHECK})
add_custom_target(judge DEPENDS ${JUDGE})
add_custom_target(bench DEPENDS ${BENCH})
add_custom_target(cpack DEPENDS ${CPACK})
add_custom_target(cover
    COMMAND rm -rf ${CMAKE_SOURCE_DIR}/coverage/lcov
    COMMAND genhtml ${CINFO} --demangle-cpp --branch-coverage -o ${CMAKE_SOURCE_DIR}/coverage/lcov
    COMMAND ${CMAKE_SOURCE_DIR}/index.py ${CMAKE_SOURCE_DIR}
    COMMAND open ${CMAKE_SOURCE_DIR}/coverage/index.html
    DEPENDS ${COVER} ${CINFO}
)

list(REMOVE_DUPLICATES GENPARAM_PROBLEMS)
list(REMOVE_DUPLICATES GENTESTS_PROBLEMS)

# ----- genparam -----
foreach(PROBLEM ${GENPARAM_PROBLEMS})
    add_custom_command(
        OUTPUT ${PROBLEM}/params.h
        COMMAND ${CMAKE_SOURCE_DIR}/problems/generate.py --clean ${PROBLEM}/info.toml 2> /dev/null
        DEPENDS ${PROBLEM}/info.toml
    )
    list(APPEND GENPARAM_OUTS ${PROBLEM}/params.h)
endforeach()
add_custom_target(genparam DEPENDS ${GENPARAM_OUTS})

# ----- gentests -----
foreach(PROBLEM ${GENTESTS_PROBLEMS})
    add_custom_command(
        OUTPUT ${PROBLEM}/gen/params.h
        COMMAND ${CMAKE_SOURCE_DIR}/problems/generate.py ${PROBLEM}/info.toml
        COMMAND touch ${PROBLEM}/gen/params.h
        DEPENDS ${PROBLEM}/info.toml
    )
    list(APPEND GENTESTS_OUTS ${PROBLEM}/gen/params.h)
endforeach()
add_custom_target(gentests DEPENDS ${GENTESTS_OUTS})
