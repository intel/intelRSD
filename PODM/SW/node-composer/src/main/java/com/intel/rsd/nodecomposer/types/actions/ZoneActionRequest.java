/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.actions;

import java.net.URI;
import java.util.HashSet;
import java.util.Set;

public class ZoneActionRequest {
    private Links links = new Links();

    public ZoneActionRequest(Set<URI> endpointUris) {
        this.links.setEndpoints(endpointUris);
    }

    public Links getLinks() {
        return links;
    }

    public class Links {
        private Set<URI> endpoints = new HashSet<>();

        public void setEndpoints(Set<URI> endpoints) {
            this.endpoints.addAll(endpoints);
        }

        public Set<URI> getEndpoints() {
            return endpoints;
        }
    }
}
