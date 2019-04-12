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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPortVlan;
import com.intel.rsd.nodecomposer.types.actions.VlanCreationRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static java.lang.Boolean.TRUE;
import static java.util.stream.Collectors.toList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class VlanSelector {
    private final List<Integer> reservedVlanIds;

    @Autowired
    public VlanSelector(@Value("${allocation.reserved-vlan-ids:1,170,4088,4091,4094}") List<Integer> reservedVlanIds) {
        this.reservedVlanIds = reservedVlanIds;
    }

    public List<VlanCreationRequest> getVlansToCreate(Collection<EthernetSwitchPortVlan> existingVlans, List<Vlan> requestedVlans) {
        return requestedVlans.stream()
            .filter(requestedVlan -> existingVlans.stream().noneMatch(vlan -> isTheSameVlan(requestedVlan, vlan)))
            .map(requestedVlan -> new VlanCreationRequest(requestedVlan.getVlanId(), requestedVlan.isTagged(), requestedVlan.isEnabled()))
            .collect(toList());
    }

    public List<EthernetSwitchPortVlan> getTaggedVlansToDelete(EthernetSwitchPort switchPort, List<Vlan> vlansToPreserve) {
        return switchPort.getEthernetSwitchPortVlans().stream()
            .filter(vlan -> Objects.equals(vlan.getTagged(), TRUE))
            .filter(vlan -> vlansToPreserve.stream().noneMatch(preservedVlan -> isTheSameVlan(preservedVlan, vlan)))
            .filter(vlan -> reservedVlanIds.stream().noneMatch(vlanId -> Objects.equals(vlan.getVlanId(), vlanId)))
            .collect(toList());
    }

    public List<EthernetSwitchPortVlan> getUntaggedVlansToDelete(EthernetSwitchPort switchPort, Vlan untaggedVlanToPreserve) {
        return switchPort.getEthernetSwitchPortVlans().stream()
            .filter(vlan -> !Objects.equals(vlan.getTagged(), TRUE))
            .filter(vlan -> untaggedVlanToPreserve == null || !Objects.equals(vlan.getVlanId(), untaggedVlanToPreserve.getVlanId()))
            .filter(vlan -> reservedVlanIds.stream().noneMatch(vlanId -> Objects.equals(vlan.getVlanId(), vlanId)))
            .collect(toList());
    }

    public Optional<EthernetSwitchPortVlan> tryGetUntaggedVlanToChange(EthernetSwitchPort associatedSwitchPort) {
        return associatedSwitchPort.getEthernetSwitchPortVlans().stream()
            .filter(vlan -> !Objects.equals(vlan.getTagged(), TRUE))
            .findFirst();
    }

    private boolean isTheSameVlan(Vlan vlan, EthernetSwitchPortVlan portVlan) {
        return Objects.equals(vlan.getVlanId(), portVlan.getVlanId()) && Objects.equals(vlan.isTagged(), portVlan.getTagged());
    }
}

