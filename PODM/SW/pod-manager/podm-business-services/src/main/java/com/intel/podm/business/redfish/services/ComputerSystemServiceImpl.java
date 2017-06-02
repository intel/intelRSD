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
import com.intel.podm.business.dto.redfish.ComputerSystemDto;
import com.intel.podm.business.dto.redfish.attributes.ComputerSystemDeviceDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.dao.PciePortDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.embeddables.ComputerSystemDevice;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.BootDtoHelper;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SYSTEMS;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEndpointContexts;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPcieDeviceContexts;
import static com.intel.podm.business.redfish.ContextCollections.asPcieDeviceFunctionContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ClassFanOutComplexity"})
@RequestScoped
public class ComputerSystemServiceImpl implements ReaderService<ComputerSystemDto> {
    @Inject
    private GenericDao genericDao;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private BootDtoHelper bootDtoHelper;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Inject
    private PciePortDao pciePortDao;

    @Override
    @Transactional(REQUIRES_NEW)
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        Collection<ComputerSystem> computerSystemCollection = genericDao.findAll(ComputerSystem.class);
        return new CollectionDto(SYSTEMS, getAsIdSet(computerSystemCollection));
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public ComputerSystemDto getResource(Context system) throws ContextResolvingException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(system);

        return ComputerSystemDto
            .newBuilder()
            .id(computerSystem.getId().toString())
            .name(computerSystem.getName())
            .description(computerSystem.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(computerSystem.getService(), computerSystem.getUnknownOems()))
            .assetTag(computerSystem.getAssetTag())
            .biosVersion(computerSystem.getBiosVersion())
            .sku(computerSystem.getSku())
            .hostName(computerSystem.getHostName())
            .indicatorLed(computerSystem.getIndicatorLed())
            .model(computerSystem.getModel())
            .memorySockets(computerSystem.getMemorySockets())
            .memoryStatus(computerSystem.getMemoryStatus())
            .totalSystemMemoryGiB(computerSystem.getTotalSystemMemoryGiB())
            .uuid(computerSystem.getUuid())
            .manufacturer(computerSystem.getManufacturer())
            .processorsCount(computerSystem.getProcessorsCount())
            .processorsModel(computerSystem.getProcessorModel())
            .processorSockets(computerSystem.getProcessorSockets())
            .processorsStatus(computerSystem.getProcessorStatus())
            .chassis(asChassisContexts(computerSystem.getChassis()))
            .boot(bootDtoHelper.buildBootDto(computerSystem.getBoot()))
            .discoveryState(computerSystem.getDiscoveryState())
            .status(computerSystem.getStatus())
            .systemType(computerSystem.getSystemType())
            .powerState(computerSystem.getPowerState())
            .serialNumber(computerSystem.getSerialNumber())
            .partNumber(computerSystem.getPartNumber())
            .pciDevices(buildDevices(computerSystem.getPciDevices()))
            .pcieDevices(asPcieDeviceContexts(computerSystem.getPcieDevices()))
            .endpoints(attachEndpoints(computerSystem))
            .pcieDeviceFunctions(
                asPcieDeviceFunctionContexts(computerSystem.getPcieDeviceFunctions()))
            .allowableResetTypes(computerSystem.getAllowableResetTypes())
            .managedBy(asManagerContexts(computerSystem.getManagers()))
            .pcieConnectionId(computerSystem.getPcieConnectionId())
            .build();
    }

    private Set<Context> attachEndpoints(ComputerSystem computerSystem) {
        Set<Endpoint> endpoints = pciePortDao.getUpstreamPortsByCableIds(computerSystem.getPcieConnectionId()).stream()
            .map(Port::getEndpoints)
            .flatMap(Collection::stream)
            .collect(toSet());
        return asEndpointContexts(endpoints);
    }

    private Set<RedfishComputerSystem.Device> buildDevices(Collection<ComputerSystemDevice> devices) {
        return devices.stream()
            .map(device -> new ComputerSystemDeviceDto(device.getVendorId(), device.getDeviceId()))
            .collect(toSet());
    }
}
