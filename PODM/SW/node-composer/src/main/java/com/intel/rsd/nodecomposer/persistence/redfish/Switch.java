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
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "switch")
public class Switch extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "switch_type")
    @Enumerated(STRING)
    private Protocol switchType;

    @OneToMany(mappedBy = "fabricSwitch", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Port> ports = new HashSet<>();

    @ManyToMany(mappedBy = "fabricSwitches", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> chassis = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

    public Set<Port> getPorts() {
        return ports;
    }

    public void addPort(Port port) {
        requiresNonNull(port, "port");
        ports.add(port);
        if (!this.equals(port.getSwitch())) {
            port.setSwitch(this);
        }
    }

    public void unlinkPort(Port port) {
        if (ports.contains(port)) {
            ports.remove(port);
            if (port != null) {
                port.unlinkSwitch(this);
            }
        }
    }

    public Set<Chassis> getChassis() {
        return chassis;
    }

    public void addChassis(Chassis newChassis) {
        requiresNonNull(newChassis, "newChassis");

        chassis.add(newChassis);
        if (!newChassis.getSwitch().contains(this)) {
            newChassis.addSwitch(this);
        }
    }

    public void unlinkChassis(Chassis chassisToUnlink) {
        if (chassis.contains(chassisToUnlink)) {
            chassis.remove(chassisToUnlink);
            if (chassisToUnlink != null) {
                chassisToUnlink.unlinkSwitch(this);
            }
        }
    }

    public Fabric getFabric() {
        return fabric;
    }

    public void setFabric(Fabric fabric) {
        if (fabric == null) {
            unlinkFabric(this.fabric);
        } else {
            this.fabric = fabric;
            if (!fabric.getSwitches().contains(this)) {
                fabric.addSwitch(this);
            }
        }
    }

    public void unlinkFabric(Fabric fabric) {
        if (Objects.equals(this.fabric, fabric)) {
            this.fabric = null;
            if (fabric != null) {
                fabric.unlinkSwitch(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(ports, this::unlinkPort);
        unlinkCollection(chassis, this::unlinkChassis);
        unlinkFabric(fabric);
    }
}
