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

package com.intel.podm.ipxesupplier;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@ApplicationScoped
public class IpxeDirectory {
    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Inject
    private IpxeScriptFactory ipxeScriptFactory;

    @Inject
    private ComputerSystemDao computerSystemDao;

    @Transactional(REQUIRES_NEW)
    public IpxeScript getIpxeScript(MacAddress macAddress, UUID uuid) throws AssetNotFoundException {
        ComputerSystem computerSystem = tryFindComputerSystemByUuid(uuid);

        if (computerSystem == null) { //fallback to MAC
            EthernetInterface ethernetInterface = findEthernetInterface(macAddress);
            computerSystem = ethernetInterface.getComputerSystem();

            if (computerSystem == null) {
                throw new AssetNotFoundException("Ethernet Interfaces with specified MAC Address does not belong to computer system.");
            }
        }

        return ipxeScriptFactory.create(computerSystem);
    }

    private ComputerSystem tryFindComputerSystemByUuid(UUID uuid) {
        return computerSystemDao.findPrimarySystem(uuid).orElse(null);
    }

    private EthernetInterface findEthernetInterface(MacAddress macAddress) throws AssetNotFoundException {
        EthernetInterface ethernetInterface;

        try {
            ethernetInterface = ethernetInterfaceDao.getEnabledEthernetInterfaceByMacAddress(macAddress);
        } catch (NonUniqueResultException e) {
            throw new AssetNotFoundException("Error appeared while finding Ethernet Interfaces for MAC Address: " + macAddress, e);
        }

        if (ethernetInterface == null) {
            throw new AssetNotFoundException("Ethernet Interface with MAC Address: " + macAddress + " was not found.");
        }
        return ethernetInterface;
    }
}
