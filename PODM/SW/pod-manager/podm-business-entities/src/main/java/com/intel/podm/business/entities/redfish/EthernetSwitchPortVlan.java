/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "ethernet_switch_port_vlan", indexes = @Index(name = "idx_ethernet_switch_port_vlan_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class EthernetSwitchPortVlan extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "vlan_enable")
    private Boolean vlanEnable;

    @Column(name = "tagged")
    private Boolean tagged;

    @Column(name = "vlan_id")
    private Integer vlanId;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_port_id")
    private EthernetSwitchPort ethernetSwitchPort;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_interface_id")
    private EthernetInterface ethernetInterface;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public void setVlanEnable(Boolean vlanEnable) {
        this.vlanEnable = vlanEnable;
    }

    public Boolean getTagged() {
        return tagged;
    }

    public void setTagged(Boolean tagged) {
        this.tagged = tagged;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public void setVlanId(Integer vlanId) {
        this.vlanId = vlanId;
    }

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

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, ethernetSwitchPort) || isContainedBy(possibleParent, ethernetInterface);
    }
}
