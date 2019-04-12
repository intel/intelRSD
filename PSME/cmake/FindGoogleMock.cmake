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

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompiling.")
    return()
endif()

if (NOT GTEST_FOUND)
    message(WARNING "No GoogleTests found")
    return()
endif()

find_package_local(GoogleMock gmock)
if (GMOCK_FOUND)
    set(GTEST_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} CACHE INTERNAL "included GMOCK")
    set(GTEST_LIBRARY_DIRS ${GTEST_LIBRARY_DIRS} ${GMOCK_LIBRARY_DIRS} CACHE INTERNAL "included GMOCK")
    set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS} CACHE INTERNAL "included GMOCK")
endif()
