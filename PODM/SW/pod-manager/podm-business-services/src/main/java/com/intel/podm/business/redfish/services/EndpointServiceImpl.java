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
import com.intel.podm.business.dto.redfish.EndpointDto;
import com.intel.podm.business.dto.redfish.RedundancyDto;
import com.intel.podm.business.dto.redfish.attributes.ConnectedEntityDto;
import com.intel.podm.business.dto.redfish.attributes.IdentifierDto;
import com.intel.podm.business.dto.redfish.attributes.PciIdDto;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.embeddables.PciId;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.EntityRole;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Objects;
import java.util.Set;
import java.util.stream.Collectors;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ENDPOINTS;
import static com.intel.podm.business.redfish.ContextCollections.asPortContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ClassFanOutComplexity"})
@Transactional(REQUIRED)
public class EndpointServiceImpl implements ReaderService<EndpointDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context fabricContext) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(fabricContext);
        return new CollectionDto(ENDPOINTS, getAsIdSet(fabric.getEndpoints()));
    }

    @Override
    public EndpointDto getResource(Context context) throws ContextResolvingException {
        Endpoint endpoint = (Endpoint) traverser.traverse(context);

        return EndpointDto
            .newBuilder()
            .id(endpoint.getId().toString())
            .name(endpoint.getName())
            .description(endpoint.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(endpoint.getService(), endpoint.getUnknownOems()))
            .status(endpoint.getStatus())
            .endpointProtocol(endpoint.getProtocol())
            .ports(asPortContexts(endpoint.getPorts()))
            .pciId(toPciIdDto(endpoint.getPciId()))
            .identifiers(getIdentifiers(endpoint))
            .connectedEntities(getConnectedEntities(endpoint))
            .redundancies(getRedundancies(endpoint))
            .hostReservationMemoryBytes(endpoint.getHostReservationMemoryBytes())
            .build();
    }

    private Set<IdentifierDto> getIdentifiers(Endpoint endpoint) {
        if (endpoint.getIdentifiers() == null) {
            return null;
        }
        Set<IdentifierDto> identifierDtos = endpoint.getIdentifiers().stream()
            .map(identifier -> IdentifierDto.newBuilder()
                .durableName(identifier.getDurableName())
                .durableNameFormat(identifier.getDurableNameFormat())
                .build())
            .collect(Collectors.toSet());

        return identifierDtos;
    }

    private Set<RedundancyDto> getRedundancies(Endpoint endpoint) {
        if (endpoint.getRedundancies() == null) {
            return null;
        }
        return endpoint.getRedundancies().stream()
            .map(redundancy -> RedundancyDto.newRedundancyDto()
                .context(toContext(redundancy))
                .memberId(redundancy.getMemberId())
                .name(redundancy.getName())
                .status(redundancy.getStatus())
                .maxNumSupported(redundancy.getMaxNumSupported())
                .minNumNeeded(redundancy.getMinNumNeeded())
                .mode(redundancy.getMode())
                .redundancyEnabled(redundancy.getRedundancyEnabled())
                .redundancySet(redundancy.getRedundancyMembers().stream().map(Contexts::toContext).collect(toSet()))
                .build())
            .collect(Collectors.toSet());
    }

    private Set<ConnectedEntityDto> getConnectedEntities(Endpoint endpoint) {
        if (endpoint.getConnectedEntities() == null) {
            return null;
        }
        return endpoint.getConnectedEntities().stream()
            .map(connectedEntity -> ConnectedEntityDto.newBuilder()
                .entityType(connectedEntity.getEntityType())
                .entityRole(connectedEntity.getEntityRole())
                .pciFunctionNumber(connectedEntity.getPciFunctionNumber())
                .pciClassCode(connectedEntity.getPciClassCode())
                .entityPciId(toPciIdDto(connectedEntity.getPciId()))
                .identifiers(getIdentifiers(connectedEntity))
                .entityLink(getEntityLink(connectedEntity))
                .build())
            .collect(Collectors.toSet());
    }

    private Context getEntityLink(ConnectedEntity connectedEntity) {
        if (isInitiator(connectedEntity) && (connectedEntity.getEntityLink() == null)) {
            return attachComputerSystem(connectedEntity);
        }
        return toContext(connectedEntity.getEntityLink());
    }

    private boolean isInitiator(ConnectedEntity connectedEntity) {
        return connectedEntity.getEntityRole() != EntityRole.TARGET;
    }

    private Context attachComputerSystem(ConnectedEntity connectedEntity) {
        return connectedEntity.getEndpoint().getPorts().stream()
            .map(Port::getPcieConnectionIds)
            .flatMap(Collection::stream)
            .map(connectionId -> {
                try {
                    return computerSystemDao.getComputerSystemByPcieConnectionId(connectionId);
                } catch (NonUniqueResultException e) {
                    return null;
                }
            })
            .filter(Objects::nonNull)
            .map(Contexts::toContext)
            .findFirst()
            .orElse(null);
    }

    private Set<IdentifierDto> getIdentifiers(ConnectedEntity connectedEntity) {
        if (connectedEntity.getIdentifiers() == null) {
            return null;
        }

        Set<IdentifierDto> identifierDtos = connectedEntity.getIdentifiers().stream()
            .map(identifier -> IdentifierDto.newBuilder()
                .durableName(identifier.getDurableName())
                .durableNameFormat(identifier.getDurableNameFormat())
                .build())
            .collect(Collectors.toSet());

        return identifierDtos;
    }

    private PciIdDto toPciIdDto(PciId pciId) {
        if (pciId == null) {
            return null;
        }

        return PciIdDto.newBuilder()
            .deviceId(pciId.getDeviceId())
            .vendorId(pciId.getVendorId())
            .subsystemId(pciId.getSubsystemId())
            .subsystemVendorId(pciId.getSubsystemVendorId())
            .build();
    }
}
