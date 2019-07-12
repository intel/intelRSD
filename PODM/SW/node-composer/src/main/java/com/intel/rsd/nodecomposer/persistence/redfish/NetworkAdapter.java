/*
 * Copyright (c) 2019 Intel Corporation
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

import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "network_adapter")
public class NetworkAdapter extends DiscoverableEntity {
    @OneToMany(mappedBy = "networkAdapter", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<NetworkDeviceFunction> networkDeviceFunctions = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "chassis_id")
    private Chassis chassis;

    public Set<NetworkDeviceFunction> getNetworkDeviceFunctions() {
        return networkDeviceFunctions;
    }

    public void addNetworkDeviceFunction(NetworkDeviceFunction networkDeviceFunction) {
        requiresNonNull(networkDeviceFunction, "networkDeviceFunction");

        networkDeviceFunctions.add(networkDeviceFunction);
        if (!this.equals(networkDeviceFunction.getNetworkAdapter())) {
            networkDeviceFunction.setNetworkAdapter(this);
        }
    }

    public void unlinkNetworkDeviceFunction(NetworkDeviceFunction networkDeviceFunction) {
        if (networkDeviceFunctions.contains(networkDeviceFunction)) {
            networkDeviceFunctions.remove(networkDeviceFunction);
            if (networkDeviceFunction != null) {
                networkDeviceFunction.unlinkNetworkAdapter(this);
            }
        }
    }

    public Chassis getChassis() {
        return chassis;
    }

    public void setChassis(Chassis chassis) {
        this.chassis = chassis;
    }

    public void unlinkChassis(Chassis chassis) {
        if (Objects.equals(this.chassis, chassis)) {
            this.chassis = null;
            if (chassis != null) {
                chassis.unlinkNetworkAdapter(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(networkDeviceFunctions, this::unlinkNetworkDeviceFunction);
        unlinkChassis(chassis);
    }
}
