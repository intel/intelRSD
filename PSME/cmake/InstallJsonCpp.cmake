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

# check if package exists in the third_party directory
# download and unpack otherwise


function(install_jsoncpp)
    assure_package("jsoncpp" "1.7.2" "https://github.com/open-source-parsers/jsoncpp/archive/1.7.2.zip" "ea9e564eb06fe609f46c7b37fb696ff0")

    set(ARGS)
    list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

    list(APPEND ARGS -DJSONCPP_WITH_TESTS:BOOL=OFF)
    list(APPEND ARGS -DJSONCPP_WITH_WARNING_AS_ERROR:BOOL=OFF)
    list(APPEND ARGS -DJSONCPP_WITH_PKGCONFIG_SUPPORT:BOOL=ON)
    list(APPEND ARGS -DJSONCPP_WITH_CMAKE_PACKAGE:BOOL=OFF)
    list(APPEND ARGS -DBUILD_SHARED_LIBS:BOOL=ON)
    list(APPEND ARGS -DBUILD_STATIC_LIBS:BOOL=OFF)
    list(APPEND ARGS -DJSONCPP_WITH_POST_BUILD_UNITTEST:BOOL=OFF)

    if (CMAKE_CROSSCOMPILING)
        if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
            list(APPEND ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
        endif()

        list(APPEND ARGS -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
    endif()

    if (CMAKE_BUILD_TYPE)
        list(APPEND ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
    endif()

    list(APPEND ARGS -DJSONCPP_SOVERSION:STRING=999)
    list(APPEND ARGS -DJSONCPP_VERSION:STRING=${JSONCPP_VERSION})

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

    file(COPY ${binary_dir}/pkg-config/jsoncpp.pc DESTINATION ${CMAKE_PKG_CONFIG_DIR})

    file(INSTALL ${binary_dir}/src/lib_json/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "*.so*"
        PATTERN "CMakeFiles" EXCLUDE
    )

    file(INSTALL ${source_dir}/include/json
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink ../${source_name}/include jsoncpp
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/include
    )
endfunction()

install_jsoncpp()
