/*
 * Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.rsd.resourcemanager.layers.merger.resources;

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.common.resources.ResourceProvider;
import lombok.SneakyThrows;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import java.time.LocalDate;
import java.util.UUID;

@Configuration
@SuppressWarnings("checkstyle:MethodCount")
public class ResourceProviderConfiguration {

    @Bean
    public UUID serviceRootUuid(@Value("${serviceRoot.UUID:34e60059-0d9a-44ee-9e57-09f9bcccf40f}") UUID uuid) {
        return uuid;
    }

    @Bean
    @SneakyThrows
    public ResourceProvider serviceRoot(UUID serviceRootUuid) {

        return new Resource("ServiceRoot.json") {
            @Override
            public ObjectNode getResource() {
                val resource = super.getResource();
                resource.put("UUID", serviceRootUuid.toString());
                return resource;
            }
        };
    }

    @Bean
    @SneakyThrows
    public ResourceProvider chassisCollectionProvider() {
        return new CollectionResource("/redfish/v1/Chassis", "ChassisCollection") {
            @Override
            public ObjectNode getResource() {
                val resource = super.getResource();
                resource.withArray("Members")
                    .addObject()
                    .put("@odata.id", "/redfish/v1/Chassis/pod");
                return resource;
            }
        };
    }

    @Bean
    @SneakyThrows
    public ResourceProvider podmChassis() {
        return new Resource("pod-chassis.json") {
            @Override
            public boolean isGlobal() {
                return false;
            }
        };
    }

    @Bean
    @SneakyThrows
    public ResourceProvider systemsCollectionProvider() {
        return new CollectionResource("/redfish/v1/Systems", "ComputerSystemCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider fabricCollectionProvider() {
        return new CollectionResource("/redfish/v1/Fabrics", "FabricCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider managersCollectionProvider() {
        return new ModifiableCollectionResource("/redfish/v1/Managers", "ManagerCollection") {
            @Override
            public ObjectNode getResource() {
                val resource = super.getResource();
                resource.withArray("Members")
                    .addObject()
                    .put("@odata.id", "/redfish/v1/Managers/PodManager");
                return resource;
            }
        };
    }

    @Bean
    @SneakyThrows
    public ResourceProvider podManager(UUID serviceEntryPointUuid) {
        return new Resource("PodManager.json") {
            @Override
            public boolean isGlobal() {
                return false;
            }
            @Override
            public ObjectNode getResource() {
                val resource = super.getResource();
                resource.put("ServiceEntryPointUUID", serviceEntryPointUuid.toString());
                return resource;
            }
        };
    }

    @Bean
    @SneakyThrows
    public ResourceProvider storageServicesCollectionProvider() {
        return new CollectionResource("/redfish/v1/StorageServices", "StorageServiceCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider taskCollectionProvider() {
        return new CollectionResource("/redfish/v1/TaskService/Tasks", "TaskCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider ethernetSwitchesCollectionProvider() {
        return new CollectionResource("/redfish/v1/EthernetSwitches", "EthernetSwitchCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider metricReportDefinitionsCollection() {
        return new CollectionResource("/redfish/v1/TelemetryService/MetricReportDefinitions", "MetricReportDefinitionCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider metricDefinitionsCollection() {
        return new CollectionResource("/redfish/v1/TelemetryService/MetricDefinitions", "MetricDefinitionCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider oemMetricReportDefinitionsCollection() {
        return new CollectionResource("/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricReportDefinitions", "MetricReportDefinitionCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider oemMetricDefinitionsCollection() {
        return new CollectionResource("/redfish/v1/Oem/Intel_RackScale/TelemetryService/MetricDefinitions", "MetricDefinitionCollection");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider telemetryService() {
        return new Resource("TelemetryService.json");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider oemTelemetryService() {
        return new Resource("OemTelemetryService.json");
    }

    @Bean
    @SneakyThrows
    public ResourceProvider taskService() {
        return new Resource("TaskService.json") {
            @Override
            public ObjectNode getResource() {
                val resource = super.getResource();
                resource.put("DateTime", LocalDate.now().toString());
                return resource;
            }
        };
    }
}
