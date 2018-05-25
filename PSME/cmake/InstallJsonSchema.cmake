# <license_header>
#
# Copyright (c) 2015-2018 Intel Corporation
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

function(install_json_schema)
    # Fixed on 1.0.0 but patched to 'master' version (from Feb 9, 2018 10:25:08, commit ad9e9923ab7083f1c542467d759bbf89ecc338b5)
    # Patch file tools/patches/json_schema-1.0.0.patch is used here. Patch for creating static library included.
    assure_package(json_schema 1.0.0 "https://github.com/pboettch/json-schema-validator/archive/1.0.0.zip" "86e10eb93c74b97a583e6438c4d263bd")

    set(ARGS)
    list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

    list(APPEND ARGS -DNLOHMANN_JSON_DIR=${JSON_INCLUDE_DIRS})

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

    file(INSTALL ${binary_dir}/libjson-schema-validator.a
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    )
    file(INSTALL ${source_dir}/src/json-schema.hpp
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )

    CONFIGURE_FILE(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/json_schema.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/json_schema.pc
        @ONLY
    )
endfunction()

install_json_schema()
