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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.services.detection.ServiceEndpoint;

import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.HashSet;
import java.util.Set;

import static com.google.common.base.Preconditions.checkNotNull;
import static javax.ejb.LockType.READ;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.MANDATORY;

/**
 * Class responsible for protecting storage assets from being
 * wiped out by deep discovery and composition processes.
 */
@Singleton
public class StorageGuard {

    private final Set<MacAddress> storageMacAddresses = new HashSet<>();

    @Inject
    private Logger logger;

    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Lock(WRITE)
    public void protectStorageEndpoint(ServiceEndpoint endpoint) {
        MacAddress macAddress = endpoint.getMacAddress();
        if (macAddress != null) {
            storageMacAddresses.add(macAddress);
            ComputerSystem computerSystem = getAssociatedComputerSystemByMacAddress(macAddress);
            if (computerSystem != null) {
                associateComputerSystemWithStorage(computerSystem);
            }
            logger.i("Storage detected with macAddress: {}", macAddress);
        } else {
            logger.w("No MAC address available for storage at {}, it will not be protected", endpoint.getEndpointUri());
        }
    }

    private ComputerSystem getAssociatedComputerSystemByMacAddress(MacAddress macAddress) {
        EthernetInterface iface;
        try {
            iface = ethernetInterfaceDao.getEnabledAndHealthyEthernetInterfaceByMacAddress(macAddress);
        } catch (NonUniqueResultException e) {
            throw new IllegalArgumentException(e.getMessage(), e);
        }
        return (iface != null) ? iface.getComputerSystem() : null;
    }

    @Lock(WRITE)
    public void removeProtectionForStorageEndpoint(ServiceEndpoint endpoint) {
        MacAddress macAddress = endpoint.getMacAddress();
        if (macAddress != null) {
            storageMacAddresses.remove(macAddress);
            logger.i("Removing storage protection for MAC address: {}", macAddress);
        }
    }

    @Lock(READ)
    public boolean isAssociatedWithProtectedStorage(MacAddress macAddress) {
        checkNotNull(macAddress, "macAddress cannot be null!");
        return storageMacAddresses.contains(macAddress);
    }

    @Lock(WRITE)
    @Transactional(MANDATORY)
    public void releaseAssociatedStorageComputerSystem(ExternalService service) {
        if (service.getMacAddress() != null) {
            ComputerSystem computerSystem = getAssociatedComputerSystemByMacAddress(service.getMacAddress());
            if (computerSystem != null) {
                computerSystem.setStorageServiceHost(false);
                if (computerSystem.getComposedNode() == null) {
                    computerSystem.setAllocated(false);
                }
            }
        }
    }

    @Lock(WRITE)
    @Transactional(MANDATORY)
    public void associateComputerSystemWithStorage(ComputerSystem computerSystem) {
        if (!computerSystem.isStorageServiceHost()) {
            computerSystem.setStorageServiceHost(true);
            computerSystem.setAllocated(true);
            logger.i("Computer system {} is now marked as storage service host", computerSystem.getUuid());
        } else {
            logger.i("Computer system {} was already marked as storage service host", computerSystem.getUuid());
        }
    }
}
