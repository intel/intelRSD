/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.redfish.EthernetInterface.MAC_ADDRESS;
import static com.intel.podm.business.entities.redfish.EthernetInterface.VLAN_ID;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class EthernetInterfaceDao extends Dao<EthernetInterface> {
    public EthernetInterface getEthernetInterfaceByMacAddress(MacAddress macAddress) {
        return repository.getSingleByProperty(EthernetInterface.class, MAC_ADDRESS, macAddress);
    }

    public EthernetInterface getEthernetInterfaceByVlanId(int vlanId) {
        return repository.getSingleByProperty(EthernetInterface.class, VLAN_ID, vlanId);
    }
}
