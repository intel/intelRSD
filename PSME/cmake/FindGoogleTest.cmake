# <license_header>
#
# Copyright (c) 2015-2019 Intel Corporation
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
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

# GoogleTest doesn't have pkg config??

if (NOT GTEST_FOUND)
    find_file(gtest_library libgtest.a)
    find_library(gtest_library gtest)

    find_path(gtest_include "gtest/gtest.h")

    if (gtest_library AND gtest_include)
        add_library(gtest IMPORTED STATIC GLOBAL)
        set_target_properties(gtest PROPERTIES
            IMPORTED_LOCATION ${gtest_library}
        )

        get_filename_component(gtest_library_dir ${gtest_library} DIRECTORY)

        set(GTEST_LIBRARIES ${gtest_library} pthread CACHE INTERNAL "Google Tests")
        set(GTEST_LIBRARY_DIRS ${gtest_library_dir} CACHE INTERNAL "Google Tests")
        set(GTEST_INCLUDE_DIRS ${gtest_include} CACHE INTERNAL "Google Tests")
        set(GTEST_FOUND TRUE CACHE INTERNAL "Google Tests")
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

    # append to a list of all targets that have unit tests defined
    set_property(GLOBAL APPEND APPEND_STRING PROPERTY unittest_all_targets
                 ";${associated_target}")

    set(test_sources)
    foreach (arg ${ARGN})
        set(test_sources ${test_sources} ${arg})
    endforeach()

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
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
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

    add_test(NAME ${test_target}
        COMMAND $<TARGET_FILE:${test_target}>
    )
endfunction()
