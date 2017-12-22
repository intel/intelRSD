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

package com.intel.podm.business.redfish.services.assembly;

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
import java.util.Optional;
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

    public List<EthernetSwitchPortVlan> taggedVlansToDelete(EthernetSwitchPort switchPort,
                                                            List<RequestedNode.EthernetInterface.Vlan> vlansToPreserve) {
        List<EthernetSwitchPortVlan> taggedVlansToDelete = new ArrayList<>(switchPort.getEthernetSwitchPortVlans());

        List<Integer> reservedVlans = (List<Integer>) allocationConfigHolder.get().getReservedVlanIds();

        taggedVlansToDelete.removeIf(vlan -> !vlan.getTagged());
        taggedVlansToDelete.removeIf(vlan -> vlansToPreserve.stream().anyMatch(sameVlanPredicate(vlan)));
        taggedVlansToDelete.removeIf(vlan -> reservedVlans.stream().anyMatch(vlanId -> Objects.equals(vlan.getVlanId(), vlanId)));

        return taggedVlansToDelete;
    }

    public List<EthernetSwitchPortVlan> untaggedVlansToDelete(EthernetSwitchPort switchPort,
                                                              RequestedNode.EthernetInterface.Vlan untaggedVlanToPreserve) {
        List<EthernetSwitchPortVlan> untaggedVlansToDelete = new ArrayList<>(switchPort.getEthernetSwitchPortVlans());
        untaggedVlansToDelete.removeIf(EthernetSwitchPortVlan::getTagged);
        if (untaggedVlanToPreserve != null) {
            untaggedVlansToDelete.removeIf(vlan -> Objects.equals(vlan.getVlanId(), untaggedVlanToPreserve.getVlanId()));
        }

        List<Integer> reservedVlans = (List<Integer>) allocationConfigHolder.get().getReservedVlanIds();
        untaggedVlansToDelete.removeIf(vlan -> reservedVlans.stream().anyMatch(vlanId -> Objects.equals(vlan.getVlanId(), vlanId)));
        return untaggedVlansToDelete;
    }

    public Optional<EthernetSwitchPortVlan> untaggedVlanToChange(EthernetSwitchPort associatedSwitchPort) {
        List<EthernetSwitchPortVlan> vlansToChange = new ArrayList<>(associatedSwitchPort.getEthernetSwitchPortVlans());
        vlansToChange.removeIf(EthernetSwitchPortVlan::getTagged);
        return vlansToChange.stream().findFirst();
    }

    private Predicate<RequestedNode.EthernetInterface.Vlan> sameVlanPredicate(EthernetSwitchPortVlan vlanRequest) {
        return vlan ->
            Objects.equals(vlan.isTagged(), vlanRequest.getTagged())
                && Objects.equals(vlan.getVlanId(), vlanRequest.getVlanId());
    }
}

