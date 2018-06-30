#
# Tests configuration
#

add_library(odr OBJECT odr.cpp)

set_target_properties(odr PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(odr
    PRIVATE $<TARGET_PROPERTY:EnTT,INTERFACE_INCLUDE_DIRECTORIES>
)

target_compile_definitions(odr
    PRIVATE $<TARGET_PROPERTY:EnTT,INTERFACE_COMPILE_DEFINITIONS>
)

target_compile_options(odr
    PRIVATE $<TARGET_PROPERTY:EnTT,INTERFACE_COMPILE_OPTIONS>
    PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-pedantic -Wall>
)

target_compile_features(odr
    PRIVATE $<TARGET_PROPERTY:EnTT,INTERFACE_COMPILE_FEATURES>
)

# make GTest available

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

option(BUILD_BENCHMARK "Build benchmark." OFF)
option(BUILD_MOD "Build mod example." OFF)
option(BUILD_SNAPSHOT "Build snapshot example." OFF)

find_path(GOOGLETEST_DEPS_DIR src/gtest.cc
    ${entt_SOURCE_DIR}/deps/googletest
    /usr/src/googletest/googletest
    /usr/src/gtest
    )
find_package(GTest)
set(GTEST_DEPENDENCIES)

if(NOT GTEST_FOUND AND EXISTS ${GTEST_ROOT})
    include(ExternalProject)
    ExternalProject_Add(gtest PREFIX ./gtest
        SOURCE_DIR ${GTEST_ROOT}
        INSTALL_COMMAND true)

    link_directories(${CMAKE_CURRENT_BINARY_DIR}/gtest/src/gtest-build)

    set(GTEST_LIBRARIES "-lgtest")
    set(GTEST_DEPENDENCIES "gtest")
    set(GTEST_FOUND TRUE)
    find_path(GTEST_INCLUDE_DIRS NAMES gtest/gtest.h PATHS ${GTEST_ROOT}/include)

    message(STATUS "Found GTest at ${GTEST_ROOT}, headers at ${GTEST_INCLUDE_DIRS}")
endif()

if(NOT GTEST_FOUND)
    message(FATAL_ERROR "GTest is required to build and run the tests")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

macro(ADD_ENTT_TEST TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} $<TARGET_OBJECTS:odr> ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE EnTT gtest_main Threads::Threads ${GTEST_LIBRARIES})
    target_include_directories(${TEST_NAME} PRIVATE ${GTEST_INCLUDE_DIRS})
    if (GTEST_DEPENDENCIES)
        add_dependencies(${TEST_NAME} ${GTEST_DEPENDENCIES})
    endif()
    set_target_properties(${TEST_NAME} PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_options(${TEST_NAME} PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-pedantic -Wall>)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# Test benchmark

if(BUILD_BENCHMARK)
    ADD_ENTT_TEST(benchmark benchmark/benchmark.cpp)
endif()

# Test mod

if(BUILD_MOD)
    set(DUKTAPE_DEPS_DIR ${entt_SOURCE_DIR}/deps/duktape)
    configure_file(${entt_SOURCE_DIR}/cmake/in/duktape.in ${DUKTAPE_DEPS_DIR}/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${DUKTAPE_DEPS_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${DUKTAPE_DEPS_DIR})
    set(DUKTAPE_SRC_DIR ${DUKTAPE_DEPS_DIR}/src/src)

    set(MOD_TEST_SOURCE ${DUKTAPE_SRC_DIR}/duktape.c mod/mod.cpp)
    ADD_ENTT_TEST(mod "${MOD_TEST_SOURCE}")
    target_include_directories(mod PRIVATE ${DUKTAPE_SRC_DIR})
endif()

# Test snapshot

if(BUILD_SNAPSHOT)
    set(CEREAL_DEPS_DIR ${entt_SOURCE_DIR}/deps/cereal)
    configure_file(${entt_SOURCE_DIR}/cmake/in/cereal.in ${CEREAL_DEPS_DIR}/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${CEREAL_DEPS_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CEREAL_DEPS_DIR})
    set(CEREAL_SRC_DIR ${CEREAL_DEPS_DIR}/src/include)

    ADD_ENTT_TEST(cereal snapshot/snapshot.cpp)
    target_include_directories(cereal PRIVATE ${CEREAL_SRC_DIR})
endif()

# Test core

ADD_ENTT_TEST(algorithm entt/core/algorithm.cpp)
ADD_ENTT_TEST(family entt/core/family.cpp)
ADD_ENTT_TEST(hashed_string entt/core/hashed_string.cpp)
ADD_ENTT_TEST(ident entt/core/ident.cpp)
ADD_ENTT_TEST(monostate entt/core/monostate.cpp)

# Test entity

ADD_ENTT_TEST(actor entt/entity/actor.cpp)
ADD_ENTT_TEST(entity entt/entity/entity.cpp)
ADD_ENTT_TEST(helper entt/entity/helper.cpp)
ADD_ENTT_TEST(prototype entt/entity/prototype.cpp)
ADD_ENTT_TEST(registry entt/entity/registry.cpp)
ADD_ENTT_TEST(snapshot entt/entity/snapshot.cpp)
ADD_ENTT_TEST(sparse_set entt/entity/sparse_set.cpp)
ADD_ENTT_TEST(view entt/entity/view.cpp)

# Test locator

ADD_ENTT_TEST(locator entt/locator/locator.cpp)

# Test process

ADD_ENTT_TEST(process entt/process/process.cpp)
ADD_ENTT_TEST(scheduler entt/process/scheduler.cpp)

# Test resource

ADD_ENTT_TEST(resource entt/resource/resource.cpp)

# Test signal

ADD_ENTT_TEST(delegate entt/signal/delegate.cpp)
ADD_ENTT_TEST(dispatcher entt/signal/dispatcher.cpp)
ADD_ENTT_TEST(emitter entt/signal/emitter.cpp)
ADD_ENTT_TEST(sigh entt/signal/sigh.cpp)
