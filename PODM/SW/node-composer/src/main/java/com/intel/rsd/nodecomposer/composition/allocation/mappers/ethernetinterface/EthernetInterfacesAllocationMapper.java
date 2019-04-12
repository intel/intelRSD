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

package com.intel.rsd.nodecomposer.composition.allocation.mappers.ethernetinterface;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.persistence.NonUniqueResultException;
import com.intel.rsd.nodecomposer.persistence.dao.EthernetSwitchPortDao;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.ascendingWhereNullsFirst;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.ascendingWhereNullsLast;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.sort;
import static com.intel.rsd.nodecomposer.composition.allocation.mappers.Sorter.urisAscendingWhereNullsLast;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class EthernetInterfacesAllocationMapper {
    private final EthernetSwitchPortDao ethernetSwitchPortDao;

    @Autowired
    public EthernetInterfacesAllocationMapper(EthernetSwitchPortDao ethernetSwitchPortDao) {
        this.ethernetSwitchPortDao = ethernetSwitchPortDao;
    }

    public Map<EthernetInterface, RequestedNode.EthernetInterface> map(Collection<RequestedNode.EthernetInterface> requestedInterfaces,
                                                                       Collection<EthernetInterface> availableInterfaces) {
        Map<EthernetInterface, RequestedNode.EthernetInterface> mappedInterfaces = new HashMap<>();

        Collection<RequestedNode.EthernetInterface> requestedInterfacesQueue =
            sort(requestedInterfaces, urisAscendingWhereNullsLast(), ascendingWhereNullsFirst(RequestedNode.EthernetInterface::getSpeedMbps));
        Collection<EthernetInterface> availableInterfacesQueue =
            sort(availableInterfaces, ascendingWhereNullsLast(EthernetInterface::getUri), ascendingWhereNullsFirst(EthernetInterface::getSpeedMbps));

        for (RequestedNode.EthernetInterface requestedEthernetInterface : requestedInterfacesQueue) {
            Optional<EthernetInterface> matched = findMatchingInterface(requestedEthernetInterface, availableInterfacesQueue);
            matched.ifPresent(ethernetInterface -> mappedInterfaces.put(ethernetInterface, requestedEthernetInterface));
            matched.ifPresent(availableInterfacesQueue::remove);
        }

        return mappedInterfaces;
    }

    private Optional<EthernetInterface> findMatchingInterface(RequestedNode.EthernetInterface requestedEthernetInterface,
                                                              Collection<EthernetInterface> availableInterfacesQueue) {
        for (EthernetInterface availableInterface : availableInterfacesQueue) {
            if (uriConstraintIsSatisfied(requestedEthernetInterface, availableInterface)
                && macAddressIsSatisfied(requestedEthernetInterface, availableInterface)
                && speedConstraintsAreSatisfied(requestedEthernetInterface, availableInterface)
                && vlansConstraintsAreSatisfied(requestedEthernetInterface, availableInterface)) {

                return of(availableInterface);
            }
        }

        return empty();
    }

    private boolean macAddressIsSatisfied(RequestedNode.EthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
        Optional<MacAddress> availableMacAddress = ofNullable(availableInterface.getMacAddress());
        Optional<MacAddress> requestedMacAddress = ofNullable(requestedEthernetInterface.getMacAddress());

        if (!requestedMacAddress.isPresent()) {
            return true;
        }

        if (!availableMacAddress.isPresent()) {
            return false;
        }

        return requestedMacAddress.get().equals(availableMacAddress.get());
    }

    private boolean uriConstraintIsSatisfied(RequestedNode.EthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
        Optional<ODataId> requestedAddress = ofNullable(requestedEthernetInterface.getResourceODataId());
        return requestedAddress.map(uri -> Objects.equals(uri, availableInterface.getUri())).orElse(true);

    }

    private boolean speedConstraintsAreSatisfied(RequestedNode.EthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
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

    private boolean vlansConstraintsAreSatisfied(RequestedNode.EthernetInterface requestedEthernetInterface, EthernetInterface availableInterface) {
        if (requestedEthernetInterface.getVlans().isPresent()) {
            try {
                return ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(availableInterface.getMacAddress()) != null;
            } catch (NonUniqueResultException e) {
                log.error(format("Could not use Ethernet Interface '%s' for allocation.", availableInterface), e);
                return false;
            }
        }

        return true;
    }
}
