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
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.RedundancyJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name",
    "description", "temperatures", "fans", "redundancy"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ThermalJson extends BaseResourceJson {
    public List<ThermalTemperatureJson> temperatures = new LinkedList<>();
    public List<ThermalFanJson> fans = new LinkedList<>();
    public Set<RedundancyJson> redundancy = new LinkedHashSet<>();

    public ThermalJson() {
        super("Thermal.v1_1_0.Thermal");
    }

    @JsonPropertyOrder({
        "oDataId", "memberId", "name", "sensorNumber", "status",
        "readingCelsius", "upperThresholdNonCritical", "upperThresholdCritical", "upperThresholdFatal",
        "lowerThresholdNonCritical", "lowerThresholdCritical", "lowerThresholdFatal", "minReadingRange",
        "maxReadingRange", "physicalContext", "relatedItem"
    })
    @SuppressWarnings({"checkstyle:VisibilityModifier"})
    public static class ThermalTemperatureJson {
        @JsonProperty("@odata.id")
        @JsonUnwrapped
        public ODataId oDataId;
        public String memberId;
        public String name;
        public Integer sensorNumber;
        public Status status;
        public BigDecimal readingCelsius;
        public BigDecimal upperThresholdNonCritical;
        public BigDecimal upperThresholdCritical;
        public BigDecimal upperThresholdFatal;
        public BigDecimal lowerThresholdNonCritical;
        public BigDecimal lowerThresholdCritical;
        public BigDecimal lowerThresholdFatal;
        public BigDecimal minReadingRange;
        public BigDecimal maxReadingRange;
        @JsonUnwrapped
        public PhysicalContext physicalContext;
        public Set<ODataId> relatedItem = new HashSet<>();
    }

    @JsonPropertyOrder({
        "oDataId", "memberId", "name", "physicalContext", "status", "reading", "readingUnits",
        "upperThresholdNonCritical", "upperThresholdCritical", "upperThresholdFatal", "lowerThresholdNonCritical",
        "lowerThresholdCritical", "lowerThresholdFatal", "minReadingRange", "maxReadingRange", "redundancy",
        "relatedItem"
    })
    @SuppressWarnings({"checkstyle:VisibilityModifier"})
    public static class ThermalFanJson {
        @JsonProperty("@odata.id")
        @JsonUnwrapped
        public ODataId oDataId;
        public String memberId;
        public String name;
        public PhysicalContext physicalContext;
        public Status status;
        public Integer reading;
        public String readingUnits;
        public Integer upperThresholdNonCritical;
        public Integer upperThresholdCritical;
        public Integer upperThresholdFatal;
        public Integer lowerThresholdNonCritical;
        public Integer lowerThresholdCritical;
        public Integer lowerThresholdFatal;
        public Integer minReadingRange;
        public Integer maxReadingRange;
        public Set<ODataId> redundancy = new HashSet<>();
        public Set<ODataId> relatedItem = new HashSet<>();
    }
}
