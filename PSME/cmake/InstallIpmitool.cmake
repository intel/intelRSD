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

function(ipmitool_install)
    include(ConfigurationPackage OPTIONAL)
    assure_package(ipmitool 1.8.17 "http://downloads.sourceforge.net/project/ipmitool/ipmitool/1.8.17/ipmitool-1.8.17.tar.bz2" "f7408aa2b40333db0413d4aab6bbe978")

    set(shared FALSE)
    if (shared)
        set(target "-shared -Wl,-soname,libipmicmd.so.999 -o ${binary_dir}/libipmicmd.so")
    else()
        set(target "-c")
    endif()

    # Compiles files.
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -fPIC -std=gnu99 ${target}
            ${source_dir}/src/plugins/ipmi_externs.c
            ${source_dir}/src/plugins/ipmi_intf.c
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
            ${source_dir}/src/plugins/serial/serial_oem.c
            ${source_dir}/lib/ipmi_sel.c
            ${source_dir}/lib/ipmi_oem.c
            ${source_dir}/lib/hpm2.c
            ${source_dir}/lib/ipmi_fru.c
            ${source_dir}/lib/dimm_spd.c
            -DHAVE_PATHS_H -DIPMI_INTF_LAN -DIPMI_INTF_SERIAL_OEM
            -I ${source_dir}/include/
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs while compile sources, ${result}")
    endif()

    # Copies all header files to include directory.
    file(INSTALL ${source_dir}/include/ipmitool/
        DESTINATION ${CMAKE_BINARY_DIR}/include/ipmitool
        FILES_MATCHING PATTERN "*.h"
    )

    if (shared)
        # Copies all built binary files to library directory.
        configure_file(${binary_dir}/libipmicmd.so
            ${CMAKE_BINARY_DIR}/lib/libipmicmd.so.${IPMITOOL_VERSION}
            COPYONLY
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink libipmicmd.so.${IPMITOOL_VERSION} libipmicmd.so.999
            COMMAND ${CMAKE_COMMAND} -E create_symlink libipmicmd.so.999 libipmicmd.so
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lib
        )
    else()
        # Pack compiled object files into static library.
        file(GLOB files ${binary_dir}/*.o)
        execute_process(
            COMMAND ar rcs ${binary_dir}/libipmicmd.a ${files}
            RESULT_VARIABLE result
        )
        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Error occurs while crating static library, ${result}")
        endif()
        configure_file(${binary_dir}/libipmicmd.a
            ${CMAKE_BINARY_DIR}/lib/libipmicmd.a
            COPYONLY
        )
    endif()

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/ipmitool.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/ipmitool.pc
        @ONLY
    )
endfunction()

ipmitool_install()
