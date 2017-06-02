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

package com.intel.podm.ipxesupplier;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class IpxeDirectory {
    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Inject
    private IpxeScriptFactory ipxeScriptFactory;

    @Transactional(REQUIRES_NEW)
    public IpxeScript getIpxeScript(MacAddress macAddress) throws AssetNotFoundException {
        EthernetInterface ethernetInterface = findEthernetInterface(macAddress);
        ComputerSystem computerSystem = ethernetInterface.getComputerSystem();

        if (computerSystem == null) {
            throw new AssetNotFoundException("Ethernet Interfaces with specified MAC Address does not belong to computer system.");
        }

        return ipxeScriptFactory.create(computerSystem);
    }

    private EthernetInterface findEthernetInterface(MacAddress macAddress) throws AssetNotFoundException {
        EthernetInterface ethernetInterface;

        try {
            ethernetInterface = ethernetInterfaceDao.getEnabledAndHealthyEthernetInterfaceByMacAddress(macAddress);
        } catch (NonUniqueResultException e) {
            throw new AssetNotFoundException("Error appeared while finding Ethernet Interfaces for MAC Address: " + macAddress, e);
        }

        if (ethernetInterface == null) {
            throw new AssetNotFoundException("Ethernet Interface with MAC Address: " + macAddress + " was not found.");
        }
        return ethernetInterface;
    }
}
