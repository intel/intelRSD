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

# Look for the header file.
find_path(GNUTLS_INCLUDE_DIR
    NAMES gnutls/gnutls.h
    PATHS ${CMAKE_BINARY_DIR}/include
)
mark_as_advanced(GNUTLS_INCLUDE_DIR)

# Look for the library
find_library(GNUTLS_LIBRARY
    NAMES gnutls libgnutls
    PATHS ${CMAKE_BINARY_DIR}/lib
)
mark_as_advanced(GNUTLS_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set GNUTLS_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GNUTLS
    REQUIRED_VARS GNUTLS_LIBRARY GNUTLS_INCLUDE_DIR
)

if(GNUTLS_FOUND)
  set(GNUTLS_LIBRARIES ${GNUTLS_LIBRARY})
  set(GNUTLS_INCLUDE_DIRS ${GNUTLS_INCLUDE_DIR})
endif()
