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

function(install_gmp)
    include(ConfigurationPackage OPTIONAL)
    assure_package(gmp 6.1.2 "https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz" "f58fa8001d60c4c77595fbbb62b63c1d")

    set(CONFIGURE_FLAGS)

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-shared)
    list(APPEND CONFIGURE_FLAGS --disable-static)

    set(ENV{CFLAGS} "-m32")
    set(ENV{CXXFLAGS} "-m32")
    set(ENV{ABI} "32")

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

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/gmp.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/gmp.pc
        @ONLY
    )

    file(INSTALL ${binary_dir}/.libs/
        DESTINATION ${CMAKE_BINARY_DIR}/lib
        FILES_MATCHING PATTERN "libgmp*.so*"
    )
    file(INSTALL ${binary_dir}/gmp.h ${binary_dir}/gmp-mparam.h
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )
endfunction()

install_gmp()
