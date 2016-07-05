/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.ChassisDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.properties.RackChassisAttributes;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ChassisService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.CHASSIS;
import static com.intel.podm.business.redfish.Contexts.asChassisContexts;
import static com.intel.podm.business.redfish.Contexts.asComputerSystemContexts;
import static com.intel.podm.business.redfish.Contexts.asEthernetSwitchContexts;
import static com.intel.podm.business.redfish.Contexts.asManagerContexts;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.ServiceType.RMM;
import static java.util.Objects.nonNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ChassisServiceImpl implements ChassisService {
    @Inject
    private GenericDao genericDao;

    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getChassisCollection() {
        Collection<Chassis> chassisCollection = genericDao.findAll(Chassis.class);
        return new CollectionDto(CHASSIS, getAsIdList(chassisCollection));
    }

    @Override
    public ChassisDto getChassis(Context chassisContext) throws EntityNotFoundException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);
        return map(chassis);
    }

    private ChassisDto map(Chassis chassis) {
        RackChassisAttributes rackChassisAttributes = chassis.getRackChassisAttributes();
        ChassisDto.Builder builder = ChassisDto
                .newBuilder()
                .id(chassis.getId().toString())
                .chassisType(chassis.getChassisType())
                .name(chassis.getName())
                .description(chassis.getDescription())
                .manufacturer(chassis.getManufacturer())
                .model(chassis.getModel())
                .sku(chassis.getSku())
                .serialNumber(chassis.getSerialNumber())
                .partNumber(chassis.getPartNumber())
                .assetTag(chassis.getAssetTag())
                .indicatorLed(chassis.getIndicatorLed())
                .locationId(chassis.getLocationId())
                .locationParentId(chassis.getLocationParentId())
                .status(chassis.getStatus())
                .contains(asChassisContexts(chassis.getContainedChassis()))
                .switches(asEthernetSwitchContexts(chassis.getContainedSwitches()))
                .containedBy(nonNull(chassis.getContainedBy()) ? toContext(chassis.getContainedBy()) : null)
                .computerSystems(asComputerSystemContexts(chassis.getComputerSystems()))
                .managedBy(asManagerContexts(chassis.getManagers()))
                .managersInChassis(asManagerContexts(chassis.getManagersInChassis()))
                .rmmPresent(chassis.getService() != null && chassis.getService().getServiceType().equals(RMM));

                addChassisAttributes(rackChassisAttributes, builder);
                return builder.build();
    }

    private void addChassisAttributes(RackChassisAttributes rackChassisAttributes, ChassisDto.Builder builder) {
       if (rackChassisAttributes != null) {
           builder
               .uuid(rackChassisAttributes.getUuid())
               .rackSupportsDisaggregatedPowerCooling(rackChassisAttributes.getRackSupportsDisaggregatedPowerCooling())
               .rackPuid(rackChassisAttributes.getRackPuid())
               .geoTag(rackChassisAttributes.getGeoTag());
       }
    }
}
