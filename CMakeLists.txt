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
target_compile_options(prelude PRIVATE -DLOCAL -DCLANGD -Wall -Wextra -Wconversion -Wno-sign-conversion -Werror)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS sol/*.cpp)

foreach(SOURCE ${SOURCES})
    file(SIZE ${SOURCE} FILE_SIZE)
    if(FILE_SIZE EQUAL 0)
        continue()
    endif()

    get_filename_component(SOL ${SOURCE} NAME_WE)
    get_filename_component(SOL_DIR ${SOURCE} DIRECTORY)
    get_filename_component(PROBLEM ${SOL_DIR} NAME_WE)
    string(REPLACE /sol/ /problems/ PROBLEM_DIR ${SOL_DIR})
    set(CHECKER ${PROBLEM_DIR}/checker)

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

    add_custom_command(
        OUTPUT ${SOURCE}.cxx
        COMMAND oj-bundle
            -I${CMAKE_SOURCE_DIR}/ac-library
            -I${CMAKE_SOURCE_DIR}/include
            -I${CMAKE_SOURCE_DIR}/problems/common
            ${SOURCE} > ${SOURCE}.c++ 2> /dev/null
        COMMAND sed -E "'s@^#line [0-9]+ \\\".*\\\"@// &@'" ${SOURCE}.c++ > ${SOURCE}.cxx
        COMMAND cat ${CMAKE_SOURCE_DIR}/include/pack.h ${SOURCE}.cxx > ${SOURCE}.c++
        DEPENDS ${TARGET_DEBUG}
    )

    set(CHECK_VERIFYS "")
    set(BENCH_VERIFYS "")
    set(COVER_OUTPUTS "")
    set(COVER_VERIFYS "")
    set(CPACK_OUTPUTS "")
    set(CPACK_VERIFYS "")

    foreach(INPUT ${INPUTS})
        get_filename_component(TEST ${INPUT} NAME_WE)
        set(OUTPUT ${PROBLEM_DIR}/out/${TEST}.out)
        set(PREFIX ${CMAKE_BINARY_DIR}/${TARGET}/${TEST})
        set(CHECK_OUTPUT ${PREFIX}.check.output)
        set(CHECK_VERIFY ${PREFIX}.check.verify)
        set(BENCH_OUTPUT ${PREFIX}.bench.output)
        set(BENCH_VERIFY ${PREFIX}.bench.verify)
        set(COVER_OUTPUT ${PREFIX}.cover.profraw)
        set(COVER_RESULT ${PREFIX}.cover/coverage${SOURCE}.txt)
        set(COVER_VERIFY ${CMAKE_SOURCE_DIR}/coverage/cover/${PROBLEM}/${SOL}/${TEST}.txt)
        set(CPACK_OUTPUT ${PREFIX}.cpack.profraw)
        set(CPACK_RESULT ${PREFIX}.cpack/coverage${SOURCE}.cxx.txt)
        set(CPACK_VERIFY ${CMAKE_SOURCE_DIR}/coverage/cpack/${PROBLEM}/${SOL}/${TEST}.txt)

        list(APPEND CHECK_VERIFYS ${CHECK_VERIFY})
        list(APPEND BENCH_VERIFYS ${BENCH_VERIFY})
        list(APPEND COVER_OUTPUTS ${COVER_OUTPUT})
        list(APPEND COVER_VERIFYS ${COVER_VERIFY})
        list(APPEND CPACK_OUTPUTS ${CPACK_OUTPUT})
        list(APPEND CPACK_VERIFYS ${CPACK_VERIFY})

        add_test(
            NAME ${TARGET}-${TEST}
            COMMAND ${TARGET_DEBUG} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${CHECK_OUTPUT}
            COMMAND ${TARGET_CHECK} ${INPUT} > ${CHECK_OUTPUT}
            DEPENDS ${TARGET_CHECK} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${CHECK_VERIFY}
            COMMAND ${CHECKER} ${INPUT} ${CHECK_OUTPUT} ${OUTPUT} 2> ${CHECK_VERIFY}
            DEPENDS ${CHECKER} ${INPUT} ${CHECK_OUTPUT} ${OUTPUT}
        )
        add_custom_command(
            OUTPUT ${BENCH_OUTPUT}
            COMMAND ${TARGET_BENCH} ${INPUT} > ${BENCH_OUTPUT}
            DEPENDS ${TARGET_BENCH} ${INPUT} ${CHECK_VERIFY}
        )
        add_custom_command(
            OUTPUT ${BENCH_VERIFY}
            COMMAND ${CHECKER} ${INPUT} ${BENCH_OUTPUT} ${OUTPUT} 2> ${BENCH_VERIFY}
            DEPENDS ${CHECKER} ${INPUT} ${BENCH_OUTPUT} ${OUTPUT}
        )
        add_custom_command(
            OUTPUT ${COVER_OUTPUT}
            COMMAND LLVM_PROFILE_FILE=${COVER_OUTPUT} ./${TARGET_COVER} < ${INPUT} > /dev/null
            DEPENDS ${TARGET_COVER} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${COVER_RESULT}
            COMMAND llvm-profdata merge -sparse ${COVER_OUTPUT} -o ${PREFIX}.cover.profdata
            COMMAND llvm-cov show ${TARGET_COVER} -instr-profile=${PREFIX}.cover.profdata ${COVER_OPTIONS} -output-dir=${PREFIX}.cover
            DEPENDS ${COVER_OUTPUT}
        )
        add_custom_command(
            OUTPUT ${COVER_VERIFY}
            COMMAND cp ${COVER_RESULT} ${COVER_VERIFY}
            DEPENDS ${COVER_RESULT}
        )
        add_custom_command(
            OUTPUT ${CPACK_OUTPUT}
            COMMAND LLVM_PROFILE_FILE=${CPACK_OUTPUT} ./${TARGET_CPACK} < ${INPUT} > /dev/null
            DEPENDS ${TARGET_CPACK} ${INPUT}
        )
        add_custom_command(
            OUTPUT ${CPACK_RESULT}
            COMMAND llvm-profdata merge -sparse ${CPACK_OUTPUT} -o ${PREFIX}.cpack.profdata
            COMMAND llvm-cov show ${TARGET_CPACK} -instr-profile=${PREFIX}.cpack.profdata ${COVER_OPTIONS} -output-dir=${PREFIX}.cpack
            DEPENDS ${CPACK_OUTPUT}
        )
        add_custom_command(
            OUTPUT ${CPACK_VERIFY}
            COMMAND cp ${CPACK_RESULT} ${CPACK_VERIFY}
            DEPENDS ${CPACK_RESULT}
        )
    endforeach()

    add_custom_target(${TARGET}-check DEPENDS ${CHECK_VERIFYS})
    add_custom_target(${TARGET}-judge DEPENDS ${BENCH_VERIFYS})
    add_custom_target(${TARGET}-bench
        COMMAND ${CMAKE_SOURCE_DIR}/benchmark.py ${TARGET_BENCH} ${INPUTS} ${SOURCE}.txt
        DEPENDS ${TARGET_BENCH}
    )
    add_custom_target(${TARGET}-cover DEPENDS ${COVER_VERIFYS} ${CPACK_VERIFYS})
    add_custom_command(
        OUTPUT ${TARGET}.info
        COMMAND llvm-profdata merge -sparse ${COVER_OUTPUTS} -o ${TARGET}.cover.profdata
        COMMAND llvm-cov show ${TARGET_COVER} -instr-profile=${TARGET}.cover.profdata ${COVER_OPTIONS} -output-dir=${TARGET}-cover
        COMMAND cp ${TARGET}-cover/coverage${SOURCE}.txt ${CMAKE_SOURCE_DIR}/coverage/cover/${PROBLEM}/${SOL}.txt
        COMMAND llvm-profdata merge -sparse ${CPACK_OUTPUTS} -o ${TARGET}.cpack.profdata
        COMMAND llvm-cov show ${TARGET_CPACK} -instr-profile=${TARGET}.cpack.profdata ${COVER_OPTIONS} -output-dir=${TARGET}-cpack
        COMMAND cp ${TARGET}-cpack/coverage${SOURCE}.cxx.txt ${CMAKE_SOURCE_DIR}/coverage/cpack/${PROBLEM}/${SOL}.txt
        COMMAND llvm-cov export --format=lcov --instr-profile=${TARGET}.cover.profdata ${TARGET_COVER} > ${TARGET}.info
        DEPENDS ${COVER_OUTPUTS} ${CPACK_OUTPUTS}
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
