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

package com.intel.podm.client.actions.requests;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.resources.ODataId;

import java.net.URI;
import java.util.List;
import java.util.Optional;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;

@JsonInclude(NON_DEFAULT)
public class EthernetSwitchPortLinks {
    @JsonProperty("PrimaryVLAN")
    private ODataId primaryVlan;

    @JsonProperty("PortMembers")
    private Optional<List<ODataId>> portMembers = empty();

    public void setRequestedPortMembers(Optional<List<URI>> uris) {
        if (uris == null) {
            portMembers = null;
        } else if (uris.isPresent()) {
            portMembers = of(uris.get().stream().map(ODataId::new).collect(toList()));
        } else {
            portMembers = empty();
        }
    }

    public void setPrimaryVlan(URI primaryVlanUri) {
        if (primaryVlanUri != null) {
            primaryVlan = new ODataId(primaryVlanUri);
        }
    }

    public Optional<List<ODataId>> getPortMembers() {
        return portMembers;
    }

    public ODataId getPrimaryVlan() {
        return primaryVlan;
    }
}
