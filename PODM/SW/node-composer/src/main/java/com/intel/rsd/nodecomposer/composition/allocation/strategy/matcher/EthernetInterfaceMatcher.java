/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive.MasterDrive;
import com.intel.rsd.nodecomposer.composition.allocation.mappers.ethernetinterface.EthernetInterfacesAllocationMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static com.intel.rsd.collections.IterableHelper.single;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.rsd.nodecomposer.types.Protocol.ROCE;
import static com.intel.rsd.nodecomposer.types.Protocol.ROCEV2;
import static java.util.Arrays.asList;
import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class EthernetInterfaceMatcher {
    private static final List<Protocol> RDMA_PROTOCOLS = asList(ROCE, ROCEV2);

    private final EthernetInterfacesAllocationMapper ethernetInterfacesAllocationMapper;
    private final EntityTreeTraverser traverser;
    private final FabricProtocolHelper protocolHelper;

    @Autowired
    public EthernetInterfaceMatcher(EthernetInterfacesAllocationMapper ethernetInterfacesAllocationMapper, EntityTreeTraverser traverser,
                                    FabricProtocolHelper protocolHelper) {
        this.ethernetInterfacesAllocationMapper = ethernetInterfacesAllocationMapper;
        this.traverser = traverser;
        this.protocolHelper = protocolHelper;
    }

    public boolean matches(RequestedNode requestedNode, Collection<EthernetInterface> availableInterfaces) {
        List<RequestedNode.EthernetInterface> requestedInterfaces = requestedNode.getEthernetInterfaces();
        boolean ethernetInterfacesAreNotSpecified = isEmpty(requestedInterfaces);

        return (ethernetInterfacesAreNotSpecified
            || canMapRequestedWithAvailableInterfaces(requestedInterfaces, availableInterfaces))
            && canUseAvailableInterfacesWithRequestedDrives(requestedNode, availableInterfaces);
    }

    private boolean canUseAvailableInterfacesWithRequestedDrives(RequestedNode requestedNode, Collection<EthernetInterface> availableInterfaces) {
        List<RemoteDrive> requestedRemoteDrives = requestedNode.getRemoteDrives();
        boolean remoteDrivesAreNotSpecified = isEmpty(requestedRemoteDrives);

        return remoteDrivesAreNotSpecified
            || !isRequestedDriveNvme(requestedRemoteDrives)
            || anyOfAvailableInterfacesSupportsRdma(availableInterfaces);
    }

    private boolean anyOfAvailableInterfacesSupportsRdma(Collection<EthernetInterface> availableInterfaces) {
        return availableInterfaces.stream()
            .map(EthernetInterface::getSupportedProtocols)
            .flatMap(Collection::stream)
            .anyMatch(RDMA_PROTOCOLS::contains);
    }

    private boolean isRequestedDriveNvme(List<RemoteDrive> requestedRemoteDrives) {
        RemoteDrive remoteDrive = single(requestedRemoteDrives);
        MasterDrive master = remoteDrive.getMaster();

        ODataId resourceODataId = master != null ? master.getResourceODataId() : remoteDrive.getResourceODataId();

        if (resourceODataId == null) {
            return NVME_OVER_FABRICS.equals(remoteDrive.getProtocol());
        }

        try {
            Entity resourceEntity = traverser.traverseDiscoverableEntity(resourceODataId);

            return protocolHelper.getProtocolsFromFabricEntity(resourceEntity).contains(NVME_OVER_FABRICS);
        } catch (ODataIdResolvingException e) {
            throw new IllegalStateException("Previously validated context is not valid.");
        }
    }

    private boolean canMapRequestedWithAvailableInterfaces(Collection<RequestedNode.EthernetInterface> requestedInterfaces,
                                                           Collection<EthernetInterface> availableInterfaces) {
        Map<EthernetInterface, RequestedNode.EthernetInterface> map
            = ethernetInterfacesAllocationMapper.map(requestedInterfaces, availableInterfaces);

        return Objects.equals(map.size(), requestedInterfaces.size());
    }
}
