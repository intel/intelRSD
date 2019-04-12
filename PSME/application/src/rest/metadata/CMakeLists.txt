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

set(METADATA_INCLUDE_DIR
    ${PSME_APPLICATION_DIR}/include/psme/rest/metadata
)

set(METADATA_GENERATED_INCLUDE_DIR
    ${METADATA_INCLUDE_DIR}/include-generated
)

# Get a list of all XML files
# FIXME $metadata.xml is skipped because '$' eats the first char of filename
file(GLOB METADATA_XML ${METADATA_INCLUDE_DIR}/xml/[^\$]*.xml)

# Add a new generated file (one is enough) which depends on XML files and
# is a product of generate_headers.sh script
add_custom_command(
    OUTPUT ${METADATA_GENERATED_INCLUDE_DIR}/generated_headers.hpp
    COMMAND ${METADATA_INCLUDE_DIR}/generate_headers.sh
    DEPENDS ${METADATA_INCLUDE_DIR}/generate_headers.sh ${METADATA_XML}
)

# Add a custom target which represents the above generated header
add_custom_target(metadata_generate_headers
    DEPENDS ${METADATA_GENERATED_INCLUDE_DIR}/generated_headers.hpp)

set(sources
    metadata_manager.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang|GNU)
    set_source_files_properties(
        metadata_manager.cpp
        PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)
endif()

add_library(metadata ${sources})

# Metadata library should wait until the headers are prepared
add_dependencies(metadata metadata_generate_headers)

set_target_properties(metadata
                    PROPERTIES
                    OUTPUT_NAME metadata
                    LINKER_LANGUAGE CXX
)
