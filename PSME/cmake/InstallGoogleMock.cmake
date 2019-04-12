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


function(install_google_mock_framework)
    include(ConfigurationPackage OPTIONAL)
    assure_package(gmock 1.7.0 "https://github.com/google/googlemock/archive/release-1.7.0.zip" "1a01b2efb9253cb6cd3214faef1263da")

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
        COMMAND ${CMAKE_COMMAND} --build ${binary_dir} --target all -- ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    file(INSTALL ${binary_dir}/libgmock.a
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    )
    file(INSTALL ${source_dir}/include
        DESTINATION ${CMAKE_BINARY_DIR}
    )
    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/gmock.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/gmock.pc
        @ONLY
    )
endfunction()

install_google_mock_framework()
