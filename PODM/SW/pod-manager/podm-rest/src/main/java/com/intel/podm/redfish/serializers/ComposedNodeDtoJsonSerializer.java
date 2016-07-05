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

import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.redfish.json.templates.ComposedNodeJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.odataid.ODataIds;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

public class ComposedNodeDtoJsonSerializer extends DtoJsonSerializer<ComposedNodeDto> {
    public ComposedNodeDtoJsonSerializer() {
        super(ComposedNodeDto.class);
    }

    @Override
    protected ComposedNodeJson translate(ComposedNodeDto composedNodeDto) {
        ComposedNodeJson composedNodeJson = new ComposedNodeJson();
        ODataId oDataId = oDataId(context.getRequestPath());

        composedNodeJson.oDataContext = getContextFromId(oDataId);
        composedNodeJson.oDataId = oDataId;

        composedNodeJson.id = composedNodeDto.getId();
        composedNodeJson.name = composedNodeDto.getName();
        composedNodeJson.description = composedNodeDto.getDescription();
        composedNodeJson.systemType = composedNodeDto.getSystemType();
        composedNodeJson.assetTag = composedNodeDto.getAssetTag();
        composedNodeJson.manufacturer = composedNodeDto.getManufacturer();
        composedNodeJson.model = composedNodeDto.getModel();
        composedNodeJson.sku = composedNodeDto.getSku();
        composedNodeJson.serialNumber = composedNodeDto.getSerialNumber();
        composedNodeJson.partNumber = composedNodeDto.getPartNumber();
        composedNodeJson.uuid = composedNodeDto.getUuid();
        composedNodeJson.hostName = composedNodeDto.getHostName();
        composedNodeJson.powerState = composedNodeDto.getPowerState();
        composedNodeJson.biosVersion = composedNodeDto.getBiosVersion();
        composedNodeJson.status = composedNodeDto.getStatus();
        fillProcessorSummary(composedNodeJson, composedNodeDto);
        fillMemorySummary(composedNodeJson, composedNodeDto);
        composedNodeJson.composedNodeState = composedNodeDto.getComposedNodeState();
        fillBoot(composedNodeJson, composedNodeDto);
        fillLinks(composedNodeJson, composedNodeDto);
        fillActions(composedNodeJson, composedNodeDto);

        return composedNodeJson;
    }

    private void fillProcessorSummary(ComposedNodeJson composedNodeJson, ComposedNodeDto composedNodeDto) {
        composedNodeJson.processors.count = composedNodeDto.getComposedNodeProcessors().getCount();
        composedNodeJson.processors.model = composedNodeDto.getComposedNodeProcessors().getModel();
        composedNodeJson.processors.status = composedNodeDto.getComposedNodeProcessors().getStatus();
    }

    private void fillMemorySummary(ComposedNodeJson composedNodeJson, ComposedNodeDto composedNodeDto) {
        composedNodeJson.memory.totalSystemMemoryGiB = composedNodeDto.getComposedNodeMemory().getTotalSystemMemoryGiB();
        composedNodeJson.memory.status = composedNodeDto.getComposedNodeMemory().getStatus();
    }

    private void fillBoot(ComposedNodeJson composedNodeJson, ComposedNodeDto composedNodeDto) {
        if (isNull(composedNodeDto.getBoot())) {
            return;
        }

        composedNodeJson.boot.bootSourceOverrideEnabled = composedNodeDto.getBoot().getBootSourceOverrideEnabled();
        composedNodeJson.boot.bootSourceOverrideTarget = composedNodeDto.getBoot().getBootSourceOverrideTarget();
        composedNodeJson.boot.bootSourceAllowableValues.addAll(composedNodeDto.getBoot().getBootSourceOverrideTargetAllowableValues());
    }

    private void fillLinks(ComposedNodeJson composedNodeJson, ComposedNodeDto composedNodeDto) {
        composedNodeJson.links.computerSystem = ODataIds.oDataIdFromContext(composedNodeDto.getComputerSystem());
        composedNodeJson.links.processors.addAll(composedNodeDto.getProcessors().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
        composedNodeJson.links.memory.addAll(composedNodeDto.getMemoryModules().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
        composedNodeJson.links.ethernetInterfaces.addAll(composedNodeDto.getEthernetInterfaces().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
        composedNodeJson.links.remoteDrives.addAll(composedNodeDto.getRemoteDrives().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
        composedNodeJson.links.localDrives.addAll(composedNodeDto.getLocalDrives().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
        composedNodeJson.links.localDrives.addAll(composedNodeDto.getSimpleStorage().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
        composedNodeJson.links.managedBy.addAll(composedNodeDto.getManagedBy().stream().map(ODataIds::oDataIdFromContext)
                .collect(toList()));
    }

    private void fillActions(ComposedNodeJson composedNodeJson, ComposedNodeDto composedNodeDto) {
        composedNodeJson.actions.resetAction.target = composedNodeJson.oDataId + "/Actions/ComposedNode.Reset";
        composedNodeJson.actions.resetAction.allowableResetTypes.addAll(composedNodeDto.getAllowableResetTypes());
        composedNodeJson.actions.assembleAction.target = composedNodeJson.oDataId + "/Actions/ComposedNode.Assemble";
    }
}
