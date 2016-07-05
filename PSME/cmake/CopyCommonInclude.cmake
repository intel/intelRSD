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

function(copy_common_include)

    set(COMMON_INCLUDE_SOURCE_DIR
        ${CMAKE_CURRENT_LIST_DIR}/../common/include
        CACHE PATH "Common include source path"
    )

    # First find to decide if install it
    find_package(CommonInclude)

    set(source_dir ${COMMON_INCLUDE_SOURCE_DIR})
    set(binary_dir ${CMAKE_BINARY_DIR}/common)

    if (EXISTS ${source_dir})
        file(MAKE_DIRECTORY ${binary_dir})
        file(COPY ${source_dir} DESTINATION ${binary_dir})
    endif()
endfunction()

copy_common_include()
find_package(CommonInclude)
