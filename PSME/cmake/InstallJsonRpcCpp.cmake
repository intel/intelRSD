# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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
    assure_package(jsonrpccpp 0.6.0 "https://github.com/cinemast/libjson-rpc-cpp/archive/v0.6.0.zip" "846df54332bcd11662b8bba3285f3dd5")

    set(ARGS)
    list(APPEND ARGS -DCMAKE_CXX_FLAGS:STRING=-fPIC)
    list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ARGS -DHTTP_SERVER:BOOL=YES)
    list(APPEND ARGS -DHTTP_CLIENT:BOOL=YES)
    list(APPEND ARGS -DCOMPILE_TESTS:BOOL=NO)
    list(APPEND ARGS -DCOMPILE_STUBGEN:BOOL=NO)
    list(APPEND ARGS -DCOMPILE_EXAMPLES:BOOL=NO)
    list(APPEND ARGS -DBUILD_STATIC_LIBS:BOOL=NO)
    list(APPEND ARGS -DBUILD_SHARED_LIBS:BOOL=YES)

    if (CMAKE_CROSSCOMPILING)
        if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
            list(APPEND ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
        endif()

        list(APPEND ARGS -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
    endif()

    list(APPEND ARGS -DSO_VERSION:STRING=999)
    list(APPEND ARGS -DVERSION_STRING:STRING=${JSONRPCCPP_VERSION})

    execute_process(
        COMMAND ${CMAKE_COMMAND} ${ARGS} ${source_dir}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${binary_dir} --target all -- ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/jsonrpccpp.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/jsonrpccpp.pc
        @ONLY
    )

    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "*.so*"
        PATTERN "CMakeFiles" EXCLUDE
    )

    file(INSTALL ${source_dir}/src/jsonrpccpp/ ${binary_dir}/gen/jsonrpccpp/
        DESTINATION ${CMAKE_BINARY_DIR}/include/jsonrpccpp
        FILES_MATCHING PATTERN "*.h"
    )
endfunction()

install_jsonrpccpp()
