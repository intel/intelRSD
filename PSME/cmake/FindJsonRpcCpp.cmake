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

if (NOT JSONRPCCPP_FOUND)
    find_file(jsonrpccpp-common_library libjsonrpccpp-common.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(jsonrpccpp-common_library jsonrpccpp-common)
    find_file(jsonrpccpp-server_library libjsonrpccpp-server.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(jsonrpccpp-server_library jsonrpccpp-server)
    find_file(jsonrpccpp-client_library libjsonrpccpp-client.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(jsonrpccpp-client_library jsonrpccpp-client)
    find_path(jsonrpccpp_include "jsonrpccpp/version.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (jsonrpccpp-common_library AND jsonrpccpp-server_library
    AND jsonrpccpp-client_library AND jsonrpccpp_include)
        add_library(jsonrpccpp-common IMPORTED STATIC)
        set_target_properties(jsonrpccpp-common PROPERTIES
            IMPORTED_LOCATION ${jsonrpccpp-common_library}
        )

        add_library(jsonrpccpp-server IMPORTED STATIC)
        set_target_properties(jsonrpccpp-server PROPERTIES
            IMPORTED_LOCATION ${jsonrpccpp-server_library}
        )

        add_library(jsonrpccpp-client IMPORTED STATIC)
        set_target_properties(jsonrpccpp-client PROPERTIES
            IMPORTED_LOCATION ${jsonrpccpp-client_library}
        )

        set(JSONRPCCPP_LIBRARIES
            jsonrpccpp-common
            jsonrpccpp-server
            jsonrpccpp-client
        )
        set(JSONRPCCPP_INCLUDE_DIRS ${jsonrpccpp_include})
        set(JSONRPCCPP_FOUND TRUE)
    endif()
endif()
