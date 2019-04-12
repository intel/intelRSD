/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "ethernet_switch_port_vlan")
public class EthernetSwitchPortVlan extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "tagged")
    private Boolean tagged;

    @Getter
    @Setter
    @Column(name = "vlan_id")
    private Integer vlanId;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_port_id")
    private EthernetSwitchPort ethernetSwitchPort;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_interface_id")
    private EthernetInterface ethernetInterface;

    public EthernetSwitchPort getEthernetSwitchPort() {
        return ethernetSwitchPort;
    }

    public void setEthernetSwitchPort(EthernetSwitchPort ethernetSwitchPort) {
        if (!Objects.equals(this.ethernetSwitchPort, ethernetSwitchPort)) {
            unlinkEthernetSwitchPort(this.ethernetSwitchPort);
            this.ethernetSwitchPort = ethernetSwitchPort;
            if (ethernetSwitchPort != null && !ethernetSwitchPort.getEthernetSwitchPortVlans().contains(this)) {
                ethernetSwitchPort.addEthernetSwitchPortVlan(this);
            }
        }
    }

    public void unlinkEthernetSwitchPort(EthernetSwitchPort ethernetSwitchPort) {
        if (Objects.equals(this.ethernetSwitchPort, ethernetSwitchPort)) {
            this.ethernetSwitchPort = null;
            if (ethernetSwitchPort != null) {
                ethernetSwitchPort.unlinkEthernetSwitchPortVlan(this);
            }
        }
    }

    public EthernetInterface getEthernetInterface() {
        return ethernetInterface;
    }

    public void setEthernetInterface(EthernetInterface ethernetInterface) {
        if (!Objects.equals(this.ethernetInterface, ethernetInterface)) {
            unlinkEthernetInterface(this.ethernetInterface);
            this.ethernetInterface = ethernetInterface;
            if (ethernetInterface != null && !ethernetInterface.getEthernetSwitchPortVlans().contains(this)) {
                ethernetInterface.addEthernetSwitchPortVlan(this);
            }
        }
    }

    public void unlinkEthernetInterface(EthernetInterface ethernetInterface) {
        if (Objects.equals(this.ethernetInterface, ethernetInterface)) {
            this.ethernetInterface = null;
            if (ethernetInterface != null) {
                ethernetInterface.unlinkEthernetSwitchPortVlan(this);
            }
        }
    }

    @Override
    public void preRemove() {
        if (ethernetSwitchPort != null && this.equals(ethernetSwitchPort.getPrimaryVlan())) {
            ethernetSwitchPort.setPrimaryVlan(null);
        }
        unlinkEthernetSwitchPort(ethernetSwitchPort);
        unlinkEthernetInterface(ethernetInterface);
    }
}
