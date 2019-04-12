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

if (GENERATE_DOCUMENTATION_USED)
    return()
endif()

set(GENERATE_DOCUMENTATION_USED TRUE)

function (generate_documentation_processing)
    set(DOXYGEN_COMMAND doxygen)

    add_custom_target(doc-generate
        COMMAND ${DOXYGEN_COMMAND} doxygen.config
        COMMAND ${CMAKE_COMMAND} -E copy_directory doc
            ${CMAKE_CURRENT_BINARY_DIR}/doc
        COMMAND ${CMAKE_COMMAND} -E remove_directory doc
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_custom_target(doc-clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory doc
    )

    add_custom_target(doc-dev-generate
        COMMAND ${DOXYGEN_COMMAND} doxygen-dev.config
        COMMAND ${CMAKE_COMMAND} -E copy_directory doc-dev
            ${CMAKE_CURRENT_BINARY_DIR}/doc-dev
        COMMAND ${CMAKE_COMMAND} -E remove_directory doc-dev
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    add_custom_target(doc-dev-clean
        COMMAND ${CMAKE_COMMAND} -E remove_directory doc-dev
    )
endfunction()

generate_documentation_processing()
