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

add_subdirectory(command)

set(SOURCES
    utils.cpp
    ipmi_message.cpp
    socket.cpp
    mux.cpp
    responder.cpp
    client.cpp
    service.cpp
    worker_thread.cpp
    receiver_thread.cpp
    dispatcher.cpp
    patcher.cpp
    gpio.cpp

    network_utils.cpp

    certificate.cpp
    certificate_manager.cpp

    watcher/watcher.cpp
    watcher/task.cpp
    watcher/drawer_power_task.cpp
    watcher/thermal_sensor_task.cpp
)

set_source_files_properties(
    service.cpp
    COMPILE_FLAGS "-Wno-switch-enum"
)

set_property (
    SOURCE network_utils.cpp
    APPEND
    PROPERTY COMPILE_FLAGS
    "-Wno-effc++"
)

add_library(ipmb-service OBJECT ${SOURCES})
