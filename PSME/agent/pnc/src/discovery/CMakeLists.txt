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
    discovery_utils.cpp
    discovery_manager.cpp
    discoverer.cpp
    discovery_exceptions.cpp

    builders/builder_factory.cpp

    builders/drive_builder.cpp
    builders/endpoint_builder.cpp
    builders/fabric_builder.cpp
    builders/pcie_device_builder.cpp
    builders/pcie_function_builder.cpp
    builders/port_builder.cpp
    builders/processor_builder.cpp
    builders/storage_subsystem_builder.cpp
    builders/switch_builder.cpp
    builders/zone_builder.cpp
    builders/chassis_builder.cpp
    builders/chassis_builder_mf3.cpp
    builders/metric_builder.cpp
    builders/metric_definition_builder.cpp

    # MF3 specific implementation
    discoverer_mf3.cpp
    builders/builder_factory_mf3.cpp
    builders/switch_builder_mf3.cpp

    # PCIe device discoverers
    pcie_device_discoverers/abstract_pcie_device_discoverer.cpp
    pcie_device_discoverers/drive_discoverer.cpp
    pcie_device_discoverers/processor_discoverer.cpp
    pcie_device_discoverers/unknown_device_discoverer.cpp
    pcie_device_discoverers/ib/ib_processor_data_updater.cpp
    )

add_library(pnc-discovery STATIC ${SOURCES})

target_link_libraries(pnc-discovery PRIVATE
    agent-framework-discovery
    fru_eeprom
    opae11pp
    )
