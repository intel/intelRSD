/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.json.assembly;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.types.deserialization.PositiveIntegerDeserializer;
import com.intel.rsd.nodecomposer.types.deserialization.RequiredMacAddressDeserializer;
import com.intel.rsd.nodecomposer.types.net.MacAddress;

import java.io.Serializable;
import java.util.List;
import java.util.Optional;

import static java.lang.Boolean.TRUE;
import static java.util.Optional.ofNullable;

public final class RequestedEthernetInterfaceImpl implements RequestedNode.EthernetInterface, Serializable {
    private static final long serialVersionUID = 6061737400294156237L;

    @JsonProperty
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer speedMbps;

    @JsonProperty("PrimaryVLAN")
    @JsonDeserialize(using = PositiveIntegerDeserializer.class)
    private Integer primaryVlan;

    @JsonProperty("VLANs")
    private List<VlanImpl> vlans;

    @JsonProperty("Resource")
    private ODataId resourceODataId;

    @JsonProperty("Chassis")
    private ODataId chassisODataId;

    @JsonProperty("MACAddress")
    @JsonDeserialize(using = RequiredMacAddressDeserializer.class)
    private MacAddress macAddress;

    @Override
    public MacAddress getMacAddress() {
        return macAddress;
    }

    @Override
    public Integer getSpeedMbps() {
        return speedMbps;
    }

    @Override
    public Integer getPrimaryVlan() {
        return primaryVlan;
    }

    @Override
    public Optional<List<Vlan>> getVlans() {
        return ofNullable((List) vlans);
    }

    @Override
    public ODataId getResourceODataId() {
        return resourceODataId;
    }

    @Override
    public ODataId getChassisODataId() {
        return chassisODataId;
    }

    private static final class VlanImpl implements Vlan, Serializable {
        private static final long serialVersionUID = -2207616080915292506L;

        @JsonProperty("VLANId")
        private Integer vlanId;

        @JsonProperty("VLANEnable")
        private Boolean vlanEnable;

        @JsonProperty
        private boolean tagged;

        @Override
        public boolean isTagged() {
            return tagged;
        }

        @Override
        public Integer getVlanId() {
            return vlanId;
        }

        @Override
        public Boolean isEnabled() {
            if (vlanEnable == null) {
                return TRUE;
            }

            return vlanEnable;
        }
    }
}
