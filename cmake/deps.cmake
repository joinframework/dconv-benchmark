include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googlebenchmark
    SYSTEM
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.10.0
    GIT_SHALLOW    TRUE
)
set(BENCHMARK_ENABLE_TESTING     OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL     OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

FetchContent_Declare(
    dconv
    GIT_REPOSITORY https://github.com/joinframework/dconv.git
    GIT_TAG        v1.1.0
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(dconv)
add_library(dconv INTERFACE)
add_library(dconv::dconv ALIAS dconv)
target_include_directories(dconv INTERFACE ${dconv_SOURCE_DIR}/dconv/include)

FetchContent_Declare(
    ryu
    GIT_REPOSITORY https://github.com/ulfjack/ryu.git
    GIT_TAG        4c0618b0e44f7ef027ebae05d2cc7812048f7c8f
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(ryu)
add_library(ryu STATIC ${ryu_SOURCE_DIR}/ryu/d2s.c ${ryu_SOURCE_DIR}/ryu/s2d.c)
add_library(ryu::ryu ALIAS ryu)
target_include_directories(ryu SYSTEM PUBLIC ${ryu_SOURCE_DIR})

FetchContent_Declare(
    dragonbox
    GIT_REPOSITORY https://github.com/jk-jeon/dragonbox.git
    GIT_TAG        1.1.3
    GIT_SHALLOW    TRUE
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(dragonbox)
add_library(dragonbox STATIC ${dragonbox_SOURCE_DIR}/source/dragonbox_to_chars.cpp)
add_library(dragonbox::dragonbox ALIAS dragonbox)
target_include_directories(dragonbox SYSTEM PUBLIC ${dragonbox_SOURCE_DIR}/include)

FetchContent_Declare(
    fmt
    SYSTEM
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        12.2.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    double_conversion
    GIT_REPOSITORY https://github.com/google/double-conversion.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_MakeAvailable(double_conversion)
unset(CMAKE_POLICY_VERSION_MINIMUM)

FetchContent_Declare(
    fast_float
    GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
    GIT_TAG        v8.3.2
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(fast_float)

FetchContent_Declare(
    drachennest
    GIT_REPOSITORY https://github.com/abolz/Drachennest.git
    GIT_TAG        e6714a39ad331b4489d0b6aaf3968635bff4eb5e
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(drachennest)
add_library(drachennest STATIC
    ${drachennest_SOURCE_DIR}/src/dragon4.cc
    ${drachennest_SOURCE_DIR}/src/grisu3.cc
    ${drachennest_SOURCE_DIR}/src/schubfach_64.cc
)
add_library(drachennest::drachennest ALIAS drachennest)
target_include_directories(drachennest SYSTEM PUBLIC ${drachennest_SOURCE_DIR}/src)

FetchContent_Declare(
    teju
    GIT_REPOSITORY https://github.com/cassioneri/teju_jagua.git
    GIT_TAG        4403283bda3e7145d643496231f3c4b5509dd379
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(teju)
add_library(teju STATIC ${teju_SOURCE_DIR}/teju/src/generated/ieee64_with_uint128.c)
add_library(teju::teju ALIAS teju)
target_include_directories(teju SYSTEM PUBLIC ${teju_SOURCE_DIR} ${teju_SOURCE_DIR}/teju/include)
target_compile_definitions(teju PUBLIC teju_has_uint128)

FetchContent_Declare(
    milo
    GIT_REPOSITORY https://github.com/miloyip/dtoa-benchmark.git
    GIT_TAG        bf1fb58ade01658c908a498679e47f0b4e89aff7
    SOURCE_SUBDIR  none
)
FetchContent_MakeAvailable(milo)
add_library(milo INTERFACE)
add_library(milo::milo ALIAS milo)
target_include_directories(milo SYSTEM INTERFACE ${milo_SOURCE_DIR}/src/milo)

add_library(gay STATIC ${milo_SOURCE_DIR}/src/gay/dtoa.c ${milo_SOURCE_DIR}/src/gay/g_fmt.c)
add_library(gay::gay ALIAS gay)
set_source_files_properties(${milo_SOURCE_DIR}/src/gay/g_fmt.c PROPERTIES LANGUAGE CXX)
target_compile_definitions(gay PRIVATE strtod=gay_strtod Long=int)
target_compile_options(gay PRIVATE -w -fno-strict-aliasing)
