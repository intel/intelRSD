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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.net.URI;

import static java.net.URI.create;

public final class ODataId {
    @JsonProperty("@odata.id")
    private URI oDataId;

    public ODataId() {
    }

    public ODataId(URI oDataId) {
        this.oDataId = oDataId;
    }

    public ODataId(String oDataId) {
        this.oDataId = create(oDataId);
    }

    public URI toUri() {
        return oDataId;
    }
}
