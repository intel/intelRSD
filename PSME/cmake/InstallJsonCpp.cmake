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

function(install_jsoncpp)
    include(ConfigurationPackage OPTIONAL)

    set(JSONCPP_SOURCE_PACKAGE
        "https://github.com/open-source-parsers/jsoncpp/archive/1.6.0.zip"
    )

    find_library(jsoncpp_library jsoncpp
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(jsoncpp_include "json/json.h"
        PATH_SUFFIXES jsoncpp
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (jsoncpp_library AND jsoncpp_include)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${JSONCPP_SOURCE_PACKAGE})
    get_filename_component(source_package_name ${source_package} NAME)
    set(source_package_name jsoncpp-${source_package_name})
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
        # TODO passing compiler flags
        set(ARGS)
        list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
        list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

        list(APPEND ARGS -DJSONCPP_WITH_TESTS:BOOL=OFF)
        list(APPEND ARGS -DJSONCPP_WITH_POST_BUILD_UNITTEST:BOOL=OFF)
        list(APPEND ARGS -DJSONCPP_WITH_WARNING_AS_ERROR:BOOL=OFF)
        list(APPEND ARGS -DJSONCPP_WITH_PKGCONFIG_SUPPORT:BOOL=OFF)
        list(APPEND ARGS -DJSONCPP_WITH_CMAKE_PACKAGE:BOOL=OFF)
        list(APPEND ARGS -DJSONCPP_LIB_BUILD_SHARED:BOOL=ON)
        list(APPEND ARGS -DJSONCPP_LIB_BUILD_STATIC:BOOL=OFF)
        list(APPEND ARGS -DBUILD_SHARED_LIBS:BOOL=ON)

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

        file(INSTALL ${binary_dir}/src/lib_json/
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            FILES_MATCHING PATTERN "*.so*"
            PATTERN "CMakeFiles" EXCLUDE
        )
        file(INSTALL ${source_dir}/include/json
            DESTINATION ${CMAKE_BINARY_DIR}/include
        )
    else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()
endfunction()

install_jsoncpp()
