/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.actions;

import com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ActionInfoDto;
import com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ParameterDto;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.ProcessorHelper;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.DriveDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.dao.ProcessorDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ParametersDtoBuilder.asProtocolParameterDto;
import static com.intel.rsd.nodecomposer.business.dto.actions.actioninfo.ParametersDtoBuilder.asResourceParameterDto;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static com.intel.rsd.nodecomposer.types.Protocol.FPGA_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.ATTACH_RESOURCE_ACTION_INFO;
import static java.util.Arrays.asList;
import static java.util.Collections.emptySet;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toSet;

@Component
@SuppressWarnings({"checkstyle:MethodCount"})
public class AttachResourceInfoService {
    private final DriveDao driveDao;
    private final GenericDao genericDao;
    private final VolumeHelper volumeHelper;
    private final ProcessorHelper processorHelper;
    private final ProcessorDao processorDao;

    @Autowired
    public AttachResourceInfoService(DriveDao driveDao, GenericDao genericDao, VolumeHelper volumeHelper,
                                     ProcessorDao processorDao, ProcessorHelper processorHelper) {
        this.driveDao = driveDao;
        this.genericDao = genericDao;
        this.volumeHelper = volumeHelper;
        this.processorDao = processorDao;
        this.processorHelper = processorHelper;
    }

    public ActionInfoDto getActionInfo(ComposedNode composedNode) {
        ActionInfoDto actionInfoDto = new ActionInfoDto();
        actionInfoDto.setId(ATTACH_RESOURCE_ACTION_INFO);
        actionInfoDto.setName("Attach Resource ActionInfo");
        actionInfoDto.addParameters(new ArrayList<>(getParameters(composedNode)));

        return actionInfoDto;
    }

    private Collection<? extends ParameterDto> getParameters(ComposedNode composedNode) {
        Set<Drive> drives = getDrivesAllowableToAttach(composedNode);
        Set<Volume> volumes = getVolumesAllowableToAttach();
        Set<Processor> processors = getProcessorsAllowableToAttach(composedNode);
        Set<Endpoint> endpointsAllowableToAttach = getEndpointsAllowableToAttach(drives, processors);

        Set<ODataId> oDataIdValues = new HashSet<>(getODataId(drives));
        oDataIdValues.addAll(getODataId(volumes));
        oDataIdValues.addAll(getODataId(endpointsAllowableToAttach));
        oDataIdValues.addAll(getODataId(processors));

        Set<Protocol> protocolValues = new HashSet<>(getProtocolForDrives(drives));
        protocolValues.addAll(getProtocolsForVolumes(volumes));
        protocolValues.addAll(getProtocolsForProcessor(processors));
        protocolValues.addAll(getProtocolsForEndpoints(endpointsAllowableToAttach));

        return asList(asResourceParameterDto(oDataIdValues), asProtocolParameterDto(protocolValues));
    }

    private Set<Processor> getProcessorsAllowableToAttach(ComposedNode composedNode) {
        Set<Processor> processors = new HashSet<>(getPcieProcessorsAllowableToAttach(composedNode));
        processors.addAll(processorDao.getAvailableFpgaOverFabricsProcessors());
        return processors;
    }

    private Collection<? extends Protocol> getProtocolsForProcessor(Set<Processor> processors) {
        return processors.stream().map(processorHelper::retrieveProtocolFromProcessor).collect(toSet());
    }

