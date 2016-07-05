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

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompaling.")
    return()
endif()

function(install_nl3)
    include(ConfigurationPackage OPTIONAL)

    message("INSTALL NL3")

    set(NL3_SOURCE_PACKAGE
        http://www.infradead.org/~tgr/libnl/files/libnl-3.2.25.tar.gz
    )

    find_library(nl3_library nl-3)

    find_library(nl3_route nl-route-3 libnl-route-3.so.200
        PATHS "/usr/lib/x86_64-linux-gnu/"
    )

    find_path(nl3_include "netlink/netlink.h"
        PATH_SUFFIXES "libnl3"
    )

    if (nl3_library AND nl3_include AND nl3_route)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${NL3_SOURCE_PACKAGE})
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

        list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
        list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
        list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
        list(APPEND CONFIGURE_FLAGS --enable-shared)
        list(APPEND CONFIGURE_FLAGS --disable-static)
        list(APPEND CONFIGURE_FLAGS --disable-cli)
        list(APPEND CONFIGURE_FLAGS --disable-debug)

        set(ac_cv_va_copy_tmp $ENV{ac_cv_va_copy})
        set(ENV{ac_cv_va_copy} C99)
        execute_process(
            COMMAND ${source_dir}/configure ${CONFIGURE_FLAGS}
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )
        set(ENV{ac_cv_va_copy} ${ac_cv_va_copy_tmp})

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

        execute_process(
            COMMAND make install
            WORKING_DIRECTORY ${binary_dir}
            RESULT_VARIABLE result
        )

        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs when build project")
        endif()

        file(INSTALL ${binary_dir}/.libs/
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            FILES_MATCHING PATTERN "libnl-3*.so*"
        )
        file(INSTALL ${source_dir}/include/netlink/netlink.h
            DESTINATION ${CMAKE_BINARY_DIR}/include/netlink
        )
     else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()

endfunction()

install_nl3()
