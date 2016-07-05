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

function(install_uuid)
    include(ConfigurationPackage OPTIONAL)

    set(UUID_SOURCE_PACKAGE
        "ftp://ftp.ossp.org/pkg/lib/uuid/uuid-1.6.2.tar.gz"
    )

    find_library(uuid_library uuid++
        NAMES ossp-uuid++
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_path(uuid_include uuid++.hh
        PATH_SUFFIXES ossp
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (uuid_library AND uuid_include)
        return()
    endif()

    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${UUID_SOURCE_PACKAGE})
    get_filename_component(source_package_name ${source_package} NAME)
    string(REGEX REPLACE ".tar.gz" "" source_dir ${source_package_name})
    set(source_dir ${CMAKE_BINARY_DIR}/${source_dir})
    set(binary_dir ${source_dir})
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

        file(INSTALL ${binary_dir}/.libs/
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            FILES_MATCHING PATTERN "*uuid++.so*"
        )
        file(INSTALL ${source_dir}/uuid.h ${source_dir}/uuid++.hh
            DESTINATION ${CMAKE_BINARY_DIR}/include
        )
    else()
        message(FATAL_ERROR "${source_dir} not found")
    endif()
endfunction()

install_uuid()
