# <license_header>
#
# Copyright (c) 2016-2019 Intel Corporation
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

set(SOURCES
    access_interface.cpp
    access_interface_factory.cpp
    pcie_access_interface.cpp
    global_address_space_registers.cpp

    mrpc/command.cpp
    mrpc/twi_access_read.cpp
    mrpc/twi_access_write.cpp
    mrpc/bind_port.cpp
    mrpc/unbind_port.cpp
    mrpc/port_binding_info.cpp
    mrpc/partition_binding_info.cpp
    mrpc/link_status_retrieve.cpp

    top/top_level_registers.cpp

    partition/partition_configuration.cpp

    csr/configuration_space_register.cpp

    utils.cpp
)

set_source_files_properties(
        pcie_access_interface.cpp
        COMPILE_FLAGS "-Wno-old-style-cast")

add_library(pnc-gas OBJECT ${SOURCES})
