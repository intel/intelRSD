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
    return()
endif()

add_subdirectory(mocks)
add_subdirectory(configuration)
add_subdirectory(module)
# TODO: Change it to new command ref.
#add_subdirectory(command)
add_subdirectory(eventing)
add_subdirectory(validators)
add_subdirectory(version)

# TODO: Uncomment this after setcap will be add to build process
# to allow use of ping for non-root user.
#add_subdirectory(status)

add_custom_target(unittest_agent-framework
                  make
)
add_custom_target(unittest_agent-framework_run
                  ctest --output-on-failure
)
