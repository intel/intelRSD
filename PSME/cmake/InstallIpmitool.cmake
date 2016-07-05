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

function(ipmitool_install)
    include(ConfigurationPackage OPTIONAL)

    set(ipmitool_source_package
        "http://downloads.sourceforge.net/project/ipmitool/ipmitool/1.8.15/ipmitool-1.8.15.tar.bz2"
    )

    # Checks for library binary.
    find_library(ipmitool_library ipmitool
        PATHS ${CMAKE_BINARY_DIR}/lib
    )

    # Checks for IPMItool interface header.
    find_path(ipmitool_include_intf "ipmitool/ipmi_intf.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    # Checks for IPMItool sol header.
    find_path(ipmitool_include_sol "ipmitool/ipmi_sol.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    # If binary and header files exist then exit because ipmitool library is
    # already installed .
    if (ipmitool_library      AND
        ipmitool_include_intf AND
        ipmitool_include_sol)
        return()
    endif()

    # Sets download directory and sources uri.
    set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
    set(source_package ${ipmitool_source_package})

    get_filename_component(source_package_name ${source_package} NAME)
    string(REGEX REPLACE ".tar.bz2" "" source_dir ${source_package_name})

    # Set final paths do binary and source directory.
    set(source_dir ${CMAKE_BINARY_DIR}/${source_dir})
    set(binary_dir ${source_dir}/build)

    file(MAKE_DIRECTORY ${source_dir})
    file(MAKE_DIRECTORY ${binary_dir})

    # Downloads sources.
    if (NOT EXISTS ${download_dir}/${source_package_name})
        file(DOWNLOAD
            ${source_package}
            ${download_dir}/${source_package_name}
            SHOW_PROGRESS
        )
    endif()

    # Unpacks sources.
    if (EXISTS ${download_dir}/${source_package_name})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xvf
                ${download_dir}/${source_package_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()

    if (EXISTS ${source_dir})
        # IPMItool is a typical application with global variables. To make
        # static library linkable all global variables must be mocked.
        # IPMItool use only 2 global variables to control output information
        # which aren't needed and/or used in library.

        if (CMAKE_CROSSCOMPILING)
            list(APPEND CONFIGURE_FLAGS --host=arm-linux)
        endif()

        # Patches the file to mock global variables.
        execute_process(
            COMMAND patch ${source_dir}/src/plugins/ipmi_intf.c
            -i ${CMAKE_CURRENT_LIST_DIR}/../tools/patches/ipmi_intf.patch
        )

        # Compiles files.
        execute_process(
            COMMAND ${CMAKE_C_COMPILER} -c ${source_dir}/src/plugins/ipmi_intf.c
                             ${source_dir}/lib/log.c
                             ${source_dir}/lib/ipmi_sdr.c
                             ${source_dir}/lib/helper.c
                             ${source_dir}/lib/ipmi_strings.c
                             ${source_dir}/lib/ipmi_sdradd.c
                             ${source_dir}/lib/ipmi_sensor.c
                             ${source_dir}/lib/ipmi_picmg.c
                             ${source_dir}/src/plugins/lan/auth.c
                             ${source_dir}/src/plugins/lan/lan.c
                             ${source_dir}/src/plugins/lan/md5.c
                             ${source_dir}/lib/ipmi_sel.c
                             ${source_dir}/lib/ipmi_oem.c
                             ${source_dir}/lib/hpm2.c
                             ${source_dir}/lib/ipmi_fru.c
                             ${source_dir}/lib/dimm_spd.c
                             -DHAVE_PATHS_H -DIPMI_INTF_LAN
                             -I ${source_dir}/include/
                             -fPIC
        )
        file(GLOB files ${CMAKE_BINARY_DIR}/*.o)

        # Packs object files into static library.
        execute_process(
            COMMAND ar rcs ${binary_dir}/libipmicmd.a ${files}
        )

        # Copies all built binary files to library directory.
        file(INSTALL ${binary_dir}/
            DESTINATION ${CMAKE_BINARY_DIR}/lib
            FILES_MATCHING PATTERN "*.a*"
            PATTERN "CMakeFiles" EXCLUDE
        )

        # Copies all header files to include directory.
        file(INSTALL ${source_dir}/include/ipmitool/
            DESTINATION ${CMAKE_BINARY_DIR}/include/ipmitool
            FILES_MATCHING PATTERN "*.h"
        )

    else()
        message(FATAL_ERROR "${source_dir} not found. Unpack failed?")
    endif()

endfunction()

ipmitool_install()
