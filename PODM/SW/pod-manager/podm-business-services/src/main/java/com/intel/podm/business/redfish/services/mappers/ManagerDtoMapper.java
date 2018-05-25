/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.ManagerDto;
import com.intel.podm.business.entities.redfish.Manager;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asComputerSystemContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchContexts;
import static com.intel.podm.business.redfish.ContextCollections.asStorageServicesContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;

@Dependent
class ManagerDtoMapper extends DtoMapper<Manager, ManagerDto> {
    ManagerDtoMapper() {
        super(Manager.class, ManagerDto.class);
        this.ignoredProperties("ethernetInterfaces", "networkProtocol", "links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(Manager source, ManagerDto target) {
        super.performNotAutomatedMapping(source, target);

        ManagerDto.Links links = target.getLinks();
        links.getManagedComputerSystems().addAll(asComputerSystemContexts(source.getComputerSystems()));
        links.getManagedChassisCollection().addAll(asChassisContexts(source.getManagedChassis()));
        links.setManagerInChassis(toContext(source.getInChassisManager()));
        links.getOem().getRackScaleOem().getManagedEthernetSwitches().addAll(asEthernetSwitchContexts(source.getEthernetSwitches()));
        links.getOem().getRackScaleOem().getManagerForServices().addAll(asStorageServicesContexts(source.getStorageServices()));
    }
}
