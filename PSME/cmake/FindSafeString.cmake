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

if (NOT SAFESTRING_FOUND)
    find_file(safe-string_library libsafe-string.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(safe-string_library safe-string)

    find_path(safe-string_include "safe-string/safe_lib.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (safe-string_library AND safe-string_include)
        add_library(safe-string IMPORTED STATIC)
        set_target_properties(safe-string PROPERTIES
            IMPORTED_LOCATION ${safe-string_library}
        )

        get_filename_component(SAFESTRING_LIBRARY_DIR ${safe-string_library} DIRECTORY)

        set(SAFESTRING_INCLUDE_DIRS ${safe-string_include})
        set(SAFESTRING_LIBRARY_DIRS ${SAFESTRING_LIBRARY_DIR})
        set(SAFESTRING_LIBRARIES safe-string)
        set(SAFESTRING_FOUND TRUE)
    endif()
endif()
