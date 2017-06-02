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
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.PowerInputType;
import com.intel.podm.common.types.PowerLimitException;
import com.intel.podm.common.types.PowerSupplyType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;
import com.intel.podm.redfish.json.templates.attributes.RedundancyJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name",
    "description", "powerControl", "voltages", "powerSupplies", "redundancy"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class PowerJson extends BaseResourceJson {
    public List<PowerControlJson> powerControl = new LinkedList<>();
    public List<PowerVoltageJson> voltages = new LinkedList<>();
    public List<PowerSupplyJson> powerSupplies = new LinkedList<>();
    public List<RedundancyJson> redundancy = new LinkedList<>();

    public PowerJson() {
        super("Power.v1_1_0.Power");
    }

    @JsonPropertyOrder({
        "@odata.id", "memberId", "name", "powerConsumedWatts", "powerRequestedWatts", "powerAvailableWatts", "powerCapacityWatts", "powerAllocatedWatts",
        "powerMetrics", "powerLimit", "relatedItem", "status", "oem"
    })
    public static class PowerControlJson {
        @JsonUnwrapped
        @JsonProperty("@odata.id")
        public ODataId oDataId;
        public String memberId;
        public String name;
        public Integer powerRequestedWatts;
        public Integer powerAvailableWatts;
        public Integer powerCapacityWatts;
        public Integer powerAllocatedWatts;
        public Status status;
        public List<ODataId> relatedItem = new LinkedList<>();
        public PowerLimitJson powerLimit;
        public PowerMetricsJson powerMetrics;
        @IgnoreAutomaticOem
        public JsonNode oem;
        public Integer powerConsumedWatts;
    }

    @JsonPropertyOrder({
        "oDataId", "memberId", "name", "sensorNumber", "status",
        "readingVolts", "upperThresholdNonCritical", "upperThresholdCritical", "upperThresholdFatal",
        "lowerThresholdNonCritical", "lowerThresholdCritical", "lowerThresholdFatal", "minReadingRange",
        "maxReadingRange", "physicalContext", "relatedItem"
    })
    public static class PowerVoltageJson {
        @JsonUnwrapped
        @JsonProperty("@odata.id")
        public ODataId oDataId;
        public String memberId;
        public String name;
        public Integer sensorNumber;
        public Status status;
        public BigDecimal readingVolts;
        public BigDecimal upperThresholdNonCritical;
        public BigDecimal upperThresholdCritical;
        public BigDecimal upperThresholdFatal;
        public BigDecimal lowerThresholdNonCritical;
        public BigDecimal lowerThresholdCritical;
        public BigDecimal lowerThresholdFatal;
        public BigDecimal minReadingRange;
        public BigDecimal maxReadingRange;
        public PhysicalContext physicalContext;
        public List<ODataId> relatedItem = new LinkedList<>();
    }

    @JsonPropertyOrder({
        "oDataId", "memberId", "name", "status", "oem", "powerSupplyType",
        "lineInputVoltageType", "lineInputVoltage", "powerCapacityWatts", "lastPowerOutputWatts",
        "model", "manufacturer", "firmwareVersion", "serialNumber", "partNumber",
        "sparePartNumber", "inputRanges", "indicatorLed", "redundancy", "relatedItem"
    })
    public static class PowerSupplyJson {
        @JsonUnwrapped
        @JsonProperty("@odata.id")
        public ODataId oDataId;
        public String memberId;
        public String name;
        public Status status;
        @IgnoreAutomaticOem
        public JsonNode oem;
        public PowerSupplyType powerSupplyType;
        public LineInputVoltageType lineInputVoltageType;
        public BigDecimal lineInputVoltage;
        public BigDecimal powerCapacityWatts;
        public BigDecimal lastPowerOutputWatts;
        public String model;
        public String manufacturer;
        public String firmwareVersion;
        public String serialNumber;
        public String partNumber;
        public String sparePartNumber;
        public List<InputRangeJson> inputRanges = new ArrayList<>();
        public IndicatorLed indicatorLed;
        public List<ODataId> redundancy = new LinkedList<>();
        public List<ODataId> relatedItem = new LinkedList<>();
    }

    @JsonPropertyOrder({
        "inputType", "minimumVoltage", "maximumVoltage", "minimumFrequencyHz",
        "maximumFrequencyHz", "outputWattage", "oem"
    })
    public static class InputRangeJson {
        public PowerInputType inputType;
        public BigDecimal minimumVoltage;
        public BigDecimal maximumVoltage;
        public BigDecimal minimumFrequencyHz;
        public BigDecimal maximumFrequencyHz;
        public BigDecimal outputWattage;
        @IgnoreAutomaticOem
        public JsonNode oem;
    }

    @JsonPropertyOrder({
        "limitInWatts", "limitException", "correctionInMs"
    })
    public static class PowerLimitJson {
        public Integer correctionInMs;
        public PowerLimitException limitException;
        public Integer limitInWatts;
    }

    @JsonPropertyOrder({
        "intervalInMin", "minConsumedWatts", "maxConsumedWatts", "averageConsumedWatts"
    })

    public static class PowerMetricsJson {
        public Integer averageConsumedWatts;
        public Integer intervalInMin;
        public Integer maxConsumedWatts;
        public Integer minConsumedWatts;
    }

}
