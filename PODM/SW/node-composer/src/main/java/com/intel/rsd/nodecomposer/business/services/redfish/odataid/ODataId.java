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

package com.intel.rsd.nodecomposer.business.services.redfish.odataid;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.EqualsAndHashCode;

import javax.annotation.Nonnull;
import java.io.Serializable;
import java.net.URI;
import java.util.Objects;

import static com.google.common.base.Strings.isNullOrEmpty;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static org.apache.commons.lang3.StringUtils.removeEnd;

@EqualsAndHashCode
public final class ODataId implements Comparable<ODataId>, Serializable {
    private static final long serialVersionUID = -3522343419186404188L;

    @JsonProperty("@odata.id")
    private URI uri;

    @JsonCreator
    public ODataId(@JsonProperty("@odata.id") URI uri) {
        requiresNonNull(uri, "uri");

        URI uriPathWithoutTrailingSlash = URI.create(removeEnd(uri.getPath(), "/"));
        if (isNullOrEmpty(uri.getFragment())) {
            this.uri = uriPathWithoutTrailingSlash;
        } else {
            this.uri = URI.create(uriPathWithoutTrailingSlash + "#" + uri.getFragment());
        }
    }

    public static ODataId oDataIdFromString(String uriString) {
        return new ODataId(URI.create(uriString));
    }

    public static ODataId oDataIdFromUri(URI uri) {
        return new ODataId(uri);
    }

    public ODataId append(String uriString) {
        return oDataIdFromString(uri.toString() + uriString);
    }

    public URI toUri() {
        return uri;
    }

    @Override
    public String toString() {
        return Objects.toString(uri, null);
    }

    @Override
    public int compareTo(@Nonnull ODataId thatODataId) {
        return uri.compareTo(thatODataId.toUri());
    }
}
