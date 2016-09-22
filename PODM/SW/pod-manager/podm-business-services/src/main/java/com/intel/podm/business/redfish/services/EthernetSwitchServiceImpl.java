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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EthernetSwitchDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.EthernetSwitchService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCHES;
import static com.intel.podm.business.redfish.Contexts.asManagerContexts;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class EthernetSwitchServiceImpl implements EthernetSwitchService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private GenericDao genericDao;

    @Override
    public CollectionDto getEthernetSwitchCollection() {
        return new CollectionDto(ETHERNET_SWITCHES, getAsIdList(genericDao.findAll(EthernetSwitch.class)));
    }

    @Override
    public EthernetSwitchDto getEthernetSwitch(Context ethernetSwitch) throws EntityNotFoundException {
        EthernetSwitch aSwitch = (EthernetSwitch) traverser.traverse(ethernetSwitch);
        return EthernetSwitchDto.newBuilder()
                .id(aSwitch.getId())
                .switchId(aSwitch.getSwitchId())
                .name(aSwitch.getName())
                .description(aSwitch.getDescription())
                .manufacturer(aSwitch.getManufacturer())
                .model(aSwitch.getModel())
                .manufacturingDate(aSwitch.getManufacturingDate())
                .serialNumber(aSwitch.getSerialNumber())
                .partNumber(aSwitch.getPartNumber())
                .firmwareName(aSwitch.getFirmwareName())
                .firmwareVersion(aSwitch.getFirmwareVersion())
                .role(aSwitch.getRole())
                .status(aSwitch.getStatus())
                .chassis(aSwitch.getChassis() != null ? toContext(aSwitch.getChassis()) : null)
                .managedBy(asManagerContexts(aSwitch.getManagers()))
                .build();
    }
}
