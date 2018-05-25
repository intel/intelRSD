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

package com.intel.podm.business.entities.redfish.embeddables;

import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Embedded;

@Embeddable
@SuppressWarnings({"checkstyle:MethodCount"})
public class Condition {
    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "ipV4Address", column = @Column(name = "source_ip_v4_address")),
        @AttributeOverride(name = "mask", column = @Column(name = "source_ip_mask"))
    })
    private IpCondition ipSource;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "ipV4Address", column = @Column(name = "destination_ip_v4_address")),
        @AttributeOverride(name = "mask", column = @Column(name = "destination_ip_mask"))
    })
    private IpCondition ipDestination;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "address", column = @Column(name = "source_mac_address")),
        @AttributeOverride(name = "mask", column = @Column(name = "source_mac_mask"))
    })
    private MacCondition macSource;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "address", column = @Column(name = "destination_mac_address")),
        @AttributeOverride(name = "mask", column = @Column(name = "destination_mac_mask"))
    })
    private MacCondition macDestination;

    @Embedded
    private VlanIdCondition vlanId;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "port", column = @Column(name = "source_port")),
        @AttributeOverride(name = "mask", column = @Column(name = "source_port_mask"))
    })
    private PortCondition l4SourcePort;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "port", column = @Column(name = "destination_port")),
        @AttributeOverride(name = "mask", column = @Column(name = "destination_port_mask"))
    })
    private PortCondition l4DestinationPort;

    @Column(name = "l4_protocol")
    private Long l4Protocol;

    public IpCondition getIpSource() {
        return ipSource;
    }

    public void setIpSource(IpCondition ipSource) {
        this.ipSource = ipSource;
    }

    public IpCondition getIpDestination() {
        return ipDestination;
    }

    public void setIpDestination(IpCondition ipDestination) {
        this.ipDestination = ipDestination;
    }

    public MacCondition getMacSource() {
        return macSource;
    }

    public void setMacSource(MacCondition macSource) {
        this.macSource = macSource;
    }

    public MacCondition getMacDestination() {
        return macDestination;
    }

    public void setMacDestination(MacCondition macDestination) {
        this.macDestination = macDestination;
    }

    public VlanIdCondition getVlanId() {
        return vlanId;
    }

    public void setVlanId(VlanIdCondition vlanId) {
        this.vlanId = vlanId;
    }

    public PortCondition getL4SourcePort() {
        return l4SourcePort;
    }

    public void setL4SourcePort(PortCondition l4SourcePort) {
        this.l4SourcePort = l4SourcePort;
    }

    public PortCondition getL4DestinationPort() {
        return l4DestinationPort;
    }

    public void setL4DestinationPort(PortCondition l4DestinationPort) {
        this.l4DestinationPort = l4DestinationPort;
    }

    public Long getL4Protocol() {
        return l4Protocol;
    }

    public void setL4Protocol(Long l4Protocol) {
        this.l4Protocol = l4Protocol;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        Condition condition = (Condition) o;
        return new EqualsBuilder()
            .append(ipSource, condition.ipSource)
            .append(ipDestination, condition.ipDestination)
            .append(macSource, condition.macSource)
            .append(macDestination, condition.macDestination)
            .append(vlanId, condition.vlanId)
            .append(l4SourcePort, condition.l4SourcePort)
            .append(l4DestinationPort, condition.l4DestinationPort)
            .append(l4Protocol, condition.l4Protocol)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(ipSource)
            .append(ipDestination)
            .append(macSource)
            .append(macDestination)
            .append(vlanId)
            .append(l4SourcePort)
            .append(l4DestinationPort)
            .append(l4Protocol)
            .toHashCode();
    }

    @Embeddable
    public static class IpCondition {
        @Column(name = "ip_v4_address")
        private String ipV4Address;
        @Column(name = "ip_mask")
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

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            IpCondition that = (IpCondition) o;
            return new EqualsBuilder()
                .append(ipV4Address, that.ipV4Address)
                .append(mask, that.mask)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(ipV4Address)
                .append(mask)
                .toHashCode();
        }
    }

    @Embeddable
    public static class MacCondition {
        @Column(name = "address")
        private String address;
        @Column(name = "mac_mask")
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

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            MacCondition that = (MacCondition) o;
            return new EqualsBuilder()
                .append(address, that.address)
                .append(mask, that.mask)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(address)
                .append(mask)
                .toHashCode();
        }
    }

    @Embeddable
    public static class VlanIdCondition {
        @Column(name = "vlan_id")
        private String id;
        @Column(name = "vlan_mask")
        private String mask;

        public String getId() {
            return id;
        }

        public void setId(String id) {
            this.id = id;
        }

        public String getMask() {
            return mask;
        }

        public void setMask(String mask) {
            this.mask = mask;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            VlanIdCondition that = (VlanIdCondition) o;
            return new EqualsBuilder()
                .append(id, that.id)
                .append(mask, that.mask)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(id)
                .append(mask)
                .toHashCode();
        }
    }

    @Embeddable
    public static class PortCondition {
        @Column(name = "port")
        private Long port;
        @Column(name = "port_mask")
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

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            PortCondition that = (PortCondition) o;
            return new EqualsBuilder()
                .append(port, that.port)
                .append(mask, that.mask)
                .isEquals();
        }

        @Override
        public int hashCode() {
            return new HashCodeBuilder()
                .append(port)
                .append(mask)
                .toHashCode();
        }
    }
}
