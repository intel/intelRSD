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

if (NOT UUID_FOUND)
    find_file(uuid_library libuuid++.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(uuid_library uuid++ NAMES ossp-uuid++)
    find_path(uuid_include uuid++.hh
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (uuid_library AND uuid_include)
        add_library(uuid++ IMPORTED SHARED)
        set_target_properties(uuid++ PROPERTIES
            IMPORTED_LOCATION ${uuid_library}
        )

        get_filename_component(UUID_LIBRARY_DIRS ${uuid_library}
            DIRECTORY
        )

        set(UUID_LIBRARIES uuid++)
        set(UUID_INCLUDE_DIRS ${uuid_include})
        set(UUID_FOUND TRUE)
    endif()
endif()
