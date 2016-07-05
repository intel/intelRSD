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

add_executable(jsonrpc_example_client example_client.cpp)

target_link_libraries(jsonrpc_example_client
    curl
    jsoncpp
    jsonrpccpp-common
    jsonrpccpp-client
)

add_executable(jsonrpc_example_server example_server.cpp)

target_link_libraries(jsonrpc_example_server
    microhttpd
    jsonrpccpp-server
    jsonrpccpp-common
    jsoncpp
)