    private Set<Drive> getDrivesAllowableToAttach(ComposedNode composedNode) {
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem == null) {
            return emptySet();
        }
        return driveDao.getAchievablePcieDrives(computerSystem);
    }

    private Set<Processor> getPcieProcessorsAllowableToAttach(ComposedNode composedNode) {
        return ofNullable(composedNode.getComputerSystem()).map(processorDao::getAchievablePcieProcessors).orElse(emptySet());
    }

    private Set<Volume> getVolumesAllowableToAttach() {
        return genericDao.findAll(Volume.class).stream()
            .filter(volume -> volume.getStorageService() != null)
            .filter(excludeIscsiPortVolume())
            .filter(volume -> !volume.isAllocated())
            .filter(DiscoverableEntity::isAvailable)
            .collect(toSet());
    }

    private Set<Endpoint> getEndpointsAllowableToAttach(Set<Drive> drives, Set<Processor> processors) {
        Set<Endpoint> endpoints = getAllEndpointCandidates();
        Set<Endpoint> endpointsAllowableToAttach = new HashSet<>(getEndpointsByProtocol(NVME_OVER_FABRICS, endpoints));
        endpointsAllowableToAttach.addAll(getNvmeOverPcieEndpoints(endpoints, drives));
        endpointsAllowableToAttach.addAll(getEndpointsByProtocol(FPGA_OVER_FABRICS, endpoints));
        endpointsAllowableToAttach.addAll(getFpgaOverPcieEndpoints(endpoints, processors));

        return endpointsAllowableToAttach;
    }

    private Predicate<Volume> excludeIscsiPortVolume() {
        return volume -> !ISCSI.equals(volumeHelper.retrieveProtocolFromVolume(volume));
    }

    private Set<Endpoint> getEndpointsByProtocol(Protocol protocol, Set<Endpoint> endpoints) {
        return endpoints.stream()
            .filter(endpoint -> protocol.equals(endpoint.getProtocol()))
            .collect(toSet());
    }

    private Set<ODataId> getODataId(Set<? extends DiscoverableEntity> discoverableEntities) {
        return discoverableEntities.stream().map(DiscoverableEntity::getUri)
            .collect(toSet());
    }

    private Set<Protocol> getProtocolForDrives(Set<Drive> drives) {
        return drives.stream().map(Drive::getProtocol).collect(toSet());
    }

    private Set<Protocol> getProtocolsForVolumes(Set<Volume> volumes) {
        return volumes.stream().map(volumeHelper::retrieveProtocolFromVolume).collect(toSet());
    }

    private Set<Protocol> getProtocolsForEndpoints(Set<Endpoint> endpoints) {
        return endpoints.stream().map(Endpoint::getProtocol).collect(toSet());
    }

    private Set<Endpoint> getAllEndpointCandidates() {
        return genericDao.findAll(Endpoint.class).stream()
            .filter(excludeIscsiPortEndpoint())
            .filter(Endpoint::isAttachable)
            .filter(DiscoverableEntity::isAvailable)
            .filter(endpoint -> endpoint.hasRole(TARGET))
            .collect(toSet());
    }

    private Set<Endpoint> getFpgaOverPcieEndpoints(Set<Endpoint> endpoints, Set<Processor> processors) {
        return getEndpointsByProtocol(PCIE, endpoints).stream()
            .filter(endpoint -> isEndpointWithAchievableProcessors(endpoint, processors))
            .collect(toSet());
    }

    private boolean isEndpointWithAchievableProcessors(Endpoint endpoint, Set<Processor> processors) {
        return endpoint.getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityLink)
            .filter(Processor.class::isInstance)
            .map(Processor.class::cast)
            .anyMatch(processors::contains);
    }

    private Set<Endpoint> getNvmeOverPcieEndpoints(Set<Endpoint> endpoints, Set<Drive> drives) {
        return endpoints.stream()
            .filter(getEndpointsWithAchievableDrives(drives))
            .collect(toSet());
    }

    private Predicate<Endpoint> getEndpointsWithAchievableDrives(Set<Drive> achievableDrives) {

        return endpoint -> asList(PCIE, NVME).contains(endpoint.getProtocol()) && endpointContainsAchievablePcieDrive(endpoint, achievableDrives);
    }

    private boolean endpointContainsAchievablePcieDrive(Endpoint endpoint, Set<Drive> achievableDrives) {

        return endpoint.getConnectedEntities().stream()
            .map(ConnectedEntity::getEntityLink)
            .filter(Drive.class::isInstance)
            .map(Drive.class::cast)
            .anyMatch(achievableDrives::contains);
    }

    private Predicate<Endpoint> excludeIscsiPortEndpoint() {
        return endpoint -> !ISCSI.equals(endpoint.getProtocol());

    }
}
