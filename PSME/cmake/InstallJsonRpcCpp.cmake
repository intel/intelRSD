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

function(install_jsonrpccpp)
    include(ConfigurationPackage OPTIONAL)

    set(JSONRPCCPP_SOURCE_PACKAGE
        "https://github.com/cinemast/libjson-rpc-cpp/archive/v0.5.0.zip"
    )

    find_library(jsonrpccpp-common_library libjsonrpccpp-common.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(jsonrpccpp_include "jsonrpccpp/server.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (jsonrpccpp-common_library AND jsonrpccpp_include)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${JSONRPCCPP_SOURCE_PACKAGE})
    get_filename_component(source_package_name ${source_package} NAME)
    string(REGEX REPLACE "v" "" source_package_name ${source_package_name})
    set(source_package_name libjson-rpc-cpp-${source_package_name})
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
        list(APPEND ARGS -DCMAKE_CXX_FLAGS:STRING=-fPIC)
        list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
        list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})
        list(APPEND ARGS -DHTTP_SERVER:BOOL=YES)
        list(APPEND ARGS -DHTTP_CLIENT:BOOL=YES)
        list(APPEND ARGS -DCOMPILE_TESTS:BOOL=NO)
        list(APPEND ARGS -DCOMPILE_STUBGEN:BOOL=NO)
        list(APPEND ARGS -DCOMPILE_EXAMPLES:BOOL=NO)
        list(APPEND ARGS -DBUILD_STATIC_LIBS:BOOL=YES)
        list(APPEND ARGS -DBUILD_SHARED_LIBS:BOOL=NO)

        if (CMAKE_CROSSCOMPILING)
            if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
                list(APPEND ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
            endif()

            list(APPEND ARGS -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
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

        file(INSTALL ${binary_dir}/lib/
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            FILES_MATCHING PATTERN "*.a*"
            PATTERN "CMakeFiles" EXCLUDE
        )
        file(INSTALL ${source_dir}/src/jsonrpccpp/ ${binary_dir}/gen/jsonrpccpp/
            DESTINATION ${CMAKE_BINARY_DIR}/include/jsonrpccpp
            FILES_MATCHING PATTERN "*.h"
        )
    else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()
endfunction()

install_jsonrpccpp()
