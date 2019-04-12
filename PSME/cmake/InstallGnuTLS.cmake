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

function(install_gnutls)
    include(ConfigurationPackage OPTIONAL)
    assure_package(gnutls 3.6.7 "https://www.gnupg.org/ftp/gcrypt/gnutls/v3.6/gnutls-3.6.7.tar.xz" "c4ac669c500df939d4fbfea722367929")

    set(CONFIGURE_FLAGS)

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)
    list(APPEND CONFIGURE_FLAGS --disable-doc)
    list(APPEND CONFIGURE_FLAGS --with-included-libtasn1)
    list(APPEND CONFIGURE_FLAGS --with-included-unistring)
    list(APPEND CONFIGURE_FLAGS --without-p11-kit)
    list(APPEND CONFIGURE_FLAGS --without-tpm)
    list(APPEND CONFIGURE_FLAGS --disable-hardware-acceleration)
    list(APPEND CONFIGURE_FLAGS --host=i686-pc-linux-gnu)

    set(ENV{CFLAGS} "-m32")
    set(ENV{ABI} "32")
    set(ENV{LDFLAGS} "-L${CMAKE_BINARY_DIR}/lib -Wl,-R/opt/psme/lib/ -lnettle -lgmp")

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

    file(COPY ${binary_dir}/lib/gnutls.pc ${binary_dir}/libdane/gnutls-dane.pc
        DESTINATION ${CMAKE_PKG_CONFIG_DIR}
    )
    file(INSTALL ${binary_dir}/lib/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libgnutls*.so*"
    )
    file(INSTALL ${source_dir}/lib/includes/gnutls
        DESTINATION ${CMAKE_BINARY_DIR}/include
        FILES_MATCHING PATTERN "*.h"
    )
    file(INSTALL ${binary_dir}/lib/includes/gnutls/gnutls.h
        DESTINATION ${CMAKE_BINARY_DIR}/include/gnutls
    )
endfunction()

install_gnutls()
