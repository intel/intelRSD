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

package com.intel.podm.business.services.redfish.odataid;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.redfish.OdataIdProvider;

import java.net.URI;
import java.util.Objects;

public final class ODataId implements OdataIdProvider.ODataId {
    @JsonProperty("@odata.id")
    private URI value;

    public ODataId() {
    }

    public ODataId(String uriValue) {
        this.value = URI.create(uriValue);
    }

    public ODataId(URI uri) {
        this.value = uri;
    }

    @Override
    public URI toUri() {
        return value;
    }

    @Override
    public String toString() {
        return Objects.toString(value);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof ODataId)) {
            return false;
        }
        ODataId oDataId = (ODataId) o;
        return Objects.equals(value, oDataId.value);
    }

    @Override
    public int hashCode() {
        return Objects.hash(value);
    }
}
