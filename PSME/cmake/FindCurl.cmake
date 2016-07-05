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

if (NOT CURL_FOUND)
    find_file(curl_library libcurl.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(curl_library curl)
    find_path(curl_include "curl/curl.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (curl_library AND curl_include)
        add_library(curl IMPORTED SHARED)
        set_target_properties(curl PROPERTIES
            IMPORTED_LOCATION ${curl_library}
        )

        get_filename_component(CURL_LIBRARY_DIRS ${curl_library}
            DIRECTORY
        )

        set(CURL_LIBRARIES curl)
        set(CURL_INCLUDE_DIRS ${curl_include})
        set(CURL_FOUND TRUE)
    endif()
endif()
