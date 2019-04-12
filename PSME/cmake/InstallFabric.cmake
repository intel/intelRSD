# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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

function(install_libfabric)
    include(ConfigurationPackage OPTIONAL)
    assure_package(libfabric 1.5.3 "https://github.com/ofiwg/libfabric/releases/download/v1.5.3/libfabric-1.5.3.tar.gz" "1fe07e972fe487c6a3e44c0fb68b49a2")

    set(CONFIGURE_FLAGS)

    list(APPEND CONFIGURE_FLAGS CC=${CMAKE_C_COMPILER})
    list(APPEND CONFIGURE_FLAGS CXX=${CMAKE_CXX_COMPILER})
    list(APPEND CONFIGURE_FLAGS --prefix=${CMAKE_BINARY_DIR})
    list(APPEND CONFIGURE_FLAGS --enable-verbs=yes)
    list(APPEND CONFIGURE_FLAGS --enable-static=yes)
    list(APPEND CONFIGURE_FLAGS --enable-shared=no)


    set(ENV{CFLAGS} "-I${CMAKE_BINARY_DIR}/include -fPIC")
    set(ENV{LDFLAGS} "-L${CMAKE_BINARY_DIR}/lib")
    set(ENV{LD_LIBRARY_PATH} "${CMAKE_BINARY_DIR}/lib")
    set(ENV{PATH} "${CMAKE_BINARY_DIR}/bin:/bin:/usr/bin:${PATH}")

    execute_process(
        COMMAND ${source_dir}/configure ${CONFIGURE_FLAGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project ${source_dir}")
    endif()

    execute_process(
        COMMAND make ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )
    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    file(INSTALL ${binary_dir}/src/.libs/libfabric.a
	 DESTINATION ${CMAKE_BINARY_DIR}/lib
    )
    set(FABRIC_LIBRARY ${CMAKE_BINARY_DIR}/lib/libfabric.a)
    file(INSTALL ${source_dir}/include/rdma
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )

    file(COPY ${binary_dir}/libfabric.pc
        DESTINATION ${CMAKE_PKG_CONFIG_DIR}
    )

endfunction()

install_libfabric()
