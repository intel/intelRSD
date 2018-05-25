/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.utils.StringRepresentation.fromIterable;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class NeighborFinder {
    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public EthernetInterface tryFindNeighborEthernetInterface(EthernetSwitchPort port) throws NeighborFinderException {
        List<String> errors = new ArrayList<>();

        EthernetInterface neighborInterface = null;
        try {
            neighborInterface = ethernetInterfaceDao.getEnabledEthernetInterfaceByMacAddress(port.getNeighborMac());
        } catch (NonUniqueResultException e) {
            errors.add("Couldn't match neighbor Ethernet Interface. " + e.getMessage());
        }

        try {
            ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(port.getNeighborMac());
        } catch (NonUniqueResultException e) {
            errors.add("There are different Ethernet Switch Ports with the same Neighbor MAC Address. " + e.getMessage());
        }

        if (!errors.isEmpty()) {
            logger.e(fromIterable(errors, "\n"));
            throw new NeighborFinderException(errors);
        }
        return neighborInterface;
    }

    @Transactional(MANDATORY)
    public EthernetSwitchPort tryFindNeighborEthernetSwitchPort(EthernetInterface ethernetInterface) throws NeighborFinderException {
        List<String> errors = new ArrayList<>();

        EthernetSwitchPort neighborPort = null;
        try {
            neighborPort = ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress());
        } catch (NonUniqueResultException e) {
            errors.add("Couldn't match neighbor Ethernet Switch Port. " + e.getMessage());
        }

        try {
            ethernetInterfaceDao.getEnabledEthernetInterfaceByMacAddress(ethernetInterface.getMacAddress());
        } catch (NonUniqueResultException e) {
            errors.add("There are different Ethernet Interfaces with the same MAC Address. " + e.getMessage());
        }

        if (!errors.isEmpty()) {
            logger.e(fromIterable(errors, "\n"));
            throw new NeighborFinderException(errors);
        }
        return neighborPort;
    }

    public final class NeighborFinderException extends Exception {
        private final List<String> neighborFinderErrors = new ArrayList<>();

        NeighborFinderException(List<String> errors) {
            super();
            if (errors != null) {
                neighborFinderErrors.addAll(errors);
            }
        }

        public List<String> getNeighborFinderErrors() {
            return neighborFinderErrors;
        }
    }
}
