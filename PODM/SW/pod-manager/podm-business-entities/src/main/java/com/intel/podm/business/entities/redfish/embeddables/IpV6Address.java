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

import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.IpV6AddressOrigin;

import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Enumerated;
import java.util.Objects;

import static javax.persistence.EnumType.STRING;

@Embeddable
public class IpV6Address {
    @Column(name = "address")
    private String address;

    @Column(name = "prefix_length")
    private Integer prefixLength;

    @Column(name = "address_origin")
    @Enumerated(STRING)
    private IpV6AddressOrigin addressOrigin;

    @Column(name = "address_state")
    @Enumerated(STRING)
    private AddressState addressState;

    @Column(name = "oem")
    private String oem;

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }

    public Integer getPrefixLength() {
        return prefixLength;
    }

    public void setPrefixLength(Integer prefixLength) {
        this.prefixLength = prefixLength;
    }

    public IpV6AddressOrigin getAddressOrigin() {
        return addressOrigin;
    }

    public void setAddressOrigin(IpV6AddressOrigin addressOrigin) {
        this.addressOrigin = addressOrigin;
    }

    public AddressState getAddressState() {
        return addressState;
    }

    public void setAddressState(AddressState addressState) {
        this.addressState = addressState;
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
        IpV6Address that = (IpV6Address) o;
        return Objects.equals(address, that.address)
            && Objects.equals(prefixLength, that.prefixLength)
            && addressOrigin == that.addressOrigin
            && addressState == that.addressState
            && Objects.equals(oem, that.oem);
    }

    @Override
    public int hashCode() {
        return Objects.hash(address, prefixLength, addressOrigin, addressState, oem);
    }
}
