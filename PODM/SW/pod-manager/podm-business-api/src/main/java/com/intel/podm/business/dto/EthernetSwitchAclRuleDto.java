/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.type", "id", "name", "description", "ruleId", "action", "forwardMirrorInterface", "mirrorPortRegion", "mirrorType",
    "condition", "links"
})
@SuppressWarnings("checkstyle:MethodCount")
public final class EthernetSwitchAclRuleDto extends RedfishDto {
    private Long ruleId;
    private ActionType action;
    private Context forwardMirrorInterface;
    private final Set<Context> mirrorPortRegion = new HashSet<>();
    private MirrorType mirrorType;
    private final ConditionDto condition = new ConditionDto();
    private final Links links = new Links();

    public EthernetSwitchAclRuleDto() {
        super("#EthernetSwitchACLRule.v1_0_0.EthernetSwitchACLRule");
    }

    public Links getLinks() {
        return links;
    }

    public Long getRuleId() {
        return ruleId;
    }

    public void setRuleId(Long ruleId) {
        this.ruleId = ruleId;
    }

    public ActionType getAction() {
        return action;
    }

    public void setAction(ActionType action) {
        this.action = action;
    }

    public Context getForwardMirrorInterface() {
        return forwardMirrorInterface;
    }

    public void setForwardMirrorInterface(Context forwardMirrorInterface) {
        this.forwardMirrorInterface = forwardMirrorInterface;
    }

    public Set<Context> getMirrorPortRegion() {
        return mirrorPortRegion;
    }

    public MirrorType getMirrorType() {
        return mirrorType;
    }

    public void setMirrorType(MirrorType mirrorType) {
        this.mirrorType = mirrorType;
    }

    public ConditionDto getCondition() {
        return condition;
    }

    public final class Links extends RedfishLinksDto {
    }

    @JsonPropertyOrder({
        "IPSource", "IPDestination", "MACSource", "MACDestination", "VLANId", "L4SourcePort", "L4DestinationPort", "L4Protocol"
    })
    public static final class ConditionDto {
        @JsonProperty("IPSource")
        private IpConditionDto ipSource;
        @JsonProperty("IPDestination")
        private IpConditionDto ipDestination;
        @JsonProperty("MACSource")
        private MacConditionDto macSource;
        @JsonProperty("MACDestination")
        private MacConditionDto macDestination;
        @JsonProperty("VLANId")
        private VlanIdConditionDto vlanId;
        @JsonProperty("L4SourcePort")
        private PortConditionDto l4SourcePort;
        @JsonProperty("L4DestinationPort")
        private PortConditionDto l4DestinationPort;
        @JsonProperty("L4Protocol")
        private Long l4Protocol;

        public IpConditionDto getIpSource() {
            return ipSource;
        }

        public void setIpSource(IpConditionDto ipSource) {
            this.ipSource = ipSource;
        }

        public IpConditionDto getIpDestination() {
            return ipDestination;
        }

        public void setIpDestination(IpConditionDto ipDestination) {
            this.ipDestination = ipDestination;
        }

        public MacConditionDto getMacSource() {
            return macSource;
        }

        public void setMacSource(MacConditionDto macSource) {
            this.macSource = macSource;
        }

        public MacConditionDto getMacDestination() {
            return macDestination;
        }

        public void setMacDestination(MacConditionDto macDestination) {
            this.macDestination = macDestination;
        }

        public VlanIdConditionDto getVlanId() {
            return vlanId;
        }

        public void setVlanId(VlanIdConditionDto vlanId) {
            this.vlanId = vlanId;
        }

        public PortConditionDto getL4SourcePort() {
            return l4SourcePort;
        }

        public void setL4SourcePort(PortConditionDto l4SourcePort) {
            this.l4SourcePort = l4SourcePort;
        }

        public PortConditionDto getL4DestinationPort() {
            return l4DestinationPort;
        }

        public void setL4DestinationPort(PortConditionDto l4DestinationPort) {
            this.l4DestinationPort = l4DestinationPort;
        }

        public Long getL4Protocol() {
            return l4Protocol;
        }

        public void setL4Protocol(Long l4Protocol) {
            this.l4Protocol = l4Protocol;
        }

        @JsonPropertyOrder({"IPv4Address", "Mask"})
        public static class IpConditionDto {
            @JsonProperty("IPv4Address")
            private String ipV4Address;
            @JsonProperty("Mask")
            private String mask;

            public String getIpV4Address() {
                return ipV4Address;
            }

            public void setIpV4Address(String ipV4Address) {
                this.ipV4Address = ipV4Address;
            }

            public String getMask() {
                return mask;
            }

            public void setMask(String mask) {
                this.mask = mask;
            }
        }

        @JsonPropertyOrder({"MACAddress", "Mask"})
        public static class MacConditionDto {
            @JsonProperty("MACAddress")
            private String address;
            @JsonProperty("Mask")
            private String mask;

            public String getAddress() {
                return address;
            }

            public void setAddress(String address) {
                this.address = address;
            }

            public String getMask() {
                return mask;
            }

            public void setMask(String mask) {
                this.mask = mask;
            }
        }

        @JsonPropertyOrder({"Id", "Mask"})
        public static class VlanIdConditionDto {
            @JsonProperty("Id")
            private Long id;
            @JsonProperty("Mask")
            private Long mask;

            public Long getId() {
                return id;
            }

            public void setId(Long id) {
                this.id = id;
            }

            public Long getMask() {
                return mask;
            }

            public void setMask(Long mask) {
                this.mask = mask;
            }
        }

        @JsonPropertyOrder({"Port", "Mask"})
        public static class PortConditionDto {
            @JsonProperty("Port")
            private Long port;
            @JsonProperty("Mask")
            private Long mask;

            public Long getPort() {
                return port;
            }

            public void setPort(Long port) {
                this.port = port;
            }

            public Long getMask() {
                return mask;
            }

            public void setMask(Long mask) {
                this.mask = mask;
            }
        }
    }
}
