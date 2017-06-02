# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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

function(install_safe_string)
    include(ConfigurationPackage OPTIONAL)
    assure_package(safestring 0.0.1 "https://github.com/01org/safestringlib/archive/master.zip" "45900cbf87c27d4e75bc50f192804df4")

    file(GLOB cfiles ${source_dir}/safeclib/*.c)
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} -fPIC -c -I ${source_dir}/include/ ${cfiles}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs while compiling sources, ${result}")
    endif()

    file(GLOB objfiles ${binary_dir}/*.o)
    execute_process(
        COMMAND ar rcs ${binary_dir}/libsafe-string.a ${objfiles}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs while crating static library, ${result}")
    endif()
    configure_file(${binary_dir}/libsafe-string.a
        ${CMAKE_BINARY_DIR}/lib/libsafe-string.a
        COPYONLY
    )


    file(GLOB hfiles ${source_dir}/include/*.h ${source_dir}/include/*.hpp)
    file(INSTALL
        ${hfiles}
        ${source_dir}/include/safe_types.h
        ${source_dir}/include/safe_lib_errno.h
        DESTINATION ${CMAKE_BINARY_DIR}/include/safe-string
    )

    CONFIGURE_FILE(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/safe-string.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/safe-string.pc
        @ONLY
    )
endfunction()

install_safe_string()
