/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.business.dto.RedfishDto;
import com.intel.podm.common.types.redfish.OdataIdProvider;

import java.net.URI;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromODataType;

@JsonPropertyOrder({"@odata.context", "@odata.id"})
public class RedfishResourceAmazingWrapper {
    @JsonProperty("@odata.context")
    private final URI odataContext;

    @JsonProperty("@odata.id")
    private final URI oDataId;

    @JsonProperty
    @JsonUnwrapped
    private final RedfishDto baseDto;

    public RedfishResourceAmazingWrapper(OdataIdProvider odataIdProvider, RedfishDto redfishResource) {
        OdataIdProvider.ODataId oDataId = odataIdProvider.asOdataId();
        this.oDataId = oDataId.toUri();
        this.odataContext = getContextFromODataType(redfishResource.getoDataType());
        this.baseDto = redfishResource;
    }
}
