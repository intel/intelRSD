/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

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
    @SuppressWarnings("checkstyle:CyclomaticComplexity")
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        IpV4Address that = (IpV4Address) o;
        return Objects.equals(address, that.address)
            && Objects.equals(subnetMask, that.subnetMask)
            && addressOrigin == that.addressOrigin
            && Objects.equals(gateway, that.gateway)
            && Objects.equals(oem, that.oem);
    }

    @Override
    public int hashCode() {
        return Objects.hash(address, subnetMask, addressOrigin, gateway, oem);
    }
}
