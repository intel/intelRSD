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

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.entities.redfish.EthernetInterface.GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class EthernetInterfaceDao extends Dao<EthernetInterface> {
    public EthernetInterface getEnabledAndHealthyEthernetInterfaceByMacAddress(MacAddress macAddress) throws NonUniqueResultException {
        if (macAddress == null) {
            return null;
        }

        TypedQuery<EthernetInterface> query = entityManager.createNamedQuery(GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS, EthernetInterface.class);
        query.setParameter("macAddress", macAddress);

        List<EthernetInterface> ethernetInterfaces = query.getResultList().stream()
                .filter(EthernetInterface::isEnabledAndHealthy)
                .collect(toList());

        try {
            return singleOrNull(ethernetInterfaces);
        } catch (IllegalStateException e) {
            throw new NonUniqueResultException(
                    format("Couldn't find single, enabled and healthy Ethernet Interface with MAC Address: '%s'. Found %d Ethernet Interfaces.",
                            macAddress, ethernetInterfaces.size()));
        }
    }
}
