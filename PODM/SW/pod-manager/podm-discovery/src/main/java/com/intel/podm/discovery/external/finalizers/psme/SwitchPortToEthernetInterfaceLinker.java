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

package com.intel.podm.discovery.external.finalizers.psme;

import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import java.util.Collection;
import java.util.Objects;

@Dependent
public class SwitchPortToEthernetInterfaceLinker {
    public void link(Collection<EthernetInterface> ethernetInterfaces, Collection<EthernetSwitchPort> ethernetSwitchPorts) {
        ethernetInterfaces.forEach(EthernetInterface::unlinkEthernetSwitchPort);

        for (EthernetInterface ethernetInterface : ethernetInterfaces) {
            for (EthernetSwitchPort ethernetSwitchPort : ethernetSwitchPorts) {
                MacAddress ethernetInterfaceMacAddress = ethernetInterface.getMacAddress();
                MacAddress switchPortNeighborMac = ethernetSwitchPort.getNeighborMac();
                if (ethernetInterfaceMacAddress != null && Objects.equals(ethernetInterfaceMacAddress, switchPortNeighborMac)) {
                    ethernetSwitchPort.linkEthernetInterface(ethernetInterface);
                }
            }
        }
    }
}
