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

if (NOT VALGRINDCXX_PACKAGE)
    #set(VALGRINDCXX_PACKAGE)
endif()

if (NOT THREADS_CHECK)
    return()
endif()

if (NOT VALGRINDCXX_SOURCE_DIR)
    set(VALGRINDCXX_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/../common/valgrind-cxx)
endif()

if (NOT EXISTS ${VALGRINDCXX_SOURCE_DIR}/src/thread.cpp)
   message(WARNING "THREADS_CHECK flag disabled.")
   return()
endif()

if (NOT VALGRINDCXX_FOUND)
    find_library(valgrind-cxx_library valgrind-cxx)
    find_path(valgrind-cxx_include "valgrind-cxx/drd.hpp")

    if (valgrind-cxx_library AND valgrind-cxx_include)
        add_library(valgrind-cxx IMPORTED STATIC)
        set_target_properties(valgrind-cxx PROPERTIES
            IMPORTED_LOCATION ${valgrind-cxx_library}
        )

        get_filename_component(VALGRINDCXX_LIBRARY_DIRS ${valgrind-cxx_library}
            DIRECTORY
        )

        set(VALGRINDCXX_LIBRARIES valgrind-cxx)
        set(VALGRINDCXX_INCLUDE_DIRS ${valgrind-cxx_include})
        set(VALGRINDCXX_FOUND TRUE)
    endif()
endif()

if (NOT VALGRINDCXX_FOUND)

    include(Utils)
    include(ExternalProject)

    if (EXISTS ${VALGRINDCXX_SOURCE_DIR})
        set(PROJECT_SOURCE SOURCE_DIR ${VALGRINDCXX_SOURCE_DIR})
    elseif (VALGRINDCXX_PACKAGE)
        set(PROJECT_SOURCE URL ${VALGRINDCXX_PACKAGE})
    endif()

    get_cmake_args(ARGS)

    ExternalProject_Add(ValgrindCxxProject
        ${PROJECT_SOURCE}
        BINARY_DIR valgrind-cxx
        PREFIX valgrind-cxx
        # Disable install step
        INSTALL_COMMAND ""
        CMAKE_ARGS ${ARGS}
    )

    add_library(valgrind-cxx IMPORTED STATIC)
    add_dependencies(valgrind-cxx ValgrindCxxProject)

    ExternalProject_Get_Property(ValgrindCxxProject source_dir binary_dir)

    set_target_properties(valgrind-cxx PROPERTIES
        IMPORTED_LOCATION ${binary_dir}/lib/libvalgrind-cxx.a
    )

    set(VALGRINDCXX_LIBRARIES valgrind-cxx)
    set(VALGRINDCXX_LIBRARY_DIRS ${binary_dir}/lib)
    set(VALGRINDCXX_INCLUDE_DIRS ${source_dir}/include)
    set(VALGRINDCXX_FOUND TRUE)
endif()

if (VALGRINDCXX_FOUND)
    set(ENABLE_VALGRIND_DRD TRUE)
    set(VALGRINDCXX_COMPILE_FLAGS
        -DENABLE_VALGRIND_DRD
        -include valgrind-cxx/drd.hpp
        )
endif()
