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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.ReferenceableMember;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.api.resources.redfish.PowerResource;
import com.intel.podm.client.reader.ResourceLinksImpl;
import com.intel.podm.client.reader.ResourceSupplierImpl;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.PowerInputType;
import com.intel.podm.common.types.PowerLimitException;
import com.intel.podm.common.types.PowerSupplyType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.regex.Matcher;

import static com.intel.podm.client.resources.UnknownOemsHelper.convertJsonNodeToString;
import static com.intel.podm.common.utils.Collections.firstByPredicate;
import static java.util.stream.Collectors.toList;

@OdataTypes({
    "#Power" + OdataTypes.VERSION_PATTERN + "Power"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class PowerResourceImpl extends ExternalServiceResourceImpl implements PowerResource {
    @JsonProperty("PowerControl")
    List<PowerControl> powerControls = new ArrayList<>();

    @JsonProperty("Voltages")
    List<Voltage> voltages = new ArrayList<>();

    @JsonProperty("PowerSupplies")
    List<PowerSupply> powerSupplies = new ArrayList<>();

    @JsonProperty("Redundancy")
    List<RedundancyObjectImpl> redundancy = new ArrayList<>();

    @Override
    @LinkName("powerControls")
    public Iterable<ResourceSupplier> getPowerControls() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(powerControls);
    }

    @Override
    @LinkName("voltages")
    public Iterable<ResourceSupplier> getVoltages() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(voltages);
    }

    @Override
    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancy() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(redundancy);
    }

    @Override
    @LinkName("powerSupplies")
    public Iterable<ResourceSupplier> getPowerSupplies() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(powerSupplies);
    }

    @Override
    @SuppressWarnings({"checkstyle:ReturnCount"})
    public ExternalServiceResource getByFragment(String uriFragment) {
        Matcher matcher = createMatcher(uriFragment, "name", "id");
        if (matcher.matches()) {
            Predicate<ReferenceableMember> withTheSameMemberId = r -> Objects.equals(r.getMemberId(), matcher.group("id"));
            switch (matcher.group("name")) {
                case "Voltages":
                    return firstByPredicate(voltages, withTheSameMemberId).orElse(null);
                case "Redundancy":
                    return firstByPredicate(redundancy, withTheSameMemberId).orElse(null);
                case "PowerControl":
                    return firstByPredicate(powerControls, withTheSameMemberId).orElse(null);
                case "PowerSupplies":
                    return firstByPredicate(powerSupplies, withTheSameMemberId).orElse(null);
                default:
            }
        }

        return null;
    }

    @OdataTypes({"PowerResource\\.PowerControl"})
    @SuppressWarnings({"checkstyle:MethodCount"})
    public static class PowerControl implements PowerResource.PowerControl {
        @JsonIgnore
        private WebClient webClient;

        @JsonProperty("@odata.id")
        private String oDataId;

        @JsonProperty("MemberId")
        private String memberId;

        @JsonProperty("Name")
        private String name;

        @JsonProperty("PowerConsumedWatts")
        private Integer powerConsumedWatts;

        @JsonProperty("PowerRequestedWatts")
        private Integer powerRequestedWatts;

        @JsonProperty("PowerAvailableWatts")
        private Integer powerAvailableWatts;

        @JsonProperty("PowerCapacityWatts")
        private Integer powerCapacityWatts;

        @JsonProperty("PowerAllocatedWatts")
        private Integer powerAllocatedWatts;

        @JsonProperty("PowerMetrics")
        private PowerMetrics powerMetrics;

        @JsonProperty("PowerLimit")
        private PowerLimit powerLimit;

        @JsonProperty("RelatedItem")
        private List<ODataId> relatedItems = new ArrayList<>();

        @JsonProperty("Status")
        private Status status;

        @IgnoreAutomaticOem
        private String oem;

        public String getoDataId() {
            return oDataId;
        }

        @Override
        public String getMemberId() {
            return memberId;
        }

        @Override
        public String getName() {
            return name;
        }

        @Override
        public Integer getPowerConsumedWatts() {
            return powerConsumedWatts;
        }

        @Override
        public Status getStatus() {
            return status;
        }

        @Override
        public Links getLinks() {
            return new ResourceLinksImpl(this);
        }

        @Override
        public URI getUri() {
            return URI.create(oDataId);
        }

        @Override
        public void setUri(URI uri) {
        }

        @Override
        public void setWebClient(WebClient webClient) {
            this.webClient = webClient;
        }

        @Override
        public Integer getPowerRequestedWatts() {
            return powerRequestedWatts;
        }

        @Override
        public Integer getPowerAvailableWatts() {
            return powerAvailableWatts;
        }

        @Override
        public Integer getPowerCapacityWatts() {
            return powerCapacityWatts;
        }

        @Override
        public Integer getPowerAllocatedWatts() {
            return powerAllocatedWatts;
        }

        @Override
        public PowerMetrics getPowerMetrics() {
            return powerMetrics;
        }

        @Override
        public PowerLimit getPowerLimit() {
            return powerLimit;
        }

        @Override
        @LinkName("relatedItems")
        public Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException {
            return relatedItems.stream()
                .map(ri -> new ResourceSupplierImpl(webClient, ri.toUri()))
                .collect(toList());
        }

        @Override
        public String getOem() {
            return oem;
        }

        @JsonProperty("Oem")
        public void setOem(JsonNode jsonNode) throws JsonProcessingException {
            oem = convertJsonNodeToString(jsonNode);
        }

        private static class PowerMetrics implements PowerResource.PowerControl.PowerMetrics {
            @JsonProperty("IntervalInMin")
            private Integer intervalInMin;

            @JsonProperty("MinConsumedWatts")
            private Integer minConsumedWatts;

            @JsonProperty("MaxConsumedWatts")
            private Integer maxConsumedWatts;

            @JsonProperty("AverageConsumedWatts")
            private Integer averageConsumedWatts;

            @Override
            public Integer getIntervalInMin() {
                return intervalInMin;
            }

            @Override
            public Integer getMinConsumedWatts() {
                return minConsumedWatts;
            }

            @Override
            public Integer getMaxConsumedWatts() {
                return maxConsumedWatts;
            }

            @Override
            public Integer getAverageConsumedWatts() {
                return averageConsumedWatts;
            }
        }

        private static class PowerLimit implements PowerResource.PowerControl.PowerLimit {
            @JsonProperty("LimitInWatts")
            private Integer limitInWatts;

            @JsonProperty("LimitException")
            private PowerLimitException limitException;

            @JsonProperty("CorrectionInMs")
            private Integer correctionInMs;

            @Override
            public Integer getLimitInWatts() {
                return limitInWatts;
            }

            @Override
            public PowerLimitException getLimitException() {
                return limitException;
            }

            @Override
            public Integer getCorrectionInMs() {
                return correctionInMs;
            }
        }
    }

    @OdataTypes({"PowerResource\\.Voltage"})
    public static class Voltage implements PowerResource.Voltage {
        @JsonIgnore
        private WebClient webClient;

        @JsonProperty("@odata.id")
        private String oDataId;

        @JsonProperty("MemberId")
        private String memberId;

        @JsonProperty("Name")
        private String name;

        @JsonProperty("SensorNumber")
        private Long sensorNumber;

        @JsonProperty("Status")
        private Status status;

        @JsonProperty("ReadingVolts")
        private BigDecimal readingVolts;

        @JsonProperty("UpperThresholdNonCritical")
        private BigDecimal upperThresholdNonCritical;

        @JsonProperty("UpperThresholdCritical")
        private BigDecimal upperThresholdCritical;

        @JsonProperty("UpperThresholdFatal")
        private BigDecimal upperThresholdFatal;

        @JsonProperty("LowerThresholdNonCritical")
        private BigDecimal lowerThresholdNonCritical;

        @JsonProperty("LowerThresholdCritical")
        private BigDecimal lowerThresholdCritical;

        @JsonProperty("LowerThresholdFatal")
        private BigDecimal lowerThresholdFatal;

        @JsonProperty("MinReadingRange")
        private BigDecimal minReadingRange;

        @JsonProperty("MaxReadingRange")
        private BigDecimal maxReadingRange;

        @JsonProperty("PhysicalContext")
        private PhysicalContext physicalContext;

        @JsonProperty("RelatedItem")
        private List<ODataId> relatedItems = new ArrayList<>();

        @Override
        public String getMemberId() {
            return memberId;
        }

        @Override
        public String getName() {
            return name;
        }

        @Override
        public Long getSensorNumber() {
            return sensorNumber;
        }

        @Override
        public Status getStatus() {
            return status;
        }

        @Override
        public BigDecimal getReadingVolts() {
            return readingVolts;
        }

        @Override
        public BigDecimal getUpperThresholdNonCritical() {
            return upperThresholdNonCritical;
        }

        @Override
        public BigDecimal getUpperThresholdCritical() {
            return upperThresholdCritical;
        }

        @Override
        public BigDecimal getUpperThresholdFatal() {
            return upperThresholdFatal;
        }

        @Override
        public BigDecimal getLowerThresholdNonCritical() {
            return lowerThresholdNonCritical;
        }

        @Override
        public BigDecimal getLowerThresholdCritical() {
            return lowerThresholdCritical;
        }

        @Override
        public BigDecimal getLowerThresholdFatal() {
            return lowerThresholdFatal;
        }

        @Override
        public BigDecimal getMinReadingRange() {
            return minReadingRange;
        }

        @Override
        public BigDecimal getMaxReadingRange() {
            return maxReadingRange;
        }

        @Override
        public PhysicalContext getPhysicalContext() {
            return physicalContext;
        }

        @Override
        @LinkName("relatedItems")
        public Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException {
            return relatedItems.stream()
                .map(ri -> new ResourceSupplierImpl(webClient, ri.toUri()))
                .collect(toList());
        }

        @Override
        public Links getLinks() {
            return new ResourceLinksImpl(this);
        }

        @Override
        public URI getUri() {
            return URI.create(oDataId);
        }

        @Override
        public void setUri(URI uri) {

        }

        @Override
        public void setWebClient(WebClient webClient) {
            this.webClient = webClient;
        }
    }


    @SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
    @OdataTypes({"PowerResource\\.PowerSupply"})
    public static class PowerSupply implements PowerResource.PowerSupply {
        @JsonIgnore
        private WebClient webClient;

        @JsonProperty("@odata.id")
        private String oDataId;

        @JsonProperty("MemberId")
        private String memberId;

        @JsonProperty("Name")
        private String name;

        @JsonProperty("Status")
        private Status status;

        @IgnoreAutomaticOem
        private String oem;

        @JsonProperty("PowerSupplyType")
        private PowerSupplyType powerSupplyType;

        @JsonProperty("LineInputVoltageType")
        private LineInputVoltageType lineInputVoltageType;

        @JsonProperty("LineInputVoltage")
        private BigDecimal lineInputVoltage;

        @JsonProperty("PowerCapacityWatts")
        private BigDecimal powerCapacityWatts;

        @JsonProperty("LastPowerOutputWatts")
        private BigDecimal lastPowerOutputWatts;

        @JsonProperty("Model")
        private String model;

        @JsonProperty("Manufacturer")
        private String manufacturer;

        @JsonProperty("FirmwareVersion")
        private String firmwareVersion;

        @JsonProperty("SerialNumber")
        private String serialNumber;

        @JsonProperty("PartNumber")
        private String partNumber;

        @JsonProperty("SparePartNumber")
        private String sparePartNumber;

        @JsonProperty("InputRanges")
        private List<InputRange> inputRanges = new ArrayList<>();

        @JsonProperty("IndicatorLED")
        private IndicatorLed indicatorLed;

        @JsonProperty("Redundancy")
        private List<ODataId> redundancy = new ArrayList<>();

        @JsonProperty("RelatedItem")
        private List<ODataId> relatedItems = new ArrayList<>();

        @Override
        public String getMemberId() {
            return memberId;
        }

        @Override
        public String getName() {
            return name;
        }

        @Override
        public Status getStatus() {
            return status;
        }

        @Override
        public String getOem() {
            return oem;
        }

        @JsonProperty("Oem")
        public void setOem(JsonNode jsonNode) throws JsonProcessingException {
            oem = convertJsonNodeToString(jsonNode);
        }

        @Override
        public PowerSupplyType getPowerSupplyType() {
            return powerSupplyType;
        }

        @Override
        public LineInputVoltageType getLineInputVoltageType() {
            return lineInputVoltageType;
        }

        @Override
        public BigDecimal getLineInputVoltage() {
            return lineInputVoltage;
        }

        @Override
        public BigDecimal getPowerCapacityWatts() {
            return powerCapacityWatts;
        }

        @Override
        public BigDecimal getLastPowerOutputWatts() {
            return lastPowerOutputWatts;
        }

        @Override
        public String getModel() {
            return model;
        }

        @Override
        public String getManufacturer() {
            return manufacturer;
        }

        @Override
        public String getFirmwareVersion() {
            return firmwareVersion;
        }

        @Override
        public String getSerialNumber() {
            return serialNumber;
        }

        @Override
        public String getPartNumber() {
            return partNumber;
        }

        @Override
        public String getSparePartNumber() {
            return sparePartNumber;
        }

        @Override
        public List<PowerResource.PowerSupply.InputRange> getInputRanges() {
            return (List) inputRanges;
        }

        @Override
        public IndicatorLed getIndicatorLed() {
            return indicatorLed;
        }

        @Override
        @LinkName("powerSupplyRedundancy")
        public Iterable<ResourceSupplier> redundancy() throws ExternalServiceApiReaderException {
            return redundancy.stream()
                .map(ri -> new ResourceSupplierImpl(webClient, ri.toUri()))
                .collect(toList());
        }

        @Override
        @LinkName("relatedItems")
        public Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException {
            return relatedItems.stream()
                .map(ri -> new ResourceSupplierImpl(webClient, ri.toUri()))
                .collect(toList());
        }

        @Override
        public Links getLinks() {
            return new ResourceLinksImpl(this);
        }

        @Override
        public URI getUri() {
            return URI.create(oDataId);
        }

        @Override
        public void setUri(URI uri) {

        }

        @Override
        public void setWebClient(WebClient webClient) {
            this.webClient = webClient;
        }

        private static class InputRange implements PowerResource.PowerSupply.InputRange {
            @JsonProperty("InputType")
            private PowerInputType inputType;

            @JsonProperty("MinimumVoltage")
            private BigDecimal minimumVoltage;

            @JsonProperty("MaximumVoltage")
            private BigDecimal maximumVoltage;

            @JsonProperty("OutputWattage")
            private BigDecimal outputWattage;

            @JsonProperty("MinimumFrequencyHz")
            private BigDecimal minimumFrequencyHz;

            @JsonProperty("MaximumFrequencyHz")
            private BigDecimal maximumFrequencyHz;

            @IgnoreAutomaticOem
            private String oem;

            @Override
            public PowerInputType getInputType() {
                return inputType;
            }

            @Override
            public BigDecimal getMinimumVoltage() {
                return minimumVoltage;
            }

            @Override
            public BigDecimal getMaximumVoltage() {
                return maximumVoltage;
            }

            @Override
            public BigDecimal getOutputWattage() {
                return outputWattage;
            }

            @Override
            public BigDecimal getMinimumFrequencyHz() {
                return minimumFrequencyHz;
            }

            @Override
            public BigDecimal getMaximumFrequencyHz() {
                return maximumFrequencyHz;
            }

            @Override
            public String getOem() {
                return oem;
            }

            @JsonProperty("Oem")
            public void setOem(JsonNode jsonNode) throws JsonProcessingException {
                oem = convertJsonNodeToString(jsonNode);
            }
        }
    }
}
