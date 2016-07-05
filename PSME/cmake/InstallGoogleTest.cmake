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

# TODO pass compiler flags

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompaling.")
    return()
endif()

function(install_google_test_framework)
    include(ConfigurationPackage OPTIONAL)

    set(GTEST_SOURCE_PACKAGE
        "https://googletest.googlecode.com/files/gtest-1.7.0.zip"
    )

    find_library(gtest_library gtest
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(gtest_include "gtest/gtest.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (gtest_library AND gtest_include)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${GTEST_SOURCE_PACKAGE})
    get_filename_component(source_package_name ${source_package} NAME)
    string(REGEX REPLACE ".zip" "" source_dir ${source_package_name})
    set(source_dir ${CMAKE_BINARY_DIR}/${source_dir})
    set(binary_dir ${source_dir}/build)
    file(MAKE_DIRECTORY ${source_dir})
    file(MAKE_DIRECTORY ${binary_dir})

    if (NOT EXISTS ${download_dir}/${source_package_name})
        file(DOWNLOAD
            ${source_package}
            ${download_dir}/${source_package_name}
            SHOW_PROGRESS
        )
    endif()

    if (EXISTS ${download_dir}/${source_package_name})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xvf
                ${download_dir}/${source_package_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()

    if (EXISTS ${source_dir})
        set(ARGS)
        list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
        list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

        if (CMAKE_CROSSCOMPILING)
            if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
                list(APPEND ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
            endif()

            list(APPEND ARGS -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
        endif()

        if (CMAKE_BUILD_TYPE)
            list(APPEND ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
        endif()

        list(APPEND ARGS -DCMAKE_CXX_FLAGS:STRING=-fPIC)

        execute_process(
            COMMAND ${CMAKE_COMMAND} ${ARGS} ${source_dir}
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when configure project")
        endif()

        execute_process(
            COMMAND ${CMAKE_COMMAND} --build ${binary_dir} --target all
                -- ${BUILD_EXTRA_ARGS}
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when build project")
        endif()

        file(INSTALL ${binary_dir}/libgtest.a
            DESTINATION ${CMAKE_BINARY_DIR}/lib
        )
        file(INSTALL ${source_dir}/include
            DESTINATION ${CMAKE_BINARY_DIR}
        )
    else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()
endfunction()

install_google_test_framework()
