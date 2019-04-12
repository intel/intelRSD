# <license_header>
#
# Copyright (c) 2018-2019 Intel Corporation
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

function(generate_config_file FILE_REQUIRING GENERATED_FILE CONST_NAME BRIEF_DESCRIPTION)
    file(READ ${CMAKE_CURRENT_SOURCE_DIR}/../../configuration.json CONFIG_FILE_CONTENT)
    get_filename_component(FILE_NAME ${GENERATED_FILE} NAME)
    configure_file(${CONFIGURATION_INCLUDE_DIRS}/configuration/configuration_full.hpp.in ${GENERATED_FILE})
    set_property(SOURCE ${FILE_REQUIRING} APPEND PROPERTY OBJECT_DEPENDS ${GENERATED_FILE})
endfunction()

function(generate_default_config_file FILE_REQUIRING NAMESPACE1 NAMESPACE2 DEFAULT_ENV_FILE OUTPUT_FILE CONFIG_PATH)
    set(GENERATED_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${OUTPUT_FILE})
    file(READ ${CONFIG_PATH}/configuration.json CONFIG_FILE_CONTENT)

    if(EXISTS ${CONFIG_PATH}/configuration_schema.json)
        file(READ "${CONFIG_PATH}/configuration_schema.json" SCHEMA_FILE_CONTENT)
    else()
        set(SCHEMA_FILE_CONTENT "{ }")
    endif()

    get_filename_component(FILE_NAME ${GENERATED_FILE} NAME)
    configure_file(${CONFIGURATION_INCLUDE_DIRS}/configuration/default_configuration.hpp.in ${GENERATED_FILE})
    set_property(SOURCE ${FILE_REQUIRING} APPEND PROPERTY OBJECT_DEPENDS ${GENERATED_FILE})
endfunction()