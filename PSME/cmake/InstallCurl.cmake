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

function(install_curl)
    include(ConfigurationPackage OPTIONAL)
    assure_package(curl 7.64.0 "https://curl.haxx.se/download/curl-7.64.0.tar.gz" "a026740d599a32bcbbe6e70679397899")

    set(CONFIGURE_FLAGS)

    if(DEFINED ENV{ARISTA_PATH})
        string(REGEX REPLACE "/bin/*$" "" ARISTA_PATH "$ENV{ARISTA_PATH}")
    else()
        set(ARISTA_PATH "/opt/arista/fc18-gcc5.4.0")
    endif()

    #set(ENV{PATH} "${CMAKE_BINARY_DIR}/bin:/usr/bin:${PATH}/bin")

    list(APPEND CONFIGURE_FLAGS CFLAGS=-m32)
    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS CPPFLAGS=-I${CMAKE_BINARY_DIR}/include)

    set(ENV{LDFLAGS} "-L${ARISTA_PATH}/lib -L${ARISTA_PATH}/usr/lib -L/lib -Wl,-rpath,${ARISTA_PATH}/lib:${ARISTA_PATH}/usr/lib:/lib")

    list(APPEND CONFIGURE_FLAGS --host=i686-pc-linux-gnu)

    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)
    list(APPEND CONFIGURE_FLAGS --enable-http)
    list(APPEND CONFIGURE_FLAGS --enable-ipv6)
    list(APPEND CONFIGURE_FLAGS --disable-ftp)
    list(APPEND CONFIGURE_FLAGS --disable-file)
    list(APPEND CONFIGURE_FLAGS --disable-ldap)
    list(APPEND CONFIGURE_FLAGS --disable-ldaps)
    list(APPEND CONFIGURE_FLAGS --disable-rtsp)
    list(APPEND CONFIGURE_FLAGS --disable-proxy)
    list(APPEND CONFIGURE_FLAGS --disable-dict)
    list(APPEND CONFIGURE_FLAGS --disable-telnet)
    list(APPEND CONFIGURE_FLAGS --disable-tftp)
    list(APPEND CONFIGURE_FLAGS --disable-pop3)
    list(APPEND CONFIGURE_FLAGS --disable-imap)
    list(APPEND CONFIGURE_FLAGS --disable-smb)
    list(APPEND CONFIGURE_FLAGS --disable-smtp)
    list(APPEND CONFIGURE_FLAGS --disable-gopher)
    list(APPEND CONFIGURE_FLAGS --disable-manual)

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

    file(COPY ${binary_dir}/libcurl.pc
        DESTINATION ${CMAKE_PKG_CONFIG_DIR}
    )
    file(INSTALL ${binary_dir}/lib/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libcurl.so*"
    )
    file(INSTALL ${source_dir}/include/curl
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )
    set(CURL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)
endfunction()

install_curl()
