/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.client.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.resources.ODataId;

import java.net.URI;
import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static java.util.stream.Collectors.toList;

@JsonInclude(NON_NULL)
public class ZoneActionJson {
    @JsonProperty("Links")
    private Links links = new Links();

    public ZoneActionJson(Set<URI> endpointUris) {
        this.links.setEndpoints(endpointUris);
    }

    @JsonInclude(NON_NULL)
    public class Links {
        @JsonProperty("Endpoints")
        private Set<ODataId> endpoints = new HashSet<>();

        public void setEndpoints(Set<URI> endpointUris) {
            endpoints.addAll(endpointUris.stream()
                .map(ODataId::new)
                .collect(toList()));
        }
    }
}
