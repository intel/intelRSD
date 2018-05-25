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

package com.intel.podm.client.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static com.intel.podm.common.types.Ref.of;
import static com.intel.podm.common.types.Ref.unassigned;

@JsonInclude(NON_DEFAULT)
public final class EthernetSwitchAclRuleCondition {
    @JsonProperty("IPSource")
    private Ref<AclRuleConditionIp> ipSource = unassigned();

    @JsonProperty("IPDestination")
    private Ref<AclRuleConditionIp> ipDestination = unassigned();

    @JsonProperty("MACSource")
    private Ref<AclRuleConditionMacAddress> macSource = unassigned();

    @JsonProperty("MACDestination")
    private Ref<AclRuleConditionMacAddress> macDestination = unassigned();

    @JsonProperty("VLANId")
    private Ref<AclRuleConditionId> vlanId = unassigned();

    @JsonProperty("L4SourcePort")
    private Ref<AclRuleConditionPort> l4SourcePort = unassigned();

    @JsonProperty("L4DestinationPort")
    private Ref<AclRuleConditionPort> l4DestinationPort = unassigned();

    @JsonProperty("L4Protocol")
    private Ref<Integer> l4Protocol = unassigned();

    public EthernetSwitchAclRuleCondition() {
    }

    private EthernetSwitchAclRuleCondition(Builder builder) {
        this.ipSource = builder.ipSource;
        this.ipDestination = builder.ipDestination;
        this.macSource = builder.macSource;
        this.macDestination = builder.macDestination;
        this.vlanId = builder.vlanId;
        this.l4SourcePort = builder.l4SourcePort;
        this.l4DestinationPort = builder.l4DestinationPort;
        this.l4Protocol = builder.l4Protocol;
    }

    public static Builder newEthernetSwitchAclRuleCondition() {
        return new Builder();
    }

    public void setIpSource(Ref<AclRuleConditionIp> ipSource) {
        this.ipSource = ipSource;
    }

    public void setIpDestination(Ref<AclRuleConditionIp> ipDestination) {
        this.ipDestination = ipDestination;
    }

    public void setMacSource(Ref<AclRuleConditionMacAddress> macSource) {
        this.macSource = macSource;
    }

    public void setMacDestination(Ref<AclRuleConditionMacAddress> macDestination) {
        this.macDestination = macDestination;
    }

    public void setVlanId(Ref<AclRuleConditionId> vlanId) {
        this.vlanId = vlanId;
    }

    public void setL4SourcePort(Ref<AclRuleConditionPort> l4SourcePort) {
        this.l4SourcePort = l4SourcePort;
    }

    public void setL4DestinationPort(Ref<AclRuleConditionPort> l4DestinationPort) {
        this.l4DestinationPort = l4DestinationPort;
    }

    public void setL4Protocol(Ref<Integer> l4Protocol) {
        this.l4Protocol = l4Protocol;
    }

    @JsonInclude(NON_DEFAULT)
    public static class AclRuleConditionIp {
        @JsonProperty("IPv4Address")
        private Ref<String> ipv4Address = unassigned();

        @JsonProperty("Mask")
        private Ref<String> mask = unassigned();

        public void setIpv4Address(Ref<String> ipv4Address) {
            this.ipv4Address = ipv4Address;
        }

        public void setMask(Ref<String> mask) {
            this.mask = mask;
        }
    }

    @JsonInclude(NON_DEFAULT)
    public static class AclRuleConditionMacAddress {
        @JsonProperty("MACAddress")
        private Ref<String> macAddress = unassigned();

        @JsonProperty("Mask")
        private Ref<String> mask = unassigned();

        public void setMacAddress(Ref<String> macAddress) {
            this.macAddress = macAddress;
        }

        public void setMask(Ref<String> mask) {
            this.mask = mask;
        }
    }

    @JsonInclude(NON_DEFAULT)
    public static class AclRuleConditionId {
        @JsonProperty("Id")
        private Ref<Long> id = unassigned();

        @JsonProperty("Mask")
        private Ref<Long> mask = unassigned();

        public void setId(Ref<Long> id) {
            this.id = id;
        }

        public void setMask(Ref<Long> mask) {
            this.mask = mask;
        }
    }

