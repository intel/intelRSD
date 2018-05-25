/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.allocation.mappers.ethernetinterface.EthernetInterfacesAllocationMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive.MasterDrive;
import com.intel.podm.common.types.Protocol;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.types.Protocol.ROCE;
import static com.intel.podm.common.types.Protocol.ROCEV2;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.Arrays.asList;
import static org.apache.commons.collections4.CollectionUtils.isEmpty;

@Dependent
public class EthernetInterfaceMatcher {
    private static final List<Protocol> RDMA_PROTOCOLS = asList(ROCE, ROCEV2);

    @Inject
    EthernetInterfacesAllocationMapper ethernetInterfacesAllocationMapper;

    @Inject
    EntityTreeTraverser traverser;

    @Inject
    FabricProtocolHelper protocolHelper;

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

        Context resourceContext = master != null ? master.getResourceContext() : remoteDrive.getResourceContext();

        if (resourceContext == null) {
            return NVME_OVER_FABRICS.equals(remoteDrive.getProtocol());
        }

        try {
            Entity resourceEntity = traverser.traverse(resourceContext);

            return protocolHelper.getProtocolsFromFabricEntity(resourceEntity).contains(NVME_OVER_FABRICS);
        } catch (ContextResolvingException e) {
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
