# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompaling.")
    return()
endif()

if (NOT GTEST_FOUND)
    find_file(gtest_library libgtest.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(gtest_library gtest)
    find_path(gtest_include "gtest/gtest.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (gtest_library AND gtest_include)
        add_library(gtest IMPORTED STATIC GLOBAL)
        set_target_properties(gtest PROPERTIES
            IMPORTED_LOCATION ${gtest_library}
        )

        get_filename_component(gtest_library_dir ${gtest_library} DIRECTORY)

        set(GTEST_LIBRARIES gtest pthread)
        set(GTEST_LIBRARY_DIRS ${gtest_library_dir})
        set(GTEST_INCLUDE_DIRS ${gtest_include})
        set(GTEST_FOUND TRUE)
    endif()
endif()

if (NOT GMOCK_FOUND)
    find_file(gmock_library libgmock.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(gmock_library gmock)
    find_path(gmock_include "gmock/gmock.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (gmock_library AND gmock_include)
        add_library(gmock IMPORTED STATIC GLOBAL)
        add_dependencies(gmock gtest)
        set_target_properties(gmock PROPERTIES
            IMPORTED_LOCATION ${gmock_library}
        )

        get_filename_component(gmock_library_dir ${gmock_library} DIRECTORY)

        set(GTEST_LIBRARIES ${GTEST_LIBRARIES} gmock)
        set(GTEST_LIBRARY_DIRS ${GTEST_LIBRARY_DIRS} ${gmock_library_dir})
        set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${gmock_include})
        set(GMOCK_FOUND TRUE)
    endif()
endif()

# associated_target should be an executable, its capabilities should be verified by this test.
# Please note that the final test_target name will be different than the given test_name.
# This function sets a test_target variable which should be used by parent CMakeLists.txt instead.
function(add_gtest test_name associated_target)
    if (NOT TARGET ${associated_target})
        message(FATAL_ERROR "A non existing target ${associated_target} was specified!")
    endif()

    # Test target's name will be renamed so as to be able to quickly find them in the Unix Makefile
    # and run unit tests associated with a specific target.
    # Yes, BOTH set's are required...
    set(test_target unittest_${associated_target}_${test_name})
    set(test_target ${test_target} PARENT_SCOPE)

    set(test_sources)
    foreach (arg ${ARGN})
        set(test_sources ${test_sources} ${arg})
    endforeach()

    foreach (suppression ${VALGRIND_SUPPRESSIONS})
        set(VALGRIND_EXTRA_ARGS ${VALGRIND_EXTRA_ARGS}
            --suppressions=${suppression}
        )
    endforeach()

    set(VALGRIND_MEMCHECK_ARGS
        --tool=memcheck
        --leak-check=full
        --show-leak-kinds=all
        --error-exitcode=1
        --errors-for-leak-kinds=all
        ${VALGRIND_EXTRA_ARGS}
    )

    set(VALGRIND_DRD_ARGS
        --tool=drd
        ${VALGRIND_EXTRA_ARGS}
    )

    add_executable(${test_target} ${test_sources})
    target_link_libraries(${test_target}
        ${GTEST_LIBRARIES}
    )

    target_include_directories(${test_target} SYSTEM PUBLIC
        ${GTEST_INCLUDE_DIRS}
    )

    if (TARGET code-coverage)
        add_dependencies(code-coverage ${test_target})
    endif()

    set_target_properties(${test_target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/bin/tests
    )

    if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
        set_target_properties(${test_target} PROPERTIES
            COMPILE_FLAGS "-Wno-useless-cast -Wno-effc++ -Wno-inline -Wno-zero-as-null-pointer-constant"
        )
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
            set_target_properties(${test_target} PROPERTIES
            COMPILE_FLAGS "-Wno-global-constructors"
        )
    endif()

    find_program(VALGRIND_COMMAND valgrind)

    if (VALGRIND_COMMAND AND (MEMORY_CHECK OR THREADS_CHECK))
        if (MEMORY_CHECK)
            add_test(
                NAME ${test_target}_memcheck
                COMMAND valgrind ${VALGRIND_MEMCHECK_ARGS} $<TARGET_FILE:${test_target}>
            )
        endif()

        if (THREADS_CHECK)
            add_definitions(-DENABLE_VALGRIND_DRD)
            add_test(
                NAME ${test_target}_drd
                COMMAND valgrind ${VALGRIND_DRD_ARGS} $<TARGET_FILE:${test_target}>
	        )
        endif()
    else ()
        add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
    endif ()
endfunction()
