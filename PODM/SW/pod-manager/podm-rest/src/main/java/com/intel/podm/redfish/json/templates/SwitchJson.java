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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.ResetActionJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "switchType", "model",
    "manufacturer", "serialNumber", "partNumber", "assetTag", "domainId", "isManaged", "totalSwitchWidth",
    "indicatorLed", "powerState", "ports", "redundancy", "links", "actions", "oem", "logServices"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class SwitchJson extends BaseResourceJson {
    public Status status;
    public Protocol switchType;
    public String model;
    public String manufacturer;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    public String assetTag;
    @JsonProperty("DomainID")
    public Integer domainId;
    public Boolean isManaged;
    public Integer totalSwitchWidth;
    @JsonProperty("IndicatorLED")
    public IndicatorLed indicatorLed;
    public PowerState powerState;
    public ODataId ports;
    public List<Object> redundancy = new ArrayList();
    public Links links = new Links();
    public Actions actions = new Actions();

    public SwitchJson() {
        super("#Switch.v1_0_0.Switch");
    }

    @JsonPropertyOrder({"chassis", "pcieFunctions", "oem"})
    public class Links extends RedfishLinksJson {
        public Set<ODataId> chassis = new HashSet<>();
        public Set<ODataId> managedBy = new HashSet<>();
    }

    public class Actions extends RedfishActionsJson {
        @JsonProperty("#Switch.Reset")
        public ResetActionJson reset = new ResetActionJson();
    }
}
