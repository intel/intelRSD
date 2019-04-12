# <license_header>
#
# Copyright (c) 2019 Intel Corporation
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
function (install_aardvark)

    find_library(aadvark_library libaardvark
        PATHS ${CMAKE_BINARY_DIR}/lib
        )
    find_path(aardvark_include "aardvark.h"
        PATHS ${CMAKE_BINARY_DIR}/include
        )

    if (aardvark_library AND aardvark_include)
        return()
    endif()

    set(sdk_source_package_name "aardvark-api-linux-x86_64-v5.30.zip")
    string(REGEX REPLACE ".zip" "" sdk_source_dir ${sdk_source_package_name})
    set(sdk_source_dir ${CMAKE_BINARY_DIR}/${sdk_source_dir}/c)
    set(library_source_dir "${sdk_source_dir}")

    include(ConfigurationPackage OPTIONAL)
    find_file(aardvark_api_zip ${sdk_source_package_name}
        PATHS ${CMAKE_BINARY_DIR}/../third_party
        )

    if(NOT aardvark_api_zip)
        message(FATAL_ERROR "Error occurs when build project. Please provide and manually put Aardvark Software API (aardvark-api-linux-x86_64 v5.30) library to third_party directory.")
    endif()
    # This url requires login and password and cannot be used for automatic download, plase download this library manually.
    assure_package(aardvark-api-linux-x86_64 v5.30 "https://www.totalphase.com/products/aardvark-software-api/aardvark-api-linux-x86_64-v5.30.zip" "45219dc9d5c1259cd1773b39c57564d8")


    if (EXISTS ${library_source_dir})

        execute_process(
            COMMAND make clean
            WORKING_DIRECTORY ${library_source_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when build project")
        endif()

        execute_process(
            COMMAND make
            WORKING_DIRECTORY ${sdk_source_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when build project")
        endif()

        execute_process(
            COMMAND ar rvs libaardvark.a aardvark.o
            WORKING_DIRECTORY ${sdk_source_dir}/_output
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when build project")
        endif()

        file(INSTALL ${sdk_source_dir}/_output/libaardvark.a
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            )

        file(INSTALL ${sdk_source_dir}/aardvark.h
            DESTINATION ${CMAKE_BINARY_DIR}/include/Aardvark
            )

        set(AARDVARK_VERSION "5.30")
        CONFIGURE_FILE(
            ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/aardvark.pc.in
            ${CMAKE_BINARY_DIR}/pkg-config/aardvark.pc
            @ONLY
        )
    else()
        message("${library_source_dir} not found")
    endif()

endfunction()

install_aardvark()
