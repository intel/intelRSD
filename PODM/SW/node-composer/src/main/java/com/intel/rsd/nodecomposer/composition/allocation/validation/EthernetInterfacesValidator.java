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

package com.intel.rsd.nodecomposer.composition.allocation.validation;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.EthernetInterface.Vlan;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.utils.StringRepresentation.fromIterable;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class EthernetInterfacesValidator {

    private final Integer minVlanId;
    private final Integer maxVlanId;
    private final List<Integer> reservedVlanIds;

    @Autowired
    public EthernetInterfacesValidator(@Value("${allocation.reserved-vlan-ids:170,4088,4091,4094}") List<Integer> reservedVlanIds,
                                       @Value("${allocation.min-vlan-id:1}") Integer minVlanId,
                                       @Value("${allocation.max-vlan-id:4094}") Integer maxVlanId) {
        this.reservedVlanIds = reservedVlanIds;
        this.minVlanId = minVlanId;
        this.maxVlanId = maxVlanId;
    }

    public Violations validate(List<RequestedNode.EthernetInterface> ethernetInterfaces) {
        Violations violations = new Violations();

        List<Optional<String>> possibleViolations = new ArrayList<>();

        for (RequestedNode.EthernetInterface ethernetInterface : ethernetInterfaces) {
            Optional<List<Vlan>> vlans = ethernetInterface.getVlans();

            if (!vlans.isPresent()) {
                continue;
            }

            List<Vlan> vlanList = vlans.get();
            possibleViolations.add(validateUniquenessOfVlans(vlanList));
            possibleViolations.add(validateUntaggedVlans(vlanList));
            possibleViolations.add(validateUsageOfAllowedVlans(vlanList));
            possibleViolations.add(validatePrivateVlanRequestConsistency(ethernetInterface.getPrimaryVlan(), vlanList));
            possibleViolations.add(validateVlansRange(vlanList));
        }

        possibleViolations.forEach(possibleViolation -> possibleViolation.ifPresent(violations::addViolation));

        return violations;
    }

    private Optional<String> validateUntaggedVlans(List<Vlan> vlans) {
        List<Vlan> untaggedVlans = vlans.stream().filter(vlan -> !vlan.isTagged()).collect(toList());
        if (untaggedVlans.size() > 1) {
            return of("You can request maximum one untagged VLAN.");
        }
        return empty();
    }

    private Optional<String> validatePrivateVlanRequestConsistency(Integer primaryVlanId, List<Vlan> vlans) {
        Optional<Integer> primaryVlanOption = ofNullable(primaryVlanId);
        if (!primaryVlanOption.isPresent()) {
            return empty();
        }

        boolean primaryVlanNotDefined = vlans.stream().noneMatch(vlan -> Objects.equals(vlan.getVlanId(), primaryVlanOption.get()));
        if (primaryVlanNotDefined) {
            return of("Requested PrimaryVlan must also be enumerated on VLANs list.");
        }

        return empty();
    }

    private Optional<String> validateUsageOfAllowedVlans(List<Vlan> vlans) {
        for (Integer integer : reservedVlanIds) {
            if (vlans.stream().anyMatch(vlan -> vlan.getVlanId().equals(integer))) {
                String reservedVlansList = fromIterable(reservedVlanIds);
                return of("Requested vlan id appeared in reserved vlans list " + reservedVlansList);
            }
        }
        return empty();
    }

    private Optional<String> validateUniquenessOfVlans(List<Vlan> vlans) {
        long numberOfUniqueVlansIds = vlans.stream().map(Vlan::getVlanId).distinct().count();
        return numberOfUniqueVlansIds != vlans.size()
            ? of("There are repetitions in vlan collection")
            : empty();
    }

    private Optional<String> validateVlansRange(List<Vlan> vlans) {
        List<Vlan> outOfRangeVlans = vlans.stream()
            .filter(vlan -> vlan.getVlanId() <= minVlanId || vlan.getVlanId() >= maxVlanId)
            .collect(toList());
        if (outOfRangeVlans.size() > 0) {
            return of(String.format("VLANid is out of range. Valid values are from %d to %d.", minVlanId, maxVlanId));
        }
        return empty();
    }
}
