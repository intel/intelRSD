/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.service.detector.file;

import java.net.URI;
import java.time.LocalDateTime;

import static java.lang.String.format;
import static java.util.Objects.hash;

public class ServiceCandidate {
    private URI endpointUri;
    private LocalDateTime updateDate;
    private long retries;

    public ServiceCandidate() {
    }

    public ServiceCandidate(URI endpointUri, LocalDateTime updateDate) {
        if (endpointUri == null) {
            throw new IllegalArgumentException("URI cannot be null in ServiceCandidate");
        }
        this.endpointUri = endpointUri;
        this.updateDate = updateDate;
        this.retries = 0;
    }

    public URI getEndpointUri() {
        return endpointUri;
    }

    public long getRetries() {
        return retries;
    }

    public void increaseRetries() {
        this.retries++;
    }

    public LocalDateTime getUpdateDate() {
        return updateDate;
    }

    public void setEndpointUri(URI endpointUri) {
        this.endpointUri = endpointUri;
    }

    public void setUpdateDate(LocalDateTime updateDate) {
        this.updateDate = updateDate;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }

        ServiceCandidate candidate = (ServiceCandidate) o;
        return endpointUri.equals(candidate.endpointUri);
    }

    @Override
    public int hashCode() {
        return hash(endpointUri);
    }

    @Override
    public String toString() {
        return format("{URI=%s, updateDate=%s}", getEndpointUri(), updateDate);
    }
}
