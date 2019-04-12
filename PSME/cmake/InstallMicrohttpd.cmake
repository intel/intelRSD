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

function(install_libmicrohttpd)
    include(ConfigurationPackage OPTIONAL)
    assure_package(libmicrohttpd 0.9.62 "https://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.62.tar.gz" "97717a00ad3f883c2755fd409406cf31")

    set(CONFIGURE_FLAGS)

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)
    list(APPEND CONFIGURE_FLAGS --with-cxx)

    set(ENV{CFLAGS} "-m32 -I${CMAKE_BINARY_DIR}/include")
    set(ENV{LDFLAGS} "-L${CMAKE_BINARY_DIR}/lib")
    set(ENV{LD_LIBRARY_PATH} "${CMAKE_BINARY_DIR}/lib")
    set(ENV{PATH} "${CMAKE_BINARY_DIR}/bin:/usr/bin:${PATH}")

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

    file(COPY ${binary_dir}/libmicrohttpd.pc
        DESTINATION ${CMAKE_PKG_CONFIG_DIR}
    )
    file(INSTALL ${binary_dir}/src/microhttpd/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libmicrohttpd.so*"
    )
    set(MHD_LIBRARY ${CMAKE_BINARY_DIR}/lib/libmicrohttpd.so)
    file(INSTALL ${source_dir}/src/include/microhttpd.h
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )
endfunction()

install_libmicrohttpd()
