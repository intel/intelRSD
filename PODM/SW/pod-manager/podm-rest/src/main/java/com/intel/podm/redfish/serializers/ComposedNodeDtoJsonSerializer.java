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

import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.redfish.json.templates.ComposedNodeJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.stream.Collectors.toSet;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public class ComposedNodeDtoJsonSerializer extends DtoJsonSerializer<ComposedNodeDto> {
    public ComposedNodeDtoJsonSerializer() {
        super(ComposedNodeDto.class);
    }

    @Override
    protected ComposedNodeJson translate(ComposedNodeDto composedNodeDto) {
        ComposedNodeJson composedNodeJson = new ComposedNodeJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        composedNodeJson.oDataContext = getContextFromId(oDataId);
        composedNodeJson.oDataId = oDataId;

        composedNodeJson.id = composedNodeDto.getId();
        composedNodeJson.name = composedNodeDto.getName();
        composedNodeJson.description = composedNodeDto.getDescription();
        composedNodeJson.uuid = composedNodeDto.getUuid();
        composedNodeJson.powerState = composedNodeDto.getPowerState();
        composedNodeJson.status = composedNodeDto.getStatus();
        fillProcessorSummary(composedNodeJson, composedNodeDto);
        fillMemorySummary(composedNodeJson, composedNodeDto);
        composedNodeJson.composedNodeState = composedNodeDto.getComposedNodeState();
        fillBoot(composedNodeJson, composedNodeDto);
        fillLinks(composedNodeJson, composedNodeDto.getLinks());
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
        if (composedNodeDto.getBoot() == null) {
            return;
        }

        composedNodeJson.boot.bootSourceOverrideEnabled = composedNodeDto.getBoot().getBootSourceOverrideEnabled();
        composedNodeJson.boot.bootSourceOverrideTarget = composedNodeDto.getBoot().getBootSourceOverrideTarget();
        composedNodeJson.boot.bootSourceAllowableValues.addAll(composedNodeDto.getBoot().getBootSourceOverrideTargetAllowableValues());
        composedNodeJson.boot.bootSourceOverrideMode = composedNodeDto.getBoot().getBootSourceOverrideMode();
        composedNodeJson.boot.bootSourceOverrideModeAllowableValues.addAll(composedNodeDto.getBoot().getBootSourceOverrideModeAllowableValues());
    }

    private void fillLinks(ComposedNodeJson json, ComposedNodeDto.Links links) {
        json.links.computerSystem = ODataIdFromContextHelper.asOdataId(links.getComputerSystem());
        json.links.processors.addAll(links.getProcessors().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.memory.addAll(links.getMemoryModules().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.ethernetInterfaces.addAll(links.getEthernetInterfaces().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.remoteDrives.addAll(links.getRemoteDrives().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.localDrives.addAll(links.getLocalDrives().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.localDrives.addAll(links.getSimpleStorage().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.localDrives.addAll(links.getPcieDrives().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
        json.links.managedBy.addAll(links.getManagedBy().stream().map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
    }

    private void fillActions(ComposedNodeJson composedNodeJson, ComposedNodeDto composedNodeDto) {
        composedNodeJson.actions.resetAction.target = composedNodeJson.oDataId + "/Actions/ComposedNode.Reset";
        composedNodeJson.actions.resetAction.allowableResetTypes.addAll(composedNodeDto.getActions().getAllowableResetTypes());
        composedNodeJson.actions.assembleAction.target = composedNodeJson.oDataId + "/Actions/ComposedNode.Assemble";

        composedNodeJson.actions.attachEndpointAction.target = composedNodeJson.oDataId + "/Actions/ComposedNode.AttachEndpoint";
        composedNodeJson.actions.attachEndpointAction.allowableValues.addAll(composedNodeDto.getLinks().getAvailablePcieDrives().stream()
            .map(ODataIdFromContextHelper::asOdataId).collect(toSet()));

        composedNodeJson.actions.detachEndpointAction.target = composedNodeJson.oDataId + "/Actions/ComposedNode.DetachEndpoint";
        composedNodeJson.actions.detachEndpointAction.allowableValues.addAll(composedNodeDto.getLinks().getPcieDrives().stream()
            .map(ODataIdFromContextHelper::asOdataId).collect(toSet()));
    }
}
