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

if (NOT JSONCPP_FOUND)
    find_file(jsoncpp_library libjsoncpp.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(jsoncpp_library jsoncpp)
    find_path(jsoncpp_include "json/json.h"
        PATH_SUFFIXES jsoncpp
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (jsoncpp_library AND jsoncpp_include)
        add_library(jsoncpp IMPORTED SHARED)
        set_target_properties(jsoncpp PROPERTIES
            IMPORTED_LOCATION ${jsoncpp_library}
        )

        get_filename_component(JSONCPP_LIBRARY_DIRS ${jsoncpp_library}
            DIRECTORY
        )

        set(JSONCPP_LIBRARIES jsoncpp)
        set(JSONCPP_INCLUDE_DIRS ${jsoncpp_include})
        set(JSONCPP_FOUND TRUE)
    endif()
endif()
