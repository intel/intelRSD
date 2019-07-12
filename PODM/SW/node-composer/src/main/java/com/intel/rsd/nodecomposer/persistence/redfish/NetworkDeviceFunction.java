/*
 * Copyright (c) 2017-2019 Intel Corporation
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
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IscsiBoot;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Embedded;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "network_device_function")
public class NetworkDeviceFunction extends DiscoverableEntity {
    @Getter
    @Setter
    @Embedded
    private IscsiBoot iscsiBoot;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "network_interface_id")
    private NetworkInterface networkInterface;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "network_adapter_id")
    private NetworkAdapter networkAdapter;

    public NetworkInterface getNetworkInterface() {
        return networkInterface;
    }

    public void setNetworkInterface(NetworkInterface networkInterface) {
        this.networkInterface = networkInterface;
    }

    public void unlinkNetworkInterface(NetworkInterface networkInterface) {
        if (Objects.equals(this.networkInterface, networkInterface)) {
            this.networkInterface = null;
            if (networkInterface != null) {
                networkInterface.unlinkNetworkDeviceFunction(this);
            }
        }
    }

    public NetworkAdapter getNetworkAdapter() {
        return networkAdapter;
    }

    public void setNetworkAdapter(NetworkAdapter networkAdapter) {
        this.networkAdapter = networkAdapter;
    }

    public void unlinkNetworkAdapter(NetworkAdapter networkAdapter) {
        if (Objects.equals(this.networkAdapter, networkAdapter)) {
            this.networkAdapter = null;
            if (networkAdapter != null) {
                networkAdapter.unlinkNetworkDeviceFunction(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkNetworkAdapter(networkAdapter);
        unlinkNetworkInterface(networkInterface);
    }
}
