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
import com.intel.podm.business.dto.redfish.PcieDeviceFunctionDto;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.business.entities.redfish.PcieDeviceFunction;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.StorageControllerDtoHelper;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PCIE_DEVICE_FUNCTIONS;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetInterfaceContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class PcieDeviceFunctionServiceImpl implements ReaderService<PcieDeviceFunctionDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Inject
    private StorageControllerDtoHelper storageControllerDtoHelper;

    @Override
    public CollectionDto getCollection(Context pcieDeviceContext) throws ContextResolvingException {
        PcieDevice pcieDevice = (PcieDevice) traverser.traverse(pcieDeviceContext);
        return new CollectionDto(PCIE_DEVICE_FUNCTIONS, getAsIdSet(pcieDevice.getPcieDeviceFunctions()));
    }

    @Override
    public PcieDeviceFunctionDto getResource(Context pcieDeviceFunctionContext) throws ContextResolvingException {
        PcieDeviceFunction pcieDeviceFunction = (PcieDeviceFunction) traverser.traverse(pcieDeviceFunctionContext);
        return map(pcieDeviceFunction);
    }

    private PcieDeviceFunctionDto map(PcieDeviceFunction pcieDeviceFunction) {
        return PcieDeviceFunctionDto.newBuilder()
            .id(pcieDeviceFunction.getId().toString())
            .name(pcieDeviceFunction.getName())
            .description(pcieDeviceFunction.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(pcieDeviceFunction.getService(), pcieDeviceFunction.getUnknownOems()))
            .functionId(pcieDeviceFunction.getFunctionId())
            .deviceClass(pcieDeviceFunction.getDeviceClass())
            .functionType(pcieDeviceFunction.getFunctionType())
            .deviceId(pcieDeviceFunction.getDeviceId())
            .vendorId(pcieDeviceFunction.getVendorId())
            .classCode(pcieDeviceFunction.getClassCode())
            .revisionId(pcieDeviceFunction.getRevisionId())
            .subsystemId(pcieDeviceFunction.getSubsystemId())
            .subsystemVendorId(pcieDeviceFunction.getSubsystemVendorId())
            .status(pcieDeviceFunction.getStatus())
            .pcieDevice(toContext(pcieDeviceFunction.getPcieDevice()))
            .drives(asDriveContexts(pcieDeviceFunction.getDrives()))
            .ethernetInterfaces(asEthernetInterfaceContexts(pcieDeviceFunction.getEthernetInterfaces()))
            .storageControllers(pcieDeviceFunction.getStorageControllers().stream()
                .map(storageController -> storageControllerDtoHelper.createStorageControllerDto(storageController))
                .collect(toSet()))
            .build();
    }
}
