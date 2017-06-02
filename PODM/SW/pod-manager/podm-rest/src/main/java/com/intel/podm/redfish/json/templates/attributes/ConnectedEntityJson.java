/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.redfish.json.templates.attributes;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.EntityType;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.Set;

@JsonPropertyOrder({
    "entityType", "entityRole", "pciFunctionNumber", "pciClassCode", "entityPciId", "entityLink", "identifiers", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ConnectedEntityJson {
    @JsonProperty("EntityType")
    public EntityType entityType;
    @JsonProperty("EntityRole")
    public EntityRole entityRole;
    @JsonProperty("PciFunctionNumber")
    public Integer pciFunctionNumber;
    @JsonProperty("PciClassCode")
    public String pciClassCode;
    @JsonProperty("EntityPciId")
    public PciIdJson entityPciId;
    @JsonProperty("Identifiers")
    public Set<IdentifierJson> identifiers;
    @JsonProperty("EntityLink")
    public ODataId entityLink;
}
