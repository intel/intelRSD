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

package com.intel.podm.business.redfish.services.allocation.validation;

import com.intel.podm.business.Violations;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.AllocationConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static com.intel.podm.common.utils.StringRepresentation.fromIterable;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;

@Dependent
public class EthernetInterfacesValidator {
    @Inject
    @Config
    private Holder<AllocationConfig> allocationConfigHolder;

    public Violations validate(List<RequestedNode.EthernetInterface> ethernetInterfaces) {
        Violations violations = new Violations();

        List<Optional<String>> possibleViolations = new ArrayList<>();

        for (RequestedNode.EthernetInterface ethernetInterface : ethernetInterfaces) {
            Optional<List<RequestedNode.EthernetInterface.Vlan>> vlans = ethernetInterface.getVlans();

            if (!vlans.isPresent()) {
                continue;
            }

            possibleViolations.add(validateUniquenessOfVlans(vlans.get()));
            possibleViolations.add(validateUntaggedVlans(vlans.get()));
            possibleViolations.add(validateUsageOfAllowedVlans(vlans.get()));
            possibleViolations.add(validatePrivateVlanRequestConsistency(ethernetInterface.getPrimaryVlan(), vlans.get()));
        }

        possibleViolations.forEach(possibleViolation -> possibleViolation.ifPresent(violations::addViolation));

        return violations;
    }

    private Optional<String> validateUntaggedVlans(List<RequestedNode.EthernetInterface.Vlan> vlans) {
        List<RequestedNode.EthernetInterface.Vlan> untaggedVlans = vlans.stream().filter(vlan -> !vlan.isTagged()).collect(toList());
        if (untaggedVlans.size() > 1) {
            return of("You can request maximum one untagged VLAN.");
        }
        return empty();
    }

    private Optional<String> validatePrivateVlanRequestConsistency(Integer primaryVlanId, List<RequestedNode.EthernetInterface.Vlan> vlans) {
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

    private Optional<String> validateUsageOfAllowedVlans(List<RequestedNode.EthernetInterface.Vlan> vlans) {
        Iterable<Integer> reservedVlans = allocationConfigHolder.get().getReservedVlanIds();
        for (Integer integer : reservedVlans) {
            if (vlans.stream().anyMatch(vlan -> vlan.getVlanId().equals(integer))) {
                String reservedVlansList = fromIterable(reservedVlans);
                return of("Requested vlan id appeared in reserved vlans list " + reservedVlansList);
            }
        }
        return empty();
    }

    private Optional<String> validateUniquenessOfVlans(List<RequestedNode.EthernetInterface.Vlan> vlans) {
        long numberOfUniqueVlansIds = vlans.stream().map(RequestedNode.EthernetInterface.Vlan::getVlanId).distinct().count();
        return numberOfUniqueVlansIds != vlans.size()
            ? of("There are repetitions in vlan collection")
            : empty();
    }
}
