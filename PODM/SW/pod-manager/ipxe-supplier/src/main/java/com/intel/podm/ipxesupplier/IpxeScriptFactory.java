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

package com.intel.podm.ipxesupplier;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.IpTransportDetails;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;

import javax.enterprise.context.ApplicationScoped;
import java.util.Optional;

import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.types.DurableNameFormat.LUN;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.utils.Collections.firstByPredicate;

@ApplicationScoped
public class IpxeScriptFactory {
    public IpxeScript create(ComputerSystem computerSystem) throws AssetNotFoundException {
        if (computerSystem.getMetadata().isInAnyOfStates(RUNNING)) {
            return new DeepDiscoveryIpxeScript();
        } else {
            Optional<ConnectedEntity> connectedEntity = findConnectedEntityWithBootableVolume(computerSystem);

            Endpoint targetEndpoint = connectedEntity
                .orElseThrow(() -> new AssetNotFoundException("Connected entity with bootable volume not found in composed node"))
                .getEndpoint();

            Endpoint initiatorEndpoint = firstByPredicate(targetEndpoint.getZone().getEndpoints(), endpoint -> endpoint.hasRole(INITIATOR))
                .orElseThrow(() -> new AssetNotFoundException("Initiator endpoint not found"));

            return createSanbootIpxeScript(initiatorEndpoint, targetEndpoint, connectedEntity.get());
        }
    }

    private Optional<ConnectedEntity> findConnectedEntityWithBootableVolume(ComputerSystem computerSystem) throws AssetNotFoundException {
        ComposedNode composedNode = findComposedNode(computerSystem);
        return composedNode.findAnyConnectedEntityWithBootableVolume();
    }

    private ComposedNode findComposedNode(ComputerSystem computerSystem) throws AssetNotFoundException {
        if (computerSystem.getComposedNode() == null) {
            throw new AssetNotFoundException("Composed Node not found");
        }

        return computerSystem.getComposedNode();
    }

    private SanbootIpxeScript createSanbootIpxeScript(Endpoint initiatorEndpoint, Endpoint targetEndpoint, ConnectedEntity targetConnectedEntity)
        throws AssetNotFoundException {
        String initiatorIqn = getIqnFromEndpoint(initiatorEndpoint);
        String targetIqn = getIqnFromEndpoint(targetEndpoint);

        IpTransportDetails transportDetail = getIscsiTransportDetail(targetEndpoint);
        String targetPortalIp = getIpFromTransportDetail(transportDetail);
        Integer targetPortalPort = transportDetail.getPort();

        String lun = targetConnectedEntity.getIdentifiers().stream()
            .filter(ce -> LUN.equals(ce.getDurableNameFormat()))
            .map(Identifier::getDurableName)
            .findFirst()
            .orElseThrow(() -> new AssetNotFoundException("Connected entity Identifiers has not defined LUN"));

        return SanbootIpxeScript
            .newBuilder()
            .initiatorIqn(initiatorIqn)
            .serverName(targetPortalIp)
            .port(targetPortalPort)
            .lun(lun)
            .targetName(targetIqn)
            .build();
    }

    private String getIqnFromEndpoint(Endpoint endpoint) throws AssetNotFoundException {
        return endpoint.findIqnIdentifier()
            .orElseThrow(() -> new AssetNotFoundException("iQN identifier not found in endpoint"))
            .getDurableName();
    }

    private IpTransportDetails getIscsiTransportDetail(Endpoint endpoint) throws AssetNotFoundException {
        return endpoint.findIscsiTransport()
            .orElseThrow(() -> new AssetNotFoundException("IpTransportDetails with iSCSI transport not found in endpoint"));
    }

    private String getIpFromTransportDetail(IpTransportDetails transportDetail) throws AssetNotFoundException {
        return transportDetail.getIp().orElseThrow(() -> new AssetNotFoundException("IP address is not specified"));
    }
}
