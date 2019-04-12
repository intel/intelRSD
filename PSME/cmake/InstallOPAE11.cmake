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

function(install_opae11)
    include(ConfigurationPackage OPTIONAL)

    set(OPAE11_VERSION "1.1.0-2")
    assure_package(opae11 ${OPAE11_VERSION} "https://github.com/OPAE/opae-sdk/archive/1.1.0-2.zip" "ba6975ce8a577378d84a420d15660c79")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --target opae11-c opae11-c++ -DBUILD_ASE=0 -DBUILD_ASE_SAMPLES=0 -DBUILD_LIBOPAE_PY=0 ${source_dir}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project ${source_dir}")
    endif()

    execute_process(
        COMMAND make ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}/libopae
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    execute_process(
        COMMAND make ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}/libopaecxx
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    # Copies all header files to include directory.
    file(INSTALL ${source_dir}/common/include/
        DESTINATION ${CMAKE_BINARY_DIR}/include/opae11
        )

    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib/
        FILES_MATCHING PATTERN "libopae11-c.so*"
        )

    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib/
        FILES_MATCHING PATTERN "libopae11-cxx-core.so"
        )

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/opae11.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/opae11.pc
        @ONLY
    )

    link_directories("/opt/psme/lib/")
endfunction()

install_opae11()
