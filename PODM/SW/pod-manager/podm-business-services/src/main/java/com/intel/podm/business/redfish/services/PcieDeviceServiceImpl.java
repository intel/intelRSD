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
import com.intel.podm.business.dto.redfish.PcieDeviceDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.PcieDevice;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.PCIE_DEVICES;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPcieDeviceFunctionContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class PcieDeviceServiceImpl implements ReaderService<PcieDeviceDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private GenericDao genericDao;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        List<PcieDevice> pcieDevices = genericDao.findAll(PcieDevice.class);
        return new CollectionDto(PCIE_DEVICES, getAsIdSet(pcieDevices));
    }

    @Override
    public PcieDeviceDto getResource(Context pcieDeviceContext) throws ContextResolvingException {
        PcieDevice pcieDevice = (PcieDevice) traverser.traverse(pcieDeviceContext);
        return map(pcieDevice);
    }

    private PcieDeviceDto map(PcieDevice pcieDevice) {
        return PcieDeviceDto.newBuilder()
            .id(pcieDevice.getId().toString())
            .name(pcieDevice.getName())
            .description(pcieDevice.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(pcieDevice.getService(), pcieDevice.getUnknownOems()))
            .assetTag(pcieDevice.getAssetTag())
            .manufacturer(pcieDevice.getManufacturer())
            .model(pcieDevice.getModel())
            .sku(pcieDevice.getSku())
            .serialNumber(pcieDevice.getSerialNumber())
            .partNumber(pcieDevice.getPartNumber())
            .deviceType(pcieDevice.getDeviceType())
            .firmwareVersion(pcieDevice.getFirmwareVersion())
            .status(pcieDevice.getStatus())
            .chassis(asChassisContexts(pcieDevice.getChassis()))
            .pcieFunctions(asPcieDeviceFunctionContexts(pcieDevice.getPcieDeviceFunctions()))
            .build();
    }
}
