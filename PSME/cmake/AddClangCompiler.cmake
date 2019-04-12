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

if (NOT CMAKE_CXX_COMPILER_ID MATCHES Clang)
    return()
endif ()

function (clang_compiler_processing)
    set(C_FLAGS   -std=c11)
    set(CXX_FLAGS -std=c++14)

    set(C_WARNINGS
        -Werror
        -Weverything
        -Wno-padded
        -Wno-covered-switch-default
    )

    set(CXX_WARNINGS
        -Werror
        -Weverything
        -Wno-newline-eof
        -Wno-padded
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        # for agent framework - to be removed
        -Wno-reorder
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-sign-conversion
        -Wno-float-equal

        -Wdocumentation
        -fcomment-block-commands=anchor,i,header,startuml,enduml
        -Wdocumentation-deprecated-sync
        -Wno-deprecated-declarations
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

    set(C_FLAGS "${C_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
    set(CXX_FLAGS "${CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

    set(CMAKE_C_FLAGS_DEBUG "${COMPILER_DEBUG} ${C_FLAGS}" CACHE STRING "CMake C Flags Debug" FORCE)
    set(CMAKE_C_FLAGS_RELEASE "${COMPILER_RELEASE} ${C_FLAGS}" CACHE STRING "CMake C Flags Release" FORCE)
    set(CMAKE_C_FLAGS_COVERAGE "${COMPILER_COVERAGE} ${C_FLAGS}" CACHE STRING "CMake C Flags Coverage" FORCE)

    set(CMAKE_CXX_FLAGS_DEBUG "${COMPILER_DEBUG} ${CXX_FLAGS}" CACHE STRING "CMake CXX Flags Debug" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "${COMPILER_RELEASE} ${CXX_FLAGS}" CACHE STRING "CMake CXX Flags Release" FORCE)
    set(CMAKE_CXX_FLAGS_COVERAGE "${COMPILER_COVERAGE} ${CXX_FLAGS}" CACHE STRING "CMake CXX Flags Coverage" FORCE)

    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${LINKER_DEBUG} ${LD_FLAGS}" CACHE STRING "CMake Exe Linker Flags Debug")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${LINKER_RELEASE} ${LD_FLAGS} -s" CACHE STRING "CMake Exe Linker Flags Release" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE "${LINKER_COVERAGE} ${LD_FLAGS}" CACHE STRING "CMake Exe Linker Flags Coverage" FORCE)
endfunction ()

clang_compiler_processing()
