# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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

function(install_microhttpd)
    include(ConfigurationPackage OPTIONAL)

    set(MICROHTTPD_SOURCE_PACKAGE
        "http://ftpmirror.gnu.org/libmicrohttpd/libmicrohttpd-0.9.34.tar.gz"
    )

    find_library(microhttpd_library
        NAMES microhttpd
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(microhttpd_include microhttpd.h
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (microhttpd_library AND microhttpd_include)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${MICROHTTPD_SOURCE_PACKAGE})
    get_filename_component(source_package_name ${source_package} NAME)
    string(REGEX REPLACE ".tar.gz" "" source_dir ${source_package_name})
    set(source_dir ${CMAKE_BINARY_DIR}/${source_dir})
    set(binary_dir ${source_dir}/build)
    file(MAKE_DIRECTORY ${source_dir})
    file(MAKE_DIRECTORY ${binary_dir})

    if (NOT EXISTS ${download_dir}/${source_package_name})
        file(DOWNLOAD
            ${source_package}
            ${download_dir}/${source_package_name}
            SHOW_PROGRESS
        )
    endif()

    if (EXISTS ${download_dir}/${source_package_name})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xvf
                ${download_dir}/${source_package_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()

    if (EXISTS ${source_dir})
        set(CONFIGURE_FLAGS)

        if (CMAKE_CROSSCOMPILING)
            list(APPEND CONFIGURE_FLAGS --host=arm-linux)
        endif()

        list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
        list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})

        if (ENABLE_HTTPS)
            if (LIBGCRYPT_PREFIX)
                list(APPEND CONFIGURE_FLAGS --with-libgcrypt-prefix=${LIBGCRYPT_PREFIX})
            endif()
            if (GNUTLS_PREFIX)
                list(APPEND CONFIGURE_FLAGS --with-gnutls=${GNUTLS_PREFIX})
            endif()
        else()
            list(APPEND CONFIGURE_FLAGS --disable-https)
        endif()

        list(APPEND CONFIGURE_FLAGS --disable-static)

        execute_process(
            COMMAND ${source_dir}/configure ${CONFIGURE_FLAGS}
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when configure project")
        endif()

        execute_process(
            COMMAND make ${BUILD_EXTRA_ARGS}
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when build project")
        endif()

        file(INSTALL ${binary_dir}/src/microhttpd/.libs/
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            FILES_MATCHING PATTERN "*microhttpd.so*"
        )
        file(INSTALL ${source_dir}/src/include/microhttpd.h
            DESTINATION ${CMAKE_BINARY_DIR}/include
        )
    else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()
endfunction()

install_microhttpd()
