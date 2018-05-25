/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import javax.persistence.Column;
import javax.persistence.Embeddable;

@Embeddable
public class PciId {
    @Column(name = "vendor_id")
    private String vendorId;

    @Column(name = "device_id")
    private String deviceId;

    @Column(name = "subsystem_id")
    private String subsystemId;

    @Column(name = "subsystem_vendor_id")
    private String subsystemVendorId;

    public String getVendorId() {
        return vendorId;
    }

    public void setVendorId(String vendorId) {
        this.vendorId = vendorId;
    }

    public String getSubsystemVendorId() {
        return subsystemVendorId;
    }

    public void setSubsystemVendorId(String subsystemVendorId) {
        this.subsystemVendorId = subsystemVendorId;
    }

    public String getSubsystemId() {
        return subsystemId;
    }

    public void setSubsystemId(String subsystemId) {
        this.subsystemId = subsystemId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public String getDeviceId() {
        return deviceId;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        PciId pciId = (PciId) o;

        return new EqualsBuilder()
            .append(vendorId, pciId.vendorId)
            .append(deviceId, pciId.deviceId)
            .append(subsystemId, pciId.subsystemId)
            .append(subsystemVendorId, pciId.subsystemVendorId)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(vendorId)
            .append(deviceId)
            .append(subsystemId)
            .append(subsystemVendorId)
            .toHashCode();
    }
}
