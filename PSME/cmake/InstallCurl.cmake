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

function(install_curl)
    include(ConfigurationPackage OPTIONAL)

    set(CURL_SOURCE_PACKAGE
        "http://curl.haxx.se/download/curl-7.41.0.tar.gz"
    )

    find_library(curl_library curl
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(curl_include "curl/curl.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (curl_library AND curl_include)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${CURL_SOURCE_PACKAGE})
    get_filename_component(source_package_name ${source_package} NAME)
    string(REGEX REPLACE ".tar.gz" "" source_dir ${source_package_name})
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

        list(APPEND ARGS -DBUILD_CURL_TESTS:BOOL=OFF)
        list(APPEND ARGS -DBUILD_CURL_EXE:BOOL=OFF)
        list(APPEND ARGS -DCMAKE_USE_OPENSSL:BOOL=OFF)
        list(APPEND ARGS -DHAVE_GLIBC_STRERROR_R:INTERNAL=FAILED_TO_RUN)
        list(APPEND ARGS -DHAVE_GLIBC_STRERROR_R__TRYRUN_OUTPUT:INTERNAL=)
        list(APPEND ARGS -DHAVE_POSIX_STRERROR_R:INTERNAL=0)
        list(APPEND ARGS -DHAVE_POSIX_STRERROR_R__TRYRUN_OUTPUT:INTERNAL=)
        list(APPEND ARGS -DHAVE_POLL_FINE_EXITCODE:INTERNAL=FAILED_TO_RUN)

        if (CMAKE_CROSSCOMPILING)
            if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
                list(APPEND ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
            endif()

            list(APPEND ARGS -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
        endif()

        if (CMAKE_BUILD_TYPE)
            list(APPEND ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
        endif()

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

        execute_process(
            COMMAND ${CMAKE_COMMAND} --build ${binary_dir} --target install
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when install project")
        endif()
    else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()
endfunction()

install_curl()
