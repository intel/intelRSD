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

package com.intel.podm.allocation.mappers.ethernetinterface;

import com.intel.podm.allocation.mappers.Sorter;
import com.intel.podm.business.dto.redfish.RequestedEthernetInterface;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;

import static com.google.common.collect.Maps.newHashMap;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;

@Dependent
public class EthernetInterfacesAllocationMapper {
    @Inject
    EthernetSwitchPortDao ethernetSwitchPortDao;

    public Map<EthernetInterface, RequestedEthernetInterface> map(List<EthernetInterface> availableInterfaces,
                                                                  List<RequestedEthernetInterface> requestedInterfaces) {
        Map<EthernetInterface, RequestedEthernetInterface> mappedInterfaces = newHashMap();

        List<RequestedEthernetInterface> requestedInterfacesQueue =
                Sorter.sort(requestedInterfaces,
                        Sorter.contextsAscendingWhereNullsLast(), Sorter.ascendingWhereNullsFirst(RequestedEthernetInterface::getSpeedMbps));
        List<EthernetInterface> availableInterfacesQueue =
                Sorter.sort(availableInterfaces,
                        Sorter.ascendingWhereNullsLast(EthernetInterface::getId), Sorter.ascendingWhereNullsFirst(EthernetInterface::getSpeedMbps));

        for (RequestedEthernetInterface requestedEthernetInterface : requestedInterfacesQueue) {
            Optional<EthernetInterface> matched = findMatchingInterface(requestedEthernetInterface, availableInterfacesQueue);
            matched.ifPresent(ethernetInterface -> mappedInterfaces.put(ethernetInterface, requestedEthernetInterface));
            matched.ifPresent(availableInterfacesQueue::remove);
        }

        return mappedInterfaces;
    }

    private Optional<EthernetInterface> findMatchingInterface(RequestedEthernetInterface requestedEthernetInterface,
                                                              List<EthernetInterface> availableInterfacesQueue) {
        for (EthernetInterface availableInterface : availableInterfacesQueue) {
            if (idConstraintIsSatisfied(requestedEthernetInterface, availableInterface)
                    && speedConstraintsAreSatisfied(requestedEthernetInterface, availableInterface)
                    && vlansConstraintsAreSatisfied(requestedEthernetInterface, availableInterface)) {

                return of(availableInterface);
            }
        }

        return empty();
    }

    private boolean idConstraintIsSatisfied(RequestedEthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
        Id availableId = availableInterface.getId();
        Optional<Context> requestedAddress = ofNullable(requestedEthernetInterface.getResourceContext());

        if (!requestedAddress.isPresent()) {
            return true;
        }

        return Objects.equals(requestedAddress.get().getId(), availableId);
    }

    private boolean speedConstraintsAreSatisfied(RequestedEthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
        Optional<Integer> availableSpeed = ofNullable(availableInterface.getSpeedMbps());
        Optional<Integer> requestedSpeed = ofNullable(requestedEthernetInterface.getSpeedMbps());

        if (!requestedSpeed.isPresent()) {
            return true;
        }

        if (!availableSpeed.isPresent()) {
            return false;
        }

        return requestedSpeed.get() <= availableSpeed.get();
    }

    private boolean vlansConstraintsAreSatisfied(RequestedEthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
        if (requestedEthernetInterface.getVlans().isPresent()) {
            return availableInterface.getNeighborSwitchPort() != null;
        }

        return true;
    }
}
