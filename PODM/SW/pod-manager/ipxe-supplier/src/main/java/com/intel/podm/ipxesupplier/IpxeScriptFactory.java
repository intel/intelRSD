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

package com.intel.podm.ipxesupplier;

import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.RemoteTargetIscsiAddress;

import javax.enterprise.context.Dependent;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.utils.IterableHelper.single;

@Dependent
public class IpxeScriptFactory {
    public IpxeScript create(ComputerSystem computerSystem) throws AssetNotFoundException {
        if (computerSystem.getMetadata().isInAnyOfStates(RUNNING)) {
            return new DeepDiscoveryIpxeScript();
        } else {
            return remoteTargetIpxe(computerSystem);
        }
    }

    private RemoteTargetIpxeScript remoteTargetIpxe(ComputerSystem computerSystem) throws AssetNotFoundException {
        ComposedNode composedNode = findComposedNode(computerSystem);
        RemoteTarget remoteTarget = findRemoteTarget(composedNode);

        String iscsiInitiatorIqn = remoteTarget.getIscsiInitiatorIqn();
        RemoteTargetIscsiAddress address = findAddress(remoteTarget);
        Integer lun = findLun(address);

        return RemoteTargetIpxeScript
                .newBuilder()
                .initiatorIqn(iscsiInitiatorIqn)
                .serverName(address.getTargetPortalIp())
                .port(address.getTargetPortalPort())
                .lun(lun)
                .targetName(address.getTargetIqn())
                .build();
    }

    private ComposedNode findComposedNode(ComputerSystem computerSystem) throws AssetNotFoundException {
        if (computerSystem.getComposedNode() == null) {
            throw new AssetNotFoundException("Composed Node not found");
        }

        return computerSystem.getComposedNode();
    }

    private RemoteTarget findRemoteTarget(ComposedNode composedNode) throws AssetNotFoundException {
        Collection<RemoteTarget> remoteTargets = composedNode.getRemoteTargets();

        if (remoteTargets.size() != 1) {
            throw new AssetNotFoundException("Invalid number of remote targets in composed node (expected: 1, found: " + remoteTargets.size() + ")");
        }

        return single(remoteTargets);
    }

    private RemoteTargetIscsiAddress findAddress(RemoteTarget remoteTarget) throws AssetNotFoundException {
        Collection<RemoteTargetIscsiAddress> addresses = remoteTarget.getRemoteTargetIscsiAddresses();

        if (addresses.size() != 1) {
            throw new AssetNotFoundException("Invalid number of addresses in remote target (expected: 1, found: " + addresses.size() + ")");
        }

        return single(addresses);
    }

    private Integer findLun(RemoteTargetIscsiAddress address) throws AssetNotFoundException {
        List<Integer> luns = address.getTargetLuns();

        if (luns.size() != 1) {
            throw new AssetNotFoundException("Invalid number of LUNs in remote target (expected: 1, found: " + luns.size() + ")");
        }

        return single(luns);
    }
}
