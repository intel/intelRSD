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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.ChassisDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.embeddables.RackChassisAttributes;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;

import static com.intel.podm.business.dto.ChassisDto.Oem.RackScaleOem.ODATA_TYPE_RACK;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asComputerSystemContexts;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPowerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asStorageContexts;
import static com.intel.podm.business.redfish.ContextCollections.asThermalContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.ChassisType.RACK;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toSet;

@Dependent
class ChassisDtoMapper extends DtoMapper<Chassis, ChassisDto> {
    @Inject
    @Config
    private Holder<DiscoveryConfig> discoveryConfig;

    ChassisDtoMapper() {
        super(Chassis.class, ChassisDto.class);
        this.ignoredProperties("power", "thermal", "links", "status");
    }

    @Override
    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    protected void performNotAutomatedMapping(Chassis source, ChassisDto target) {
        super.performNotAutomatedMapping(source, target);

        if (!source.isComplementary()) {
            ChassisDto.Links links = target.getLinks();
            links.setContains(asChassisContexts(source.getContainedChassis()));
            Chassis containedByChassis = source.getContainedByChassis();
            if (containedByChassis != null) {
                links.setContainedBy(toContext(containedByChassis));
            }
            links.setComputerSystems(asComputerSystemContexts(source.getComputerSystems()));
            links.setManagedBy(asManagerContexts(source.getManagers()));
            links.setManagersInChassis(asManagerContexts(source.getInChassisManagers()));
            links.setStorage(asStorageContexts(source.getComputerSystems().stream()
                .map(ComputerSystem::getStorages)
                .flatMap(Collection::stream)
                .collect(toSet())));
            links.setDrives(asDriveContexts(source.getDrives()));
            links.setPoweredBy(asPowerContexts(source.getPoweredBy()));
            links.setCooledBy(asThermalContexts(source.getCooledBy()));
            links.getOem().getRackScaleOem().setSwitches(asEthernetSwitchContexts(source.getEthernetSwitches()));
            mapStatus(source, target);
        }

        if (RACK.equals(source.getChassisType())) {
            ChassisDto.RackChassisRackScaleOem rackOem = new ChassisDto.RackChassisRackScaleOem();
            rackOem.setRmmPresent(isRmmPresent(source));
            RackChassisAttributes rackChassisAttributes = source.getRackChassisAttributes();
            if (rackChassisAttributes != null) {
                rackOem.setUuid(rackChassisAttributes.getUuid());
                rackOem.setRackSupportsDisaggregatedPowerCooling(rackChassisAttributes.getRackSupportsDisaggregatedPowerCooling());
                rackOem.setGeoTag(rackChassisAttributes.getGeoTag());
            }
            target.getOem().getRackScaleOem().setRackChassisOem(rackOem);
            target.getOem().getRackScaleOem().setoDataType(ODATA_TYPE_RACK);
        }

        target.getOem().getRackScaleOem().getLocation().setId(source.getLocationId());
        target.getOem().getRackScaleOem().getLocation().setParentId(getParentLocation(source));
    }

    private void mapStatus(Chassis source, ChassisDto target) {
        ofNullable(source.getStatus()).ifPresent(target::setStatus);
    }

    private String getParentLocation(Chassis chassis) {
        Chassis parentChassis = chassis.getContainedByChassis();
        if (chassis.getLocationParentId() == null && parentChassis != null) {
            return parentChassis.getLocationId();
        }

        return chassis.getLocationParentId();
    }

    private boolean isRmmPresent(Chassis chassis) {
        return chassis
            .getManagers()
            .stream()
            .anyMatch(Manager::isPresent);
    }
}
