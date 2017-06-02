/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EthernetSwitchDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCHES;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class EthernetSwitchServiceImpl implements ReaderService<EthernetSwitchDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private GenericDao genericDao;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        return new CollectionDto(ETHERNET_SWITCHES, getAsIdSet(genericDao.findAll(EthernetSwitch.class)));
    }

    @Override
    public EthernetSwitchDto getResource(Context ethernetSwitch) throws ContextResolvingException {
        EthernetSwitch aSwitch = (EthernetSwitch) traverser.traverse(ethernetSwitch);

        return EthernetSwitchDto.newBuilder()
            .id(aSwitch.getId().toString())
            .name(aSwitch.getName())
            .description(aSwitch.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(aSwitch.getService(), aSwitch.getUnknownOems()))
            .switchId(aSwitch.getSwitchId())
            .manufacturer(aSwitch.getManufacturer())
            .model(aSwitch.getModel())
            .manufacturingDate(aSwitch.getManufacturingDate())
            .serialNumber(aSwitch.getSerialNumber())
            .partNumber(aSwitch.getPartNumber())
            .firmwareName(aSwitch.getFirmwareName())
            .firmwareVersion(aSwitch.getFirmwareVersion())
            .role(aSwitch.getRole())
            .status(aSwitch.getStatus())
            .chassis(toContext(aSwitch.getChassis()))
            .managedBy(asManagerContexts(aSwitch.getManagers()))
            .build();
    }
}
