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

package com.intel.podm.assembly;

import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.actions.VlanCreationRequest;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.AllocationConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;

import static java.util.stream.Collectors.toList;

@Dependent
public class VlanSelector {
    @Inject
    @Config
    private Holder<AllocationConfig> allocationConfigHolder;

    public List<VlanCreationRequest> vlansToCreate(Collection<EthernetSwitchPortVlan> switchPortVlans,
                                                   List<RequestedNode.EthernetInterface.Vlan> requestedVlans) {
        List<RequestedNode.EthernetInterface.Vlan> vlans = new ArrayList<>(requestedVlans);
        vlans.removeIf(vlan -> switchPortVlans.stream().anyMatch(sameVlanPredicate(vlan)));

        return vlans.stream()
                .map(vlan -> new VlanCreationRequest(vlan.getVlanId(), vlan.isTagged(), vlan.isEnabled()))
                .collect(toList());
    }

    private Predicate<EthernetSwitchPortVlan> sameVlanPredicate(RequestedNode.EthernetInterface.Vlan vlan) {
        return v -> Objects.equals(v.getVlanId(), vlan.getVlanId()) && Objects.equals(v.getTagged(), vlan.isTagged());
    }

    public List<EthernetSwitchPortVlan> vlansToDelete(EthernetSwitchPort switchPort,
                                                      List<RequestedNode.EthernetInterface.Vlan> requestedVlans) {
        List<EthernetSwitchPortVlan> vlansToDelete = new ArrayList<>(switchPort.getEthernetSwitchPortVlans());

        List<Integer> reservedVlans = (List<Integer>) allocationConfigHolder.get().getReservedVlanIds();

        vlansToDelete.removeIf(vlan -> requestedVlans.stream().anyMatch(sameVlanPredicate(vlan)));
        vlansToDelete.removeIf(vlan -> reservedVlans.stream().anyMatch(vlanId -> Objects.equals(vlan.getVlanId(), vlanId)));

        return vlansToDelete;
    }

    private Predicate<RequestedNode.EthernetInterface.Vlan> sameVlanPredicate(EthernetSwitchPortVlan vlanRequest) {
        return vlan ->
                Objects.equals(vlan.isTagged(), vlanRequest.getTagged())
                        && Objects.equals(vlan.getVlanId(), vlanRequest.getVlanId());
    }
}

