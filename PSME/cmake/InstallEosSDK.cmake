# <license_header>
#
# Copyright (c) 2017-2019 Intel Corporation
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

function(install_eos_sdk)
    include(ConfigurationPackage OPTIONAL)
    assure_package(EosSdk 2.3.1 "https://github.com/aristanetworks/EosSdk/archive/v2.3.1.tar.gz" "6ee8678b173878e5138a6bf2756e2135")

    execute_process(
        COMMAND ${source_dir}/build.sh
        WORKING_DIRECTORY ${source_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occured while compiling sources in ${binary_dir}, ${result}")
    endif()

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/eos.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/eos.pc
        @ONLY
    )

    file(INSTALL ${source_dir}/eos
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )

    file(INSTALL ${source_dir}/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libeos.so*"
    )

endfunction()

install_eos_sdk()
