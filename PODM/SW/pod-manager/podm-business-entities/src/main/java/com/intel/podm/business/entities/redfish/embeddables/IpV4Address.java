/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.common.types.IpV4AddressOrigin;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class IpV4Address {
    @Column(name = "address")
    private String address;

    @Column(name = "subnet_mask")
    private String subnetMask;

    @Column(name = "address_origin")
    @Enumerated(STRING)
    private IpV4AddressOrigin addressOrigin;

    @Column(name = "gateway")
    private String gateway;

    @Column(name = "oem")
    private String oem;

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public String getSubnetMask() {
        return subnetMask;
    }

    public void setSubnetMask(String subnetMask) {
        this.subnetMask = subnetMask;
    }

    public IpV4AddressOrigin getAddressOrigin() {
        return addressOrigin;
    }

    public void setAddressOrigin(IpV4AddressOrigin addressOrigin) {
        this.addressOrigin = addressOrigin;
    }

    public String getGateway() {
        return gateway;
    }

    public void setGateway(String gateway) {
        this.gateway = gateway;
    }

    public String getOem() {
        return oem;
    }

    public void setOem(String oem) {
        this.oem = oem;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        IpV4Address that = (IpV4Address) o;

        return new EqualsBuilder()
            .append(address, that.address)
            .append(subnetMask, that.subnetMask)
            .append(addressOrigin, that.addressOrigin)
            .append(gateway, that.gateway)
            .append(oem, that.oem)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(address)
            .append(subnetMask)
            .append(addressOrigin)
            .append(gateway)
            .append(oem)
            .toHashCode();
    }
}
