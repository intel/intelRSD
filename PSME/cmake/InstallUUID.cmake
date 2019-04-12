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

function(install_uuid)
    include(ConfigurationPackage OPTIONAL)
    assure_package(uuid 1.6.2 "http://www.mirrorservice.org/sites/ftp.ossp.org/pkg/lib/uuid/uuid-1.6.2.tar.gz" "5db0d43a9022a6ebbbc25337ae28942f")

    set(CONFIGURE_FLAGS)
    if (CMAKE_CROSSCOMPILING)
        list(APPEND CONFIGURE_FLAGS --host=arm-linux)
    endif()

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)
    list(APPEND CONFIGURE_FLAGS --with-cxx)
    list(APPEND CONFIGURE_FLAGS --without-dce)
    list(APPEND CONFIGURE_FLAGS --without-perl)
    list(APPEND CONFIGURE_FLAGS --without-perl-compat)
    list(APPEND CONFIGURE_FLAGS --without-php)
    list(APPEND CONFIGURE_FLAGS --without-pgsql)

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

    file(COPY
        ${binary_dir}/uuid.pc
        DESTINATION ${CMAKE_PKG_CONFIG_DIR}
    )
    file(INSTALL ${binary_dir}/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libuuid*.so*"
    )
    file(INSTALL ${binary_dir}/uuid.h ${source_dir}/uuid++.hh
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )
endfunction()

install_uuid()
