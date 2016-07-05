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

if (NOT CMAKE_CXX_COMPILER_ID MATCHES Clang)
    return()
endif ()

function (clang_compiler_processing)
    set(C_FLAGS)

    set(CXX_FLAGS
        -std=c++11
        -fexceptions
        -DGTEST_HAS_PTHREAD=1
    )

    set(C_WARNINGS)

    set(CXX_WARNINGS
        -Wall
        -Wextra
        -Wshadow
        -Wno-unused-parameter
        -Wno-unused-const-variable
        -Wno-missing-field-initializers
    )

    set(COMPILER_DEFENSES
        -fPIE
        -fPIC
        -fstack-protector-strong
        -D_FORTIFY_SOURCE=2
    )

    set(LINKER_DEFENSES
        -pie
        -z noexecstack
        -z relro -z now
    )

    string(REGEX REPLACE ";" " " COMPILER_DEFENSES "${COMPILER_DEFENSES}")
    string(REGEX REPLACE ";" " " LINKER_DEFENSES   "${LINKER_DEFENSES}")

    set(C_FLAGS     ${C_FLAGS}   ${C_WARNINGS})
    set(CXX_FLAGS   ${CXX_FLAGS} ${CXX_WARNINGS})

    set(C_FLAGS     ${C_FLAGS}   ${CMAKE_C_FLAGS})
    set(CXX_FLAGS   ${CXX_FLAGS} ${CMAKE_CXX_FLAGS})
    set(LD_FLAGS    ${LD_FLAGS}  ${CMAKE_EXE_LINKER_FLAGS})

    string(REGEX REPLACE ";" " " C_FLAGS    "${C_FLAGS}")
    string(REGEX REPLACE ";" " " CXX_FLAGS  "${CXX_FLAGS}")
    string(REGEX REPLACE ";" " " LD_FLAGS   "${LD_FLAGS}")

    set(COMPILER_DEBUG      "-O0 -g3 -ggdb")
    set(COMPILER_RELEASE    "-O3 -DNDEBUG -fdata-sections -ffunction-sections ${COMPILER_DEFENSES}")
    set(COMPILER_COVERAGE   "-O0 -g --coverage")

    set(LINKER_DEBUG        "")
    set(LINKER_RELEASE      "-Wl,--gc-sections ${LINKER_DEFENSES}")
    set(LINKER_COVERAGE     "--coverage")

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" PARENT_SCOPE)
    endif()

    set(CMAKE_C_FLAGS_DEBUG "${COMPILER_DEBUG} ${C_FLAGS}" PARENT_SCOPE)
    set(CMAKE_C_FLAGS_RELEASE "${COMPILER_RELEASE} ${C_FLAGS}" PARENT_SCOPE)
    set(CMAKE_C_FLAGS_COVERAGE "${COMPILER_COVERAGE} ${C_FLAGS}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${LINKER_DEBUG} ${LD_FLAGS}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${LINKER_RELEASE} ${LD_FLAGS}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${LINKER_COVERAGE} ${LD_FLAGS}" PARENT_SCOPE)

    set(CMAKE_CXX_FLAGS_DEBUG "${COMPILER_DEBUG} ${CXX_FLAGS}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_RELEASE "${COMPILER_RELEASE} ${CXX_FLAGS}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_COVERAGE "${COMPILER_COVERAGE} ${CXX_FLAGS}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${LINKER_DEBUG} ${LD_FLAGS}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${LINKER_RELEASE} ${LD_FLAGS}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_CXXOVERAGE "${LINKER_COVERAGE} ${LD_FLAGS}" PARENT_SCOPE)
endfunction ()

clang_compiler_processing()
