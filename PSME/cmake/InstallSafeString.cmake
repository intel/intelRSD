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

include(InstallScriptsCommon)

function(install_safe_string)
    include(ConfigurationPackage OPTIONAL)

    set(SAFE_STRING_SOURCE_DIR
        ${CMAKE_CURRENT_LIST_DIR}/../common/safe-string
        CACHE PATH "Safe string library source path"
    )

    find_library(safe_string_library safe-string
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(safe_string_include "safe-string/safe_lib.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (safe_string_library AND safe_string_include)
        return()
    endif()

    set(source_dir ${SAFE_STRING_SOURCE_DIR})
    set(binary_dir ${CMAKE_BINARY_DIR}/safe-string/build)
    file(MAKE_DIRECTORY ${binary_dir})

    if (EXISTS ${source_dir})
        init_cmake_args(ARGS)

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

install_safe_string()
