/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.common.types.redfish.RedfishComputerSystem;
import com.intel.podm.redfish.json.templates.ComputerSystemJson;
import com.intel.podm.redfish.json.templates.attributes.ComputerSystemDeviceJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.List;
import java.util.Set;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public class ComputerSystemDtoJsonSerializer extends BaseDtoJsonSerializer<ComputerSystemDto> {
    public ComputerSystemDtoJsonSerializer() {
        super(ComputerSystemDto.class);
    }

    @Override
    protected ComputerSystemJson translate(ComputerSystemDto dto) {
        ComputerSystemJson computerSystemJson = new ComputerSystemJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

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

        computerSystemJson.processors = oDataIdFromUri(create(oDataId + "/Processors"));
        computerSystemJson.memory = oDataIdFromUri(create(oDataId + "/Memory"));
        computerSystemJson.simpleStorage = oDataIdFromUri(create(oDataId + "/SimpleStorage"));
        computerSystemJson.storage = oDataIdFromUri(create(oDataId + "/Storage"));
        computerSystemJson.ethernetInterfaces = oDataIdFromUri(create(oDataId + "/EthernetInterfaces"));
        computerSystemJson.pcieDevices.addAll(asOdataIdSet(dto.getPcieDevices()));
        computerSystemJson.pcieDeviceFunctions.addAll(asOdataIdSet(dto.getPciDeviceFunctions()));
        computerSystemJson.networkInterfaces = oDataIdFromUri(create(oDataId + "/NetworkInterfaces"));

        computerSystemJson.actions.reset.target = oDataId + "/Actions/ComputerSystem.Reset";
        computerSystemJson.actions.reset.allowableResetTypes.addAll(dto.getAllowableResetTypes());
        computerSystemJson.actions.oem.startDeepDiscovery.target = oDataId + "/Actions/ComputerSystem.StartDeepDiscovery";

        computerSystemJson.oem.rackScaleOem.odataType = "#Intel.Oem.ComputerSystem";
        computerSystemJson.oem.rackScaleOem.pciDevices = mapToPciDevices(dto.getPciDevices());
        computerSystemJson.oem.rackScaleOem.memorySockets = dto.getMemorySockets();
        computerSystemJson.oem.rackScaleOem.pcieConnectionId = dto.getPcieConnectionId();
        computerSystemJson.oem.rackScaleOem.discoveryState = dto.getDiscoveryState();
        computerSystemJson.oem.rackScaleOem.processorSockets = dto.getProcessorSockets();

        computerSystemJson.links.chassis.addAll(asOdataIdSet(dto.getLinks().getChassis()));
        computerSystemJson.links.managedBy.addAll(asOdataIdSet(dto.getLinks().getManagedBy()));
        computerSystemJson.links.endpoints.addAll(asOdataIdSet(dto.getLinks().getEndpoints()));

        return computerSystemJson;
    }

    private static List<ComputerSystemDeviceJson> mapToPciDevices(Set<RedfishComputerSystem.Device> pciDevices) {
        return pciDevices == null
            ? emptyList()
            : pciDevices.stream()
            .map(systemDeviceDto -> new ComputerSystemDeviceJson(systemDeviceDto.getVendorId(), systemDeviceDto.getDeviceId()))
            .collect(toList());
    }

    private static ComputerSystemJson.Boot mapToBootJson(RedfishComputerSystem.Boot dto) {
        //Since is optional value it can be null
        if (dto == null) {
            return null;
        }

        ComputerSystemJson.Boot boot = new ComputerSystemJson.Boot();
        boot.oDataType = "#ComputerSystem.v1_1_0.Boot";
        boot.bootSourceOverrideEnabled = dto.getBootSourceOverrideEnabled();
        boot.bootSourceOverrideTarget = dto.getBootSourceOverrideTarget();
        boot.bootSourceAllowableValues.addAll(dto.getBootSourceOverrideTargetAllowableValues());
        boot.bootSourceOverrideMode = dto.getBootSourceOverrideMode();
        boot.bootSourceOverrideModeAllowableValues.addAll(dto.getBootSourceOverrideModeAllowableValues());
        return boot;
    }
}
