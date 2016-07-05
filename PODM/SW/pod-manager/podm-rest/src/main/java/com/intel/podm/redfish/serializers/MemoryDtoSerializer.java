/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.MemoryDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryLocationDto;
import com.intel.podm.business.dto.redfish.attributes.MemoryRegionDto;
import com.intel.podm.redfish.json.templates.BaseJson;
import com.intel.podm.redfish.json.templates.MemoryJson;
import com.intel.podm.redfish.json.templates.attributes.MemoryLocationJson;
import com.intel.podm.redfish.json.templates.attributes.MemoryRegionJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.Collection;
import java.util.List;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

public class MemoryDtoSerializer extends DtoJsonSerializer<MemoryDto> {

    private static final String OEM_ODATA_TYPE = "#Intel.Oem.Memory";

    protected MemoryDtoSerializer() {
        super(MemoryDto.class);
    }

    @Override
    protected BaseJson translate(MemoryDto dto) {

        MemoryJson memoryJson = new MemoryJson();

        ODataId oDataId = ODataId.oDataId(context.getRequestPath());
        memoryJson.oDataId = oDataId;
        memoryJson.oDataContext = getContextFromId(oDataId);
        memoryJson.id = dto.getId();
        memoryJson.name = dto.getName();
        memoryJson.description = dto.getDescription();
        memoryJson.memoryDeviceType = dto.getMemoryDeviceType();
        memoryJson.baseModuleType = dto.getBaseModuleType();
        memoryJson.memoryType = dto.getMemoryType();
        memoryJson.capacityMib = dto.getCapacityMib();
        memoryJson.dataWidthBits = dto.getDataWidthBits();
        memoryJson.busWidthBits = dto.getBusWidthBits();
        memoryJson.manufacturer = dto.getManufacturer();
        memoryJson.serialNumber = dto.getSerialNumber();
        memoryJson.partNumber = dto.getPartNumber();
        memoryJson.firmwareRevision = dto.getFirmwareRevision();
        memoryJson.firmwareApiVersion = dto.getFirmwareApiVersion();
        memoryJson.vendorId = dto.getVendorId();
        memoryJson.deviceId = dto.getDeviceId();
        memoryJson.rankCount = dto.getRankCount();
        memoryJson.deviceLocator = dto.getDeviceLocator();
        memoryJson.errorCorrection = dto.getErrorCorrection();
        memoryJson.status = dto.getStatus();
        memoryJson.operatingSpeedMhz = dto.getOperatingSpeedMhz();
        memoryJson.memoryLocation = translateMemoryLocation(dto.getMemoryLocation());

        processCollections(memoryJson, dto);
        processOem(memoryJson, dto);

        return memoryJson;
    }

    private MemoryLocationJson translateMemoryLocation(MemoryLocationDto memoryLocationDto) {
        if (memoryLocationDto == null) {
            return null;
        }

        MemoryLocationJson memoryLocationJson = new MemoryLocationJson();
        memoryLocationJson.socket = memoryLocationDto.getLocationSocket();
        memoryLocationJson.memoryController = memoryLocationDto.getLocationMemoryController();
        memoryLocationJson.channel = memoryLocationDto.getLocationChannel();
        memoryLocationJson.slot = memoryLocationDto.getLocationSlot();

        return memoryLocationJson;
    }

    private void processCollections(MemoryJson memoryJson, MemoryDto dto) {
        memoryJson.memoryMedia.addAll(dto.getMemoryMedia());
        memoryJson.allowedSpeedsMhz.addAll(dto.getAllowedSpeedsMhz());
        memoryJson.functionClasses.addAll(dto.getFunctionClasses());
        memoryJson.regions.addAll(processMemoryRegions(dto.getRegions()));
        memoryJson.operatingMemoryModes.addAll(dto.getOperatingMemoryModes());
    }

    private List<MemoryRegionJson> processMemoryRegions(Collection<MemoryRegionDto> regions) {
        if (regions == null) {
            return emptyList();
        }

        return regions.stream()
                .map(reg -> new MemoryRegionJson(reg.getRegionId(), reg.getMemoryClassification(), reg.getOffsetMib(), reg.getSizeMib()))
                .collect(toList());
    }

    private void processOem(MemoryJson memoryJson, MemoryDto dto) {
        memoryJson.oem.rackScaleOem.odataType = OEM_ODATA_TYPE;
        memoryJson.oem.rackScaleOem.voltageVolt = dto.getVoltageVolt();
    }
}
