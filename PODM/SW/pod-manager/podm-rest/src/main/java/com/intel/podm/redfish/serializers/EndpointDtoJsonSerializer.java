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

import com.intel.podm.business.dto.redfish.EndpointDto;
import com.intel.podm.business.dto.redfish.attributes.ConnectedEntityDto;
import com.intel.podm.business.dto.redfish.attributes.IdentifierDto;
import com.intel.podm.business.dto.redfish.attributes.PciIdDto;
import com.intel.podm.redfish.json.templates.EndpointJson;
import com.intel.podm.redfish.json.templates.attributes.ConnectedEntityJson;
import com.intel.podm.redfish.json.templates.attributes.IdentifierJson;
import com.intel.podm.redfish.json.templates.attributes.PciIdJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.Set;

import static com.intel.podm.redfish.serializers.RedundancyDtoJsonMapper.toRedundancy;
import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;

public class EndpointDtoJsonSerializer extends BaseDtoJsonSerializer<EndpointDto> {
    public EndpointDtoJsonSerializer() {
        super(EndpointDto.class);
    }

    @Override
    protected EndpointJson translate(EndpointDto dto) {
        EndpointJson endpointJson = new EndpointJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        endpointJson.oDataId = oDataId;
        endpointJson.oDataContext = getContextFromId(oDataId);
        endpointJson.id = dto.getId();
        endpointJson.name = dto.getName();
        endpointJson.description = dto.getDescription();
        endpointJson.status = dto.getStatus();
        endpointJson.endpointProtocol = dto.getEndpointProtocol();
        endpointJson.pciId = mapToPciId(dto.getPciId());
        endpointJson.identifiers = mapToIdentifiers(dto.getIdentifiers());
        endpointJson.connectedEntities = mapToConnectedEntities(dto.getConnectedEntities());
        endpointJson.hostReservationMemoryBytes = dto.getHostReservationMemoryBytes();
        endpointJson.links.ports.addAll(asOdataIdSet(dto.getLinks().getPorts()));

        endpointJson.redundancy.addAll(
            dto.getRedundancies()
                .stream()
                .map(toRedundancy())
                .collect(toSet())
        );
        return endpointJson;
    }

    private Set<IdentifierJson> mapToIdentifiers(Set<IdentifierDto> identifiers) {
        return identifiers != null ? identifiers.stream()
            .map(identifier -> new IdentifierJson(identifier.getDurableName(),
                identifier.getDurableNameFormat())).collect(toSet()) : emptySet();
    }

    private PciIdJson mapToPciId(PciIdDto pciIdDto) {
        return pciIdDto != null ? new PciIdJson(pciIdDto.getVendorId(),
            pciIdDto.getDeviceId(),
            pciIdDto.getSubsystemId(),
            pciIdDto.getSubsystemVendorId()) : null;
    }

    private Set<ConnectedEntityJson> mapToConnectedEntities(Set<ConnectedEntityDto> connectedEntities) {
        return connectedEntities != null ? connectedEntities.stream()
            .map(this::createConnectEntity)
            .collect(toSet()) : emptySet();
    }

    private ConnectedEntityJson createConnectEntity(ConnectedEntityDto dto) {
        ConnectedEntityJson json = new ConnectedEntityJson();
        json.entityType = dto.getEntityType();
        json.entityRole = dto.getEntityRole();
        json.pciClassCode = dto.getPciClassCode();
        json.entityPciId = mapToPciId(dto.getEntityPciId());
        json.identifiers = mapToIdentifiers(dto.getIdentifiers());
        json.entityLink = asOdataId(dto.getEntityLink());
        json.pciFunctionNumber = dto.getPciFunctionNumber();

        return json;
    }
}
