/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.dto.redfish.attributes;

public final class PciIdDto {
    private final String deviceId;
    private final String vendorId;
    private final String subsystemId;
    private final String subsystemVendorId;

    private PciIdDto(Builder builder) {
        vendorId = builder.vendorId;
        deviceId = builder.deviceId;
        subsystemId = builder.subsystemId;
        subsystemVendorId = builder.subsystemVendorId;
    }

    public String getVendorId() {
        return vendorId;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public String getSubsystemId() {
        return subsystemId;
    }

    public String getSubsystemVendorId() {
        return subsystemVendorId;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String deviceId;
        private String vendorId;
        private String subsystemId;
        private String subsystemVendorId;

        public Builder deviceId(String deviceId) {
            this.deviceId = deviceId;
            return this;
        }

        public Builder vendorId(String vendorId) {
            this.vendorId = vendorId;
            return this;
        }

        public Builder subsystemId(String subsystemId) {
            this.subsystemId = subsystemId;
            return this;
        }

        public Builder subsystemVendorId(String subsystemVendorId) {
            this.subsystemVendorId = subsystemVendorId;
            return this;
        }

        public PciIdDto build() {
            return new PciIdDto(this);
        }
    }
}
