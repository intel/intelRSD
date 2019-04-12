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

package com.intel.rsd.nodecomposer.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;

import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataContextProvider.getContextFromODataType;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromString;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "name", "Members@odata.count", "members", "actions"})
public class ComposedNodeCollectionDto {
    @JsonProperty("@odata.type")
    private final String oDataType = "#ComposedNodeCollection.ComposedNodeCollection";

    @JsonProperty("@odata.context")
    private final URI oDataContext = getContextFromODataType(oDataType);

    @JsonUnwrapped
    private final ODataId oDataId = oDataIdFromString("/redfish/v1/Nodes");

    @JsonProperty("Name")
    private final String name = "Composed Node Collection";

    @JsonProperty("Members")
    private final List<ODataId> members = new ArrayList<>();

    @JsonProperty("Actions")
    private final Actions actions = new Actions();

    public ComposedNodeCollectionDto(Collection<ODataId> uris) {
        members.addAll(uris);
    }

    @JsonProperty("Members@odata.count")
    private int getCount() {
        return members.size();
    }

    public final class Actions {
        @JsonProperty("#ComposedNodeCollection.Allocate")
        private final AllocateAction allocate = new AllocateAction();

        public final class AllocateAction {
            @JsonProperty("target")
            private final String target = oDataId + "/Actions/Allocate";
        }
    }
}
