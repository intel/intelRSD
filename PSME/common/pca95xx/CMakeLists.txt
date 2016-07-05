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

cmake_minimum_required(VERSION 2.8)
project("PCA95xx" C CXX)

set(PCA95XX_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH "PCA95XX include dir path"
)

set(PCA95XX_LIBRARIES
    pca95xx
    CACHE PATH "PCA95XX libraries"
)

add_library(pca95xx STATIC
    src/pca95xx.c
    src/pca95xx.cpp
    src/pca95xx_i2c.c
    src/pca95xx_common.c
    src/pca95xx_errors.c
    src/pca95xx_registers.c
    src/models/pca9505.c
    src/models/pca9555.c
)

target_include_directories(pca95xx PUBLIC include)

set_target_properties(pca95xx PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    # GCC bug. In C++11 inititalization with {} is correct
    set_source_files_properties(src/pca95xx.cpp PROPERTIES
        COMPILE_FLAGS "-Wno-missing-field-initializers"
    )
endif()

install (TARGETS pca95xx
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install (DIRECTORY include/pca95xx DESTINATION include)
