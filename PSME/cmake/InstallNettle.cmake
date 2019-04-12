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

function(install_nettle)
    include(ConfigurationPackage OPTIONAL)
    assure_package(nettle 3.4.1 "https://ftp.gnu.org/gnu/nettle/nettle-3.4.1.tar.gz" "9bdebb0e2f638d3b9d91f7fc264b70c1")

    set(CONFIGURE_FLAGS)

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)
    list(APPEND CONFIGURE_FLAGS --enable-public-key)
    list(APPEND CONFIGURE_FLAGS --host=i686-pc-linux-gnu)

    set(ENV{CFLAGS} "-m32 -I${CMAKE_BINARY_DIR}/include")
    set(ENV{LDFLAGS} "-L${CMAKE_BINARY_DIR}/lib -lgmp")

    set(ac_cv_va_copy_tmp $ENV{ac_cv_va_copy})
    set(ENV{ac_cv_va_copy} C99)
    execute_process(
        COMMAND ${source_dir}/configure ${CONFIGURE_FLAGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project ${source_dir}")
    endif()
    set(ENV{ac_cv_va_copy} ${ac_cv_va_copy_tmp})

    execute_process(
        COMMAND make ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    file(COPY ${binary_dir}/hogweed.pc ${binary_dir}/nettle.pc
        DESTINATION ${CMAKE_PKG_CONFIG_DIR}
    )
    file(INSTALL ${binary_dir}/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "lib*.so"
    )
    file(INSTALL ${binary_dir}/.lib/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "lib*.so.*"
    )
    file(INSTALL ${binary_dir}/ ${source_dir}/
        DESTINATION ${CMAKE_BINARY_DIR}/include/nettle
        FILES_MATCHING PATTERN "*.h"
    )
endfunction()

install_nettle()
