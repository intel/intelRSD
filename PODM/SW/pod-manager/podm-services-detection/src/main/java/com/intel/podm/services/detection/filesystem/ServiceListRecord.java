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

import static com.google.common.base.Preconditions.checkNotNull;
import static java.lang.String.format;

public class ServiceListRecord {
    private String url;
    private ServiceType serviceType;

    public ServiceListRecord(ServiceType serviceType, String url) {
        checkNotNull(url, "URL cannot be null in ServiceListRecord");
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
        return Objects.equal(url, that.url)
                && serviceType == that.serviceType;
    }

    @Override
    public int hashCode() {
        return Objects.hashCode(url, serviceType);
    }

    @Override
    public String toString() {
        return format("{URL=%s, type=%s}", url, serviceType);
    }
}
