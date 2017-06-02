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
package com.intel.podm.services.detection.dhcp.filesystem;

import com.intel.podm.common.types.ServiceType;

import java.util.Objects;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Objects.hash;

public class ServiceListRecord {
    private String url;
    private ServiceType serviceType;

    public ServiceListRecord(ServiceType serviceType, String url) {
        requiresNonNull(url, "url");
        this.url = url;
        this.serviceType = serviceType;
    }

    public String getUrl() {
        return url;
    }

    public ServiceType getServiceType() {
        return serviceType;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof ServiceListRecord)) {
            return false;
        }

        ServiceListRecord that = (ServiceListRecord) o;
        return Objects.equals(url, that.url)
                && serviceType == that.serviceType;
    }

    @Override
    public int hashCode() {
        return hash(url, serviceType);
    }

    @Override
    public String toString() {
        return format("{URL=%s, type=%s}", url, serviceType);
    }
}
