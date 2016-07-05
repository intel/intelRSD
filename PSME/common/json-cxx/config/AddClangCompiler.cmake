# Copyright (c) 2015, Tymoteusz Blazejczyk
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# * Neither the name of json-cxx nor the names of its
#   contributors may be used to endorse or promote products derived from
#   this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

if (NOT CMAKE_CXX_COMPILER_ID MATCHES Clang)
    return()
endif ()

function (clang_compiler_processing)
    set(C_FLAGS   -std=c11)
    set(CXX_FLAGS -std=c++11)

    set(C_WARNINGS
        -Werror
        -Weverything
        -Wno-padded
        -Wno-covered-switch-default
    )

    set(CXX_WARNINGS
        -Werror
        -Weverything
        -Wno-padded
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-covered-switch-default
        -Wno-global-constructors
    )

    set(C_FLAGS     ${C_FLAGS}   ${C_WARNINGS})
    set(CXX_FLAGS   ${CXX_FLAGS} ${CXX_WARNINGS})

    set(C_FLAGS     ${C_FLAGS}   ${CMAKE_C_FLAGS})
    set(CXX_FLAGS   ${CXX_FLAGS} ${CMAKE_CXX_FLAGS})
    set(LD_FLAGS    ${LD_FLAGS}  ${CMAKE_EXE_LINKER_FLAGS})

    string(REGEX REPLACE ";" " " C_FLAGS    "${C_FLAGS}")
    string(REGEX REPLACE ";" " " CXX_FLAGS  "${CXX_FLAGS}")
    string(REGEX REPLACE ";" " " LD_FLAGS   "${LD_FLAGS}")

    set(COMPILER_DEBUG      "-O0 -g3 -ggdb")
    set(COMPILER_RELEASE    "-O3 -DNDEBUG -fdata-sections -ffunction-sections")
    set(COMPILER_COVERAGE   "-O0 -g --coverage")

    set(LINKER_DEBUG        "")
    set(LINKER_RELEASE      "-Wl,--gc-sections")
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
