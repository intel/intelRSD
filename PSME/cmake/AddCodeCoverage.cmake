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

if (CODE_COVERAGE_USED)
    return()
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES coverage)
    return()
endif()

set(CODE_COVERAGE_USED TRUE)

function(code_coverage_processing)
    set(LCOV_COMMAND lcov)
    set(GENHTML_COMMAND genhtml)
    set(CODE_COVERAGE_DIR code_coverage)

    add_custom_target(code-coverage
        COMMAND ${LCOV_COMMAND} --quiet --zerocounters --directory .
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CODE_COVERAGE_DIR}
        COMMAND ${LCOV_COMMAND}
            --quiet
            --checksum
            --capture --directory .
            --initial
            --output-file ${CODE_COVERAGE_DIR}/tracefile.base
        COMMAND ctest
        COMMAND ${LCOV_COMMAND}
            --quiet
            --checksum
            --capture --directory .
            --output-file ${CODE_COVERAGE_DIR}/tracefile.test
        COMMAND ${LCOV_COMMAND}
            --quiet
            --checksum
            --add-tracefile ${CODE_COVERAGE_DIR}/tracefile.base
            --add-tracefile ${CODE_COVERAGE_DIR}/tracefile.test
            --output-file ${CODE_COVERAGE_DIR}/tracefile
        COMMAND ${LCOV_COMMAND}
            --quiet
            --checksum
            --remove ${CODE_COVERAGE_DIR}/tracefile '/usr/*' 'tests/*'
            '**gtest/*' '**gmock/*' 'examples/*' ${CODE_COVERAGE_IGNORE}
            --output-file ${CODE_COVERAGE_DIR}/tracefile.cleaned
        COMMAND ${GENHTML_COMMAND}
            --quiet
            --legend
            --num-spaces 4
            --demangle-cpp
            --title ${CMAKE_PROJECT_NAME}
            --output-directory ${CODE_COVERAGE_DIR}/html
            ${CODE_COVERAGE_DIR}/tracefile.cleaned
    )

    add_custom_target(code-coverage-clean
        COMMAND ${LCOV_COMMAND} --zerocounters --directory .
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CODE_COVERAGE_DIR}
    )
endfunction()

code_coverage_processing()
