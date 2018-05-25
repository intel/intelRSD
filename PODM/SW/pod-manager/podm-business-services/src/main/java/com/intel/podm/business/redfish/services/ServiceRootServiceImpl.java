/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.dto.redfish.ServiceRootContext;
import com.intel.podm.business.dto.redfish.ServiceRootDto;
import com.intel.podm.business.services.redfish.ServiceRootService;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.ejb.Stateless;
import javax.inject.Inject;

import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.CHASSIS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPOSED_NODES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.COMPUTER_SYSTEM_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.EVENT_SERVICE_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.FABRIC_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.MANAGERS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_SERVICES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.TELEMETRY_SERVICE_RESOURCE_NAME;
import static java.util.Collections.emptyList;

@Stateless
public class ServiceRootServiceImpl implements ServiceRootService {
    private static final String REDFISH_SERVICE_VERSION = "1.1.0";
    private static final String RACKSCALE_API_VERSION = "2.3.0";

    @Inject
    @Config
    Holder<ServiceConfig> config;

    @Override
    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    public ServiceRootDto getServiceRoot() {
        ServiceRootDto dto = new ServiceRootDto();
        dto.setId("ServiceRoot");
        dto.setName("Service root");
        dto.setDescription("POD Manager Service root");
        dto.setRedfishVersion(REDFISH_SERVICE_VERSION);
        dto.setUuid(config.get().getUuid());

        ServiceRootContext context = new ServiceRootContext();
        dto.setSystems(singletonContextOf(context, COMPUTER_SYSTEM_RESOURCE_NAME));
        dto.setChassis(singletonContextOf(context, CHASSIS_RESOURCE_NAME));
        dto.setManagers(singletonContextOf(context, MANAGERS_RESOURCE_NAME));
        dto.setEventService(singletonContextOf(context, EVENT_SERVICE_RESOURCE_NAME));
        dto.setFabrics(singletonContextOf(context, FABRIC_RESOURCE_NAME));
        dto.setStorageServices(singletonContextOf(context, STORAGE_SERVICES_RESOURCE_NAME));
        dto.setTelemetryService(singletonContextOf(context, TELEMETRY_SERVICE_RESOURCE_NAME));

        dto.setUnknownOems(emptyList());
        ServiceRootDto.Oem.RackScaleOem rackScaleOem = dto.getOem().getRackScaleOem();
        rackScaleOem.setComposedNodes(singletonContextOf(context, COMPOSED_NODES_RESOURCE_NAME));
        rackScaleOem.setEthernetSwitches(singletonContextOf(context, ETHERNET_SWITCHES_RESOURCE_NAME));
        rackScaleOem.setApiVersion(RACKSCALE_API_VERSION);

        return dto;
    }

}

