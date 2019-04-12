# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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

function(install_opae)
    include(ConfigurationPackage OPTIONAL)

    set(OPAE_VERSION "1.3.0-2")
    assure_package(opae ${OPAE_VERSION} "https://github.com/OPAE/opae-sdk/archive/1.3.0-2.zip" "8a3e69afd1875969b34136b6cfd3600b")

    execute_process(
        COMMAND ${CMAKE_COMMAND} --target opae-c++ -DENABLE_LIBFPGAOF_RDMA_TRANSPORT=ON -DBUILD_OPAE_PROXY=1 -DBUILD_ASE=0 -DBUILD_ASE_SAMPLES=0 -DBUILD_LIBOPAE_PY=0 -DCPACK_GENERATOR=DEB ${source_dir}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project ${source_dir}")
    endif()

    set(BUILD_OPAE_DEBS package_deb)
    execute_process(
        COMMAND make ${BUILD_EXTRA_ARGS} ${BUILD_OPAE_DEBS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    # Copies all header files to include directory.
    file(INSTALL ${source_dir}/common/include/
        DESTINATION ${CMAKE_BINARY_DIR}/include/opae
        )
    file(INSTALL ${source_dir}/samples/opae-proxy/include/
        DESTINATION ${CMAKE_BINARY_DIR}/include/opae
        )

    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libxfpga.so*"
        )
    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libopae-c.so*"
        )
    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libopae-cxx-core.so"
        )
    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libopae-proxy.so"
        )
    file(INSTALL ${binary_dir}/lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libfpgaof.so*"
        )
    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/opae.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/opae.pc
        @ONLY
    )
endfunction()

install_opae()
