/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.services.detection.filesystem;

import com.google.common.base.Objects;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.common.types.net.MacAddress;

import java.time.LocalDateTime;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.intel.podm.common.types.net.MacAddress.macAddressOrNull;
import static java.lang.String.format;
import static java.time.LocalDateTime.parse;

public class TmpLeasesRecord {

    private String ipAddress;
    private ServiceType serviceType;
    private MacAddress macAddress;
    private LocalDateTime date;

    public TmpLeasesRecord(ServiceType serviceType, String ip, String macAddressString, String date) {
        checkNotNull(serviceType, "Service type cannot be null in 'leases' record");
        checkNotNull(ip, "IP cannot be null in 'leases' record");
        checkNotNull(macAddressString, "MAC address cannot be null in 'leases' record");
        checkNotNull(date, "Date cannot be null in 'leases' record");
        this.serviceType = serviceType;
        this.ipAddress = ip;
        this.macAddress = macAddressOrNull(macAddressString);
        this.date = parse(date);
    }

    public String getIpAddress() {
        return ipAddress;
    }

    public ServiceType getServiceType() {
        return serviceType;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public LocalDateTime getDate() {
        return date;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof TmpLeasesRecord)) {
            return false;
        }

        TmpLeasesRecord that = (TmpLeasesRecord) o;
        return Objects.equal(ipAddress, that.ipAddress)
                && serviceType == that.serviceType
                && Objects.equal(macAddress, that.macAddress)
                && Objects.equal(date, that.date);
    }

    @Override
    public int hashCode() {
        return Objects.hashCode(ipAddress, serviceType, macAddress, date);
    }

    @Override
    public String toString() {
        return format("{IP=%s, type=%s, MAC=%s, date=%s}", ipAddress, serviceType, macAddress, date);
    }
}
