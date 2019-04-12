/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.configuration.difference;

import com.intel.rsd.nodecomposer.persistence.redfish.IpTransportDetails;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV4Address;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.IpV6Address;
import org.apache.commons.lang3.builder.ToStringBuilder;

import java.util.Objects;

class IpTransportDetailsEquator implements EquatorWithHashBuilder<IpTransportDetails> {
    @Override
    public boolean equate(IpTransportDetails first, IpTransportDetails second) {
        return Objects.equals(first.getTransportProtocol(), second.getTransportProtocol())
            && Objects.equals(first.getPort(), second.getPort())
            && Objects.equals(first.getIpV4Address(), second.getIpV4Address())
            && Objects.equals(first.getIpV6Address(), second.getIpV6Address());
    }

    @Override
    public int hash(IpTransportDetails object) {
        return Objects.hash(object.getIpV4Address(), object.getIpV6Address(), object.getPort(),
            object.getTransportProtocol());
    }

    @Override
    public String asEquatorString(IpTransportDetails transportDetails) {
        if (transportDetails != null) {
            return new ToStringBuilder(transportDetails)
                .append("TransportProtocol", transportDetails.getTransportProtocol())
                .append("Port", transportDetails.getPort())
                .append("IpV4Address", asEquatorString(transportDetails.getIpV4Address()))
                .append("Ipv6Address", asEquatorString(transportDetails.getIpV6Address()))
                .toString();
        }
        return null;
    }

    private String asEquatorString(IpV4Address ipV4Address) {
        if (ipV4Address != null) {
            return new ToStringBuilder(ipV4Address)
                .append("Address", ipV4Address.getAddress())
                .append("SubnetMask", ipV4Address.getSubnetMask())
                .append("Gateway", ipV4Address.getGateway())
                .append("Oem", ipV4Address.getOem())
                .append("AddressOrigin", ipV4Address.getAddressOrigin())
                .toString();
        }
        return null;
    }

    private String asEquatorString(IpV6Address ipV6Address) {
        if (ipV6Address != null) {
            return new ToStringBuilder(ipV6Address)
                .append("Address", ipV6Address.getAddress())
                .append("Origin", ipV6Address.getAddressOrigin())
                .append("AddressState", ipV6Address.getAddressState())
                .append("Oem", ipV6Address.getOem())
                .append("PrefixLength", ipV6Address.getPrefixLength())
                .toString();
        }
        return null;
    }
}
