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

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.entities.redfish.EthernetInterface.GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS;
import static com.intel.podm.business.entities.redfish.EthernetInterface.GET_ETHERNET_INTERFACE_MULTI_SOURCE;
import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class EthernetInterfaceDao extends Dao<EthernetInterface> {
    @Transactional(MANDATORY)
    public EthernetInterface getEnabledEthernetInterfaceByMacAddress(MacAddress macAddress) throws NonUniqueResultException {
        if (macAddress == null) {
            return null;
        }

        TypedQuery<EthernetInterface> query = entityManager.createNamedQuery(GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS, EthernetInterface.class);
        query.setParameter("macAddress", macAddress);

        List<EthernetInterface> ethernetInterfaces = query.getResultList().stream()
            .map(this::findBaseEthernetInterface)
            .filter(ethernetInterface -> statusOf(ethernetInterface).isEnabled().verify())
            .distinct()
            .collect(toList());

        try {
            return singleOrNull(ethernetInterfaces);
        } catch (IllegalStateException e) {
            throw new NonUniqueResultException(
                format("Couldn't find single, enabled and healthy Ethernet Interface with MAC Address: '%s'. Found %d Ethernet Interfaces.",
                    macAddress, ethernetInterfaces.size()));
        }
    }

    @Transactional(MANDATORY)
    public List<EthernetInterface> findComplementaryEthernetInterfaces(EthernetInterface ethernetInterface) {
        TypedQuery<EthernetInterface> query = entityManager.createNamedQuery(GET_ETHERNET_INTERFACE_MULTI_SOURCE, EthernetInterface.class);
        query.setParameter("multiSourceDiscriminator", ethernetInterface.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", true);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public Optional<EthernetInterface> findPrimaryEthernetInterface(EthernetInterface complementaryEthernetInterface) {
        TypedQuery<EthernetInterface> query = entityManager.createNamedQuery(GET_ETHERNET_INTERFACE_MULTI_SOURCE, EthernetInterface.class);
        query.setParameter("multiSourceDiscriminator", complementaryEthernetInterface.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", false);
        return ofNullable(singleOrNull(query.getResultList()));
    }

    private EthernetInterface findBaseEthernetInterface(EthernetInterface ethernetInterface) {
        List<EthernetInterface> interfaces = new ArrayList<>();
        if (ethernetInterface.isComplementary()) {
            interfaces = findPrimaryEthernetInterface(ethernetInterface)
                .map(Collections::singletonList)
                .orElseGet(Collections::emptyList);
        }
        return interfaces.size() == 1 ? interfaces.get(0) : ethernetInterface;
    }
}
