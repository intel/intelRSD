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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.PortDto;
import com.intel.podm.redfish.json.templates.PortJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

@SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
public class PortDtoJsonSerializer extends BaseDtoJsonSerializer<PortDto> {
    protected PortDtoJsonSerializer() {
        super(PortDto.class);
    }

    @Override
    protected PortJson translate(PortDto portDto) {
        PortJson portJson = new PortJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        portJson.oDataId = oDataId;
        portJson.oDataContext = getContextFromId(oDataId);

        portJson.id = portDto.getId();
        portJson.name = portDto.getName();
        portJson.description = portDto.getDescription();
        portJson.status = portDto.getStatus();
        portJson.portId = portDto.getPortId();
        portJson.portType = portDto.getPortType();
        portJson.currentSpeedGbps = portDto.getCurrentSpeedGbps();
        portJson.width = portDto.getWidth();
        portJson.maxSpeedGbps = portDto.getMaxSpeedGbps();
        portJson.portProtocol = portDto.getPortProtocol();
        portJson.oem.rackScale.cableIds.addAll(portDto.getPcieConnectionId());
        portJson.actions.reset.target = oDataId + "/Actions/Port.Reset";
        portJson.actions.reset.allowableResetTypes.addAll(portDto.getAllowableResetValues());
        portJson.links.connectedSwitches.addAll(asOdataIdSet(portDto.getLinks().getConnectedSwitches()));
        portJson.links.connectedSwitchPorts.addAll(asOdataIdSet(portDto.getLinks().getConnectedSwitchPorts()));
        portJson.links.associatedEndpoints.addAll(asOdataIdSet(portDto.getLinks().getAssociatedEndpoints()));

        return portJson;
    }
}
