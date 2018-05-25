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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;

class ConditionObject {
    @JsonProperty("IPSource")
    private IpConditionObject ipSource;
    @JsonProperty("IPDestination")
    private IpConditionObject ipDestination;
    @JsonProperty("MACSource")
    private MacConditionObject macSource;
    @JsonProperty("MACDestination")
    private MacConditionObject macDestination;
    @JsonProperty("VLANId")
    private VlanIdConditionObject vlanId;
    @JsonProperty("L4SourcePort")
    private PortConditionObject l4SourcePort;
    @JsonProperty("L4DestinationPort")
    private PortConditionObject l4DestinationPort;
    @JsonProperty("L4Protocol")
    private Long l4Protocol;

    public IpConditionObject getIpSource() {
        return ipSource;
    }

    public IpConditionObject getIpDestination() {
        return ipDestination;
    }

    public MacConditionObject getMacSource() {
        return macSource;
    }

    public MacConditionObject getMacDestination() {
        return macDestination;
    }

    public VlanIdConditionObject getVlanId() {
        return vlanId;
    }

    public PortConditionObject getL4SourcePort() {
        return l4SourcePort;
    }

    public PortConditionObject getL4DestinationPort() {
        return l4DestinationPort;
    }

    public Long getL4Protocol() {
        return l4Protocol;
    }

    public static class IpConditionObject {
        @JsonProperty("IPv4Address")
        private String ipV4Address;
        @JsonProperty("Mask")
        private String mask;

        public String getIpV4Address() {
            return ipV4Address;
        }

        public String getMask() {
            return mask;
        }
    }

    public static class MacConditionObject {
        @JsonProperty("MACAddress")
        private String address;
        @JsonProperty("Mask")
        private String mask;

        public String getAddress() {
            return address;
        }

        public String getMask() {
            return mask;
        }
    }

    public static class VlanIdConditionObject {
        @JsonProperty("Id")
        private Long id;
        @JsonProperty("Mask")
        private Long mask;

        public Long getId() {
            return id;
        }

        public Long getMask() {
            return mask;
        }
    }

    public static class PortConditionObject {
        @JsonProperty("Port")
        private Long port;
        @JsonProperty("Mask")
        private Long mask;

        public Long getPort() {
            return port;
        }

        public Long getMask() {
            return mask;
        }
    }
}
