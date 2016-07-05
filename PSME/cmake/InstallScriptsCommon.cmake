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

# Creates a list called ARGS and addds standard + compiler options to it
macro(init_cmake_args ARGS_LIST)
	if ("${ARGS_LIST}" STREQUAL "")
		message(FATAL_ERROR "init_cmake_args requires precisely one argument for CMake arguments list variable! argv1 is ${ARGS_LIST}")
	endif("${ARGS_LIST}" STREQUAL "")

	set(${ARGS_LIST})

	list(APPEND ${ARGS_LIST} -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ${ARGS_LIST} -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

	if (CMAKE_CROSSCOMPILING)
        if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
            list(APPEND ${ARGS_LIST} -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
        endif()

        list(APPEND ${ARGS_LIST} -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
    endif()

    if (CMAKE_BUILD_TYPE)
        list(APPEND ${ARGS_LIST} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
    endif()

	list(APPEND ${ARGS_LIST} -DCMAKE_C_FLAGS_DEBUG:PATH=${CMAKE_C_FLAGS_DEBUG})
	list(APPEND ${ARGS_LIST} -DCMAKE_C_FLAGS_RELEASE:PATH=${CMAKE_C_FLAGS_RELEASE})
	list(APPEND ${ARGS_LIST} -DCMAKE_C_FLAGS_COVERAGE:PATH=${CMAKE_C_FLAGS_COVERAGE})

	list(APPEND ${ARGS_LIST} -DCMAKE_CXX_FLAGS_DEBUG:PATH=${CMAKE_CXX_FLAGS_DEBUG})
	list(APPEND ${ARGS_LIST} -DCMAKE_CXX_FLAGS_RELEASE:PATH=${CMAKE_CXX_FLAGS_RELEASE})
	list(APPEND ${ARGS_LIST} -DCMAKE_CXX_FLAGS_COVERAGE:PATH=${CMAKE_CXX_FLAGS_COVERAGE})

	list(APPEND ${ARGS_LIST} -DCMAKE_EXE_LINKER_FLAGS_DEBUG:PATH=${CMAKE_EXE_LINKER_FLAGS_DEBUG})
	list(APPEND ${ARGS_LIST} -DCMAKE_EXE_LINKER_FLAGS_RELEASE:PATH=${CMAKE_EXE_LINKER_FLAGS_RELEASE})
	list(APPEND ${ARGS_LIST} -DCMAKE_EXE_LINKER_FLAGS_COVERAGE:PATH=${CMAKE_EXE_LINKER_FLAGS_COVERAGE})

	list(APPEND ${ARGS_LIST} --no-warn-unused-cli)
endmacro(init_cmake_args ARGS_LIST)
