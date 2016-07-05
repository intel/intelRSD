/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.allocation.validation;

import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.AllocationConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.utils.StringRepresentation.fromList;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;

@Dependent
public class EthernetInterfacesValidator {
    @Inject @Config
    private Holder<AllocationConfig> allocationConfigHolder;

    public Violations validate(List<RequestedEthernetInterface> ethernetInterfaces) {
        Violations violations = new Violations();

        List<Optional<String>> possibleViolations = newArrayList();

        for (RequestedEthernetInterface ethernetInterface : ethernetInterfaces) {
            Optional<List<RequestedEthernetInterface.Vlan>> vlans = ethernetInterface.getVlans();

            if (!vlans.isPresent()) {
                continue;
            }

            possibleViolations.add(validateUniquenessOfVlans(vlans.get()));
            possibleViolations.add(validateUsageOfAllowedVlans(vlans.get()));
            possibleViolations.add(validatePrivateVlanRequestConsistency(ethernetInterface.getPrimaryVlan(), vlans.get()));
        }

        possibleViolations.stream()
                .forEach(possibleViolation -> possibleViolation.ifPresent(violations::addViolation));

        return violations;
    }

    private Optional<String> validatePrivateVlanRequestConsistency(Integer primaryVlanId, List<RequestedEthernetInterface.Vlan> vlans) {
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

    private Optional<String> validateUsageOfAllowedVlans(List<RequestedEthernetInterface.Vlan> vlans) {
        List<Integer> reservedVlans = newArrayList(allocationConfigHolder.get().getReservedVlanIds());

        for (Integer integer : reservedVlans) {
            if (vlans.stream().anyMatch(vlan -> vlan.getVlanId().equals(integer))) {
                String reservedVlansList = fromList(reservedVlans);
                return of("Requested vlan id appeared in reserved vlans list " + reservedVlansList);
            }
        }
        return empty();
    }

    private Optional<String> validateUniquenessOfVlans(List<RequestedEthernetInterface.Vlan> vlans) {
        long numberOfUniqueVlansIds = vlans.stream().map(RequestedEthernetInterface.Vlan::getVlanId).distinct().count();
        return numberOfUniqueVlansIds != vlans.size()
                ? of("There are repetitions in vlan collection")
                : empty();
    }

}