    @JsonInclude(NON_DEFAULT)
    public static class AclRuleConditionPort {
        @JsonProperty("Port")
        private Ref<Long> port = unassigned();

        @JsonProperty("Mask")
        private Ref<Long> mask = unassigned();

        public void setPort(Ref<Long> port) {
            this.port = port;
        }

        public void setMask(Ref<Long> mask) {
            this.mask = mask;
        }
    }

    @SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
    public static final class Builder {
        private Ref<AclRuleConditionIp> ipSource = unassigned();
        private Ref<AclRuleConditionIp> ipDestination = unassigned();
        private Ref<AclRuleConditionMacAddress> macSource = unassigned();
        private Ref<AclRuleConditionMacAddress> macDestination = unassigned();
        private Ref<AclRuleConditionId> vlanId = unassigned();
        private Ref<AclRuleConditionPort> l4SourcePort = unassigned();
        private Ref<AclRuleConditionPort> l4DestinationPort = unassigned();
        private Ref<Integer> l4Protocol = unassigned();

        private Builder() {
        }

        public EthernetSwitchAclRuleCondition build() {
            return new EthernetSwitchAclRuleCondition(this);
        }

        public Builder ipSource(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionIp> conditionIp) {
            conditionIp.ifAssigned(ipSource -> this.ipSource = of(mapConditionIp(ipSource)));
            return this;
        }

        public Builder ipDestination(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionIp> conditionIp) {
            conditionIp.ifAssigned(ipDestination -> this.ipDestination = of(mapConditionIp(ipDestination)));

            return this;
        }

        private static AclRuleConditionIp mapConditionIp(RedfishEthernetSwitchAclRule.AclRuleConditionIp aclRuleConditionIp) {
            AclRuleConditionIp ip = new AclRuleConditionIp();
            ip.setIpv4Address(aclRuleConditionIp.getIpv4Address());
            ip.setMask(aclRuleConditionIp.getMask());

            return ip;
        }

        public Builder macSource(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionMacAddress> conditionMacAddress) {
            conditionMacAddress.ifAssigned(macSource -> this.macSource = of(mapConditionMacAddress(macSource)));

            return this;
        }

        public Builder macDestination(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionMacAddress> conditionMacAddress) {
            conditionMacAddress.ifAssigned(macDestination -> this.macDestination = of(mapConditionMacAddress(macDestination)));

            return this;
        }

        private static AclRuleConditionMacAddress mapConditionMacAddress(RedfishEthernetSwitchAclRule.AclRuleConditionMacAddress conditionMacAddress) {
            AclRuleConditionMacAddress macAddress = new AclRuleConditionMacAddress();
            macAddress.setMacAddress(conditionMacAddress.getMacAddress());
            macAddress.setMask(conditionMacAddress.getMask());

            return macAddress;
        }

        public Builder vlanId(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionId> conditionId) {
            conditionId.ifAssigned(vlanId -> {
                AclRuleConditionId aclRuleConditionId = new AclRuleConditionId();
                aclRuleConditionId.setId(vlanId.getId());
                aclRuleConditionId.setMask(vlanId.getMask());
                this.vlanId = of(aclRuleConditionId);
            });

            return this;
        }

        public Builder l4SourcePort(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionPort> conditionPort) {
            conditionPort.ifAssigned(sourcePort -> this.l4SourcePort = of(mapConditionPort(sourcePort)));

            return this;
        }

        public Builder l4DestinationPort(Ref<? extends RedfishEthernetSwitchAclRule.AclRuleConditionPort> conditionPort) {
            conditionPort.ifAssigned(destPort -> this.l4DestinationPort = of(mapConditionPort(destPort)));

            return this;
        }

        public Builder l4Protocol(Ref<Integer> l4Protocol) {
            this.l4Protocol = l4Protocol;

            return this;
        }

        private static AclRuleConditionPort mapConditionPort(RedfishEthernetSwitchAclRule.AclRuleConditionPort aclRuleConditionPort) {
            AclRuleConditionPort port = new AclRuleConditionPort();
            port.setPort(aclRuleConditionPort.getPort());
            port.setMask(aclRuleConditionPort.getMask());

            return port;
        }

    }
}
