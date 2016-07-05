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

import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.redfish.json.templates.EthernetSwitchPortJson;
import com.intel.podm.redfish.json.templates.EthernetSwitchPortJson.Oem.RackScaleOem;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.List;

import static com.intel.podm.redfish.serializers.EthernetInterfaceJsonSerializer.mapToIpv4Addresses;
import static com.intel.podm.redfish.serializers.EthernetInterfaceJsonSerializer.mapToIpv6Addresses;
import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdFromContext;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdsCollection;
import static java.net.URI.create;
import static java.util.Collections.emptyList;

public class EthernetSwitchPortDtoJsonSerializer extends DtoJsonSerializer<EthernetSwitchPortDto> {

    public EthernetSwitchPortDtoJsonSerializer() {
        super(EthernetSwitchPortDto.class);
    }

    @Override
    protected EthernetSwitchPortJson translate(EthernetSwitchPortDto dto) {
        EthernetSwitchPortJson port = new EthernetSwitchPortJson();
        ODataId oDataId = oDataId(context.getRequestPath());
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
        port.vlans = oDataId(create(oDataId + "/VLANs"));
        if (dto.getNeighborInterface() != null) {
            port.links.oem.rackScaleOem = new RackScaleOem(oDataIdFromContext(dto.getNeighborInterface()));
        }
        mapLinks(dto, port);
        return port;
    }

    private void mapLinks(EthernetSwitchPortDto dto, EthernetSwitchPortJson port) {
        port.links.primaryVlan = oDataIdFromContext(dto.getPrimaryVlan());
        port.links.switchLink = oDataIdFromContext(dto.getSwitchContext());
        port.links.portMembers = mapToPortMembers(dto.getPortMembers());
        port.links.memberOfPort = oDataIdFromContext(dto.getMemberOfPort());
    }

    private List<ODataId> mapToPortMembers(List<Context> portMembers) {
        return portMembers == null
                ? emptyList()
                : oDataIdsCollection(portMembers.stream().toArray(Context[]::new));
    }
}
