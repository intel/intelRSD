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

import com.intel.podm.business.dto.redfish.ComputerSystemDto;
import com.intel.podm.business.dto.redfish.attributes.BootDto;
import com.intel.podm.business.dto.redfish.attributes.SystemDeviceDto;
import com.intel.podm.redfish.json.templates.ComputerSystemJson;
import com.intel.podm.redfish.json.templates.attributes.DeviceJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.List;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdsCollection;
import static java.net.URI.create;
import static java.util.Collections.emptyList;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

public class SystemDtoJsonSerializer extends DtoJsonSerializer<ComputerSystemDto> {
    public SystemDtoJsonSerializer() {
        super(ComputerSystemDto.class);
    }

    @Override
    protected ComputerSystemJson translate(ComputerSystemDto dto) {
        ComputerSystemJson computerSystemJson = new ComputerSystemJson("#ComputerSystem.1.0.0.ComputerSystem");
        ODataId oDataId = oDataId(context.getRequestPath());

        computerSystemJson.oDataId = oDataId;
        computerSystemJson.oDataContext = getContextFromId(oDataId);

        computerSystemJson.id = dto.getId();
        computerSystemJson.sku = dto.getSku();
        computerSystemJson.name = dto.getName();
        computerSystemJson.boot = mapToBootJson(dto.getBoot());
        computerSystemJson.uuid = dto.getUuid();
        computerSystemJson.model = dto.getModel();
        computerSystemJson.status = dto.getStatus();
        computerSystemJson.assetTag = dto.getAssetTag();
        computerSystemJson.hostName = dto.getHostName();
        computerSystemJson.powerState = dto.getPowerState();
        computerSystemJson.biosVersion = dto.getBiosVersion();
        computerSystemJson.description = dto.getDescription();
        computerSystemJson.indicatorLed = dto.getIndicatorLed();
        computerSystemJson.manufacturer = dto.getManufacturer();
        computerSystemJson.serialNumber = dto.getSerialNumber();
        computerSystemJson.systemType = dto.getSystemType();
        computerSystemJson.partNumber = dto.getPartNumber();

        computerSystemJson.memorySummary.status = dto.getMemoryStatus();
        computerSystemJson.memorySummary.totalSystemMemoryGiB = dto.getTotalSystemMemoryGiB();

        computerSystemJson.processorSummary.count = dto.getProcessorsCount();
        computerSystemJson.processorSummary.model = dto.getProcessorsModel();
        computerSystemJson.processorSummary.status = dto.getProcessorsStatus();

        computerSystemJson.processors = oDataId(create(oDataId + "/Processors"));
        computerSystemJson.memory = oDataId(create(oDataId + "/Memory"));
        computerSystemJson.memoryChunks = oDataId(create(oDataId + "/MemoryChunks"));
        computerSystemJson.simpleStorage = oDataId(create(oDataId + "/SimpleStorage"));
        computerSystemJson.ethernetInterfaces = oDataId(create(oDataId + "/EthernetInterfaces"));

        computerSystemJson.actions.resetActionJson.target = oDataId + "/Actions/ComputerSystem.Reset";
        computerSystemJson.actions.resetActionJson.allowableResetTypes = dto.getAllowableResetTypes();
        computerSystemJson.actions.oem.rackScaleOem.startDeepDiscoveryActionJson.target = oDataId + "/Actions/ComputerSystem.StartDeepDiscovery";

        computerSystemJson.oem.rackScaleOem.odataType = "#Intel.Oem.ComputerSystem";
        computerSystemJson.oem.rackScaleOem.pciDevices = mapToPciDevices(dto.getPciDevices());
        computerSystemJson.oem.rackScaleOem.memorySockets = dto.getMemorySockets();
        computerSystemJson.oem.rackScaleOem.discoveryState = dto.getDiscoveryState();
        computerSystemJson.oem.rackScaleOem.processorSockets = dto.getProcessorSockets();
        computerSystemJson.oem.rackScaleOem.adapters = oDataId(create(oDataId + "/Adapters"));

        computerSystemJson.links.chassis.addAll(oDataIdsCollection(dto.getChassis()));
        computerSystemJson.links.managedBy.addAll(oDataIdsCollection(dto.getManagedBy()));


        return computerSystemJson;
    }

    private static List<DeviceJson> mapToPciDevices(List<SystemDeviceDto> pciDevices) {
        return isNull(pciDevices)
                ?
                emptyList()
                :
                pciDevices.stream()
                        .map(systemDeviceDto -> new DeviceJson(systemDeviceDto.getVendorId(), systemDeviceDto.getDeviceId()))
                        .collect(toList());
    }

    private static ComputerSystemJson.Boot mapToBootJson(BootDto dto) {
        //Since is optional value it can be null
        if (dto == null) {
            return null;
        }

        ComputerSystemJson.Boot boot  = new ComputerSystemJson.Boot();
        boot.bootSourceOverrideEnabled = dto.getBootSourceOverrideEnabled();
        boot.bootSourceOverrideTarget = dto.getBootSourceOverrideTarget();
        boot.bootSourceAllowableValues.addAll(dto.getBootSourceOverrideTargetAllowableValues());

        return boot;
    }
}
