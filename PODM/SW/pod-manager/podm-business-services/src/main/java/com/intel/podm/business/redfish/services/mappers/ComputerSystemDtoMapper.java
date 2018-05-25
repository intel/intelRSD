/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.ComputerSystemDto;
import com.intel.podm.business.entities.dao.PciePortDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.embeddables.ComputerSystemDevice;
import com.intel.podm.business.entities.redfish.embeddables.MemorySummary;
import com.intel.podm.business.entities.redfish.embeddables.ProcessorSummary;
import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEndpointContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPcieDeviceContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPcieDeviceFunctionContexts;
import static com.intel.podm.business.redfish.ContextCollections.asStorageServicesContexts;
import static com.intel.podm.mappers.Conditions.aggregateCondition;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Dependent
class ComputerSystemDtoMapper extends DtoMapper<ComputerSystem, ComputerSystemDto> {
    @Inject
    private PciePortDao pciePortDao;

    @Inject
    private BootDtoMapper bootDtoMapper;

    ComputerSystemDtoMapper() {
        super(ComputerSystem.class, ComputerSystemDto.class);
        this.ignoredProperties("boot", "pcieDevices", "pcieDeviceFunctions", "computerSystemMetrics",
            "links", "actions", "oem", "status", "trustedModules", "hostingRoles");
    }

    @Override
    protected void performNotAutomatedMapping(ComputerSystem source, ComputerSystemDto target) {
        super.performNotAutomatedMapping(source, target);

        if (!source.isComplementary()) {
            target.setPcieDeviceFunctions(asPcieDeviceFunctionContexts(source.getPcieDeviceFunctions()));
            target.setPcieDevices(asPcieDeviceContexts(source.getPcieDevices()));
            mapLinks(source, target);
            mapHostedServices(source, target);
            target.getActions().getReset().setAllowableResetTypes(source.getAllowableResetTypes());
            target.getActions().getOem().getChangeTpmState().setAllowableInterfaceTypes(source.getAllowableInterfaceTypes());
            mapTrustedModules(source, target);
            mapStatus(source, target);
        }

        bootDtoMapper.setMappingConditions(aggregateCondition(true, false));
        bootDtoMapper.map(source.getBoot(), target.getBoot());
        mapHostingRoles(source, target);
        mapRackScaleOem(source, target);
    }

    private void mapHostedServices(ComputerSystem source, ComputerSystemDto target) {
        ComputerSystemDto.HostedServices hostedServices = target.getHostedServices();
        hostedServices.setStorageServices(asStorageServicesContexts(source.getStorageServices()));
    }

    private void mapHostingRoles(ComputerSystem source, ComputerSystemDto target) {
        ofNullable(source.getHostingRoles()).ifPresent(target::setHostingRoles);
    }

    private void mapStatus(ComputerSystem source, ComputerSystemDto target) {
        ofNullable(source.getStatus()).ifPresent(target::setStatus);
        ofNullable(source.getMemorySummary()).map(MemorySummary::getStatus).ifPresent(status -> target.getMemorySummary().setStatus(status));
        ofNullable(source.getProcessorSummary()).map(ProcessorSummary::getStatus).ifPresent(status -> target.getProcessorSummary().setStatus(status));
    }

    private void mapTrustedModules(ComputerSystem source, ComputerSystemDto target) {
        ofNullable(source.getTrustedModules())
            .ifPresent(trustedModules ->
                target.setTrustedModules(
                    trustedModules.stream()
                        .map(tm -> createTrustedModuleDto(tm))
                        .collect(toList())
                )
            );
    }

    private ComputerSystemDto.TrustedModuleDto createTrustedModuleDto(TrustedModule source) {
        ComputerSystemDto.TrustedModuleDto trustedModuleDto = new ComputerSystemDto.TrustedModuleDto();
        mapTrustedModule(source, trustedModuleDto);

        return trustedModuleDto;
    }

    private void mapTrustedModule(TrustedModule source, ComputerSystemDto.TrustedModuleDto trustedModuleDto) {
        trustedModuleDto.setFirmwareVersion(source.getFirmwareVersion());
        trustedModuleDto.setFirmwareVersion2(source.getFirmwareVersion2());
        trustedModuleDto.setInterfaceType(source.getInterfaceType());
        trustedModuleDto.setInterfaceTypeSelection(source.getInterfaceTypeSelection());
        trustedModuleDto.setStatus(source.getStatus());
    }

    private void mapLinks(ComputerSystem source, ComputerSystemDto target) {
        ComputerSystemDto.Links links = target.getLinks();
        links.setChassis(asChassisContexts(source.getChassis()));
        links.setManagedBy(asManagerContexts(source.getManagers()));
        links.setEndpoints(findAttachedEndpoints(source));
    }

    private Set<Context> findAttachedEndpoints(ComputerSystem computerSystem) {
        Set<Endpoint> endpoints = pciePortDao.getUpstreamPortsByCableIds(computerSystem.getPcieConnectionIds()).stream()
            .map(Port::getEndpoints)
            .flatMap(Collection::stream)
            .collect(toSet());

        endpoints.addAll(computerSystem.getEndpoints());

        return asEndpointContexts(endpoints);
    }

    private void mapRackScaleOem(ComputerSystem source, ComputerSystemDto target) {
        ComputerSystemDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setDiscoveryState(source.getDiscoveryState());
        rackScaleOem.setMemorySockets(source.getMemorySockets());
        rackScaleOem.setProcessorSockets(source.getProcessorSockets());
        rackScaleOem.setPciDevices(asDevices(source.getPciDevices()));
        rackScaleOem.setPcieConnectionId(source.getPcieConnectionIds());
        rackScaleOem.setUserModeEnabled(source.getUserModeEnabled());
        rackScaleOem.setTrustedExecutionTechnologyEnabled(source.getTrustedExecutionTechnologyEnabled());
    }

    private List<ComputerSystemDto.OemDeviceDto> asDevices(List<ComputerSystemDevice> pciDevices) {
        return pciDevices.stream().map(device -> {
            ComputerSystemDto.OemDeviceDto dto = new ComputerSystemDto.OemDeviceDto();
            dto.setDeviceId(device.getDeviceId());
            dto.setVendorId(device.getVendorId());
            return dto;
        }).collect(toList());
    }
}
