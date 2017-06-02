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
import com.intel.podm.business.dto.redfish.SwitchDto;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCHES;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class SwitchServiceImpl implements ReaderService<SwitchDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context fabricContext) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(fabricContext);
        return new CollectionDto(FABRIC_SWITCHES, getAsIdSet(fabric.getSwitches()));
    }

    @Override
    public SwitchDto getResource(Context context) throws ContextResolvingException {
        Switch fabricSwitch = (Switch) traverser.traverse(context);
        return map(fabricSwitch);
    }

    private SwitchDto map(Switch fabricSwitch) {
        return SwitchDto.newBuilder()
            .id(fabricSwitch.getId().toString())
            .name(fabricSwitch.getName())
            .description(fabricSwitch.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(fabricSwitch.getService(), fabricSwitch.getUnknownOems()))
            .sku(fabricSwitch.getSku())
            .model(fabricSwitch.getModel())
            .status(fabricSwitch.getStatus())
            .assetTag(fabricSwitch.getAssetTag())
            .domainId(fabricSwitch.getDomainId())
            .isManaged(fabricSwitch.getManaged())
            .powerState(fabricSwitch.getPowerState())
            .switchType(fabricSwitch.getSwitchType())
            .partNumber(fabricSwitch.getPartNumber())
            .manufacturer(fabricSwitch.getManufacturer())
            .indicatorLed(fabricSwitch.getIndicatorLed())
            .serialNumber(fabricSwitch.getSerialNumber())
            .chassis(asChassisContexts(fabricSwitch.getChassis()))
            .managedBy(asManagerContexts(fabricSwitch.getManagers()))
            .totalSwitchWidth(fabricSwitch.getTotalSwitchWidth())
            .chassis(asChassisContexts(fabricSwitch.getChassis()))
            .allowableResetTypes(fabricSwitch.getAllowableResetTypes())
            .build();
    }
}
