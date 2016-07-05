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

if (NOT MD5_FOUND)

    find_file(md5_library libmd5.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )

    find_library(md5_library md5)
    find_path(md5_include "md5/md5.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (md5_include AND md5_library)

        add_library(md5 IMPORTED STATIC)
        set_target_properties(md5 PROPERTIES
            IMPORTED_LOCATION ${md5_library}
        )

        get_filename_component(MD5_LIBRARY_DIR ${md5_library} DIRECTORY)

        set(MD5_INCLUDE_DIRS ${md5_include})
        set(MD5_LIBRARY_DIRS ${MD5_LIBRARY_DIR})
        set(MD5_LIBRARIES md5)
        set(MD5_FOUND TRUE)

    endif()
endif()

