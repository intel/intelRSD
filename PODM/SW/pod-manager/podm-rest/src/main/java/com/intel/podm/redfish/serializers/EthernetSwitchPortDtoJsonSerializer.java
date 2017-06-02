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

import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.redfish.json.templates.EthernetSwitchPortJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;
import com.intel.podm.rest.representation.json.templates.RedfishErrorResponseJson.ExtendedInfoJson;

import java.util.List;

import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static com.intel.podm.redfish.serializers.EthernetInterfaceJsonSerializer.mapToIpv4Addresses;
import static com.intel.podm.redfish.serializers.EthernetInterfaceJsonSerializer.mapToIpv6Addresses;
import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public class EthernetSwitchPortDtoJsonSerializer extends BaseDtoJsonSerializer<EthernetSwitchPortDto> {
    public EthernetSwitchPortDtoJsonSerializer() {
        super(EthernetSwitchPortDto.class);
    }

    @Override
    protected EthernetSwitchPortJson translate(EthernetSwitchPortDto dto) {
        EthernetSwitchPortJson port = new EthernetSwitchPortJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        port.oDataId = oDataId;
        port.oDataContext = getContextFromId(oDataId);
        port.id = dto.getId();
        port.name = dto.getName();
        port.description = dto.getDescription();
        port.portId = dto.getPortId();
        port.status = dto.getStatus();
        port.portMode = dto.getPortMode();
        port.portType = dto.getPortType();
        port.linkType = dto.getLinkType();
        port.autosense = dto.getAutosense();
        port.portClass = dto.getPortClass();
        port.frameSize = dto.getFrameSize();
        port.fullDuplex = dto.getFullDuplex();
        port.macAddress = dto.getMacAddress();
        port.neighborMac = dto.getNeighborMac();
        port.linkSpeedMbps = dto.getLinkSpeedMbps();
        port.ipv4Addresses = mapToIpv4Addresses(dto.getIpv4Addresses());
        port.ipv6Addresses = mapToIpv6Addresses(dto.getIpv6Addresses());
        port.operationalState = dto.getOperationalState();
        port.neighborInfo = dto.getNeighborInfo();
        port.administrativeState = dto.getAdministrativeState();
        port.vlans = oDataIdFromUri(create(oDataId + "/VLANs"));
        mapLinks(dto.getLinks(), port);
        return port;
    }

    private void mapLinks(EthernetSwitchPortDto.Links links, EthernetSwitchPortJson port) {
        handleNeighborInterface(links, port);
        port.links.primaryVlan = asOdataId(links.getPrimaryVlan());
        port.links.switchLink = asOdataId(links.getSwitchContext());
        port.links.portMembers.addAll(asOdataIdSet(links.getPortMembers()));
        port.links.memberOfPort = asOdataId(links.getMemberOfPort());
    }

    private void handleNeighborInterface(EthernetSwitchPortDto.Links links, EthernetSwitchPortJson port) {
        Context neighborInterfaceContext = links.getNeighborInterface();
        List<ExtendedInfoJson> neighborInterfaceExtendedInfoJson =
            nullOrEmpty(links.getNeighborInterfaceExtendedInfo()) ? null : links.getNeighborInterfaceExtendedInfo().stream()
                .map(extendedInfo -> new ExtendedInfoJson(extendedInfo.getMessageId(), extendedInfo.getMessage()))
                .collect(toList());

        if (neighborInterfaceContext != null || neighborInterfaceExtendedInfoJson != null) {
            EthernetSwitchPortJson.Links.Oem.RackScaleOem rackScaleOem = port.links.oem.createNewRackScaleOem();
            rackScaleOem.neighborInterface = asOdataId(neighborInterfaceContext);
            rackScaleOem.neighborInterfaceExtendedInfo = neighborInterfaceExtendedInfoJson;
            port.links.oem.rackScaleOem = rackScaleOem;
        }
    }
}
