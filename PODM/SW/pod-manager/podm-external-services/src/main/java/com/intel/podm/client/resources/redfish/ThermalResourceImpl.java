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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.ReferenceableMember;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.api.resources.redfish.ThermalResource;
import com.intel.podm.client.reader.ResourceLinksImpl;
import com.intel.podm.client.reader.ResourceSupplierImpl;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.function.Predicate;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.intel.podm.common.utils.Collections.firstByPredicate;
import static java.util.stream.Collectors.toList;

@OdataTypes({
        "#Thermal" + OdataTypes.VERSION_PATTERN + "Thermal"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class ThermalResourceImpl extends ExternalServiceResourceImpl implements ThermalResource {

    @JsonProperty("Temperatures")
    List<Temperature> temperatures = new ArrayList<>();
    @JsonProperty("Fans")
    List<Fan> fans = new ArrayList<>();
    @JsonProperty("Redundancy")
    List<RedundancyObjectImpl> redundancies = new ArrayList<>();

    @Override
    public ExternalServiceResource getByFragment(String uriFragment) {
        Pattern pattern = Pattern.compile("/?(?<name>\\w+)/(?<id>\\d+)");
        Matcher matcher = pattern.matcher(uriFragment);

        if (matcher.matches()) {
            String memberId = matcher.group("id");
            String name = matcher.group("name");
            Predicate<ReferenceableMember> withTheSameMemberId = r -> Objects.equals(r.getMemberId(), memberId);

            switch (name) {
                case "Redundancy":
                    return firstByPredicate(redundancies, withTheSameMemberId).orElse(null);
                case "Fans":
                    return firstByPredicate(fans, withTheSameMemberId).orElse(null);
                case "Temperatures":
                    return firstByPredicate(temperatures, withTheSameMemberId).orElse(null);
                default:
            }
        }

        return null;
    }

    @Override
    @LinkName("temperatures")
    public Iterable<ResourceSupplier> getTemperatures() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(temperatures);
    }

    @Override
    @LinkName("fans")
    public Iterable<ResourceSupplier> getFans() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(fans);
    }

    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancies() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(redundancies);
    }

    @OdataTypes({"ThermalResource\\.Fan"})
    public static class Fan implements ThermalResource.Fan {
        private WebClient webClient;
        @JsonProperty("@odata.id")
        private String oDataId;
        @JsonProperty("MemberId")
        private String memberId;
        @JsonProperty("Name")
        private String name;
        @JsonProperty("PhysicalContext")
        private PhysicalContext physicalContext;
        @JsonProperty("Status")
        private Status status;
        @JsonProperty("Reading")
        private Integer reading;
        @JsonProperty("ReadingUnits")
        private String readingUnits;
        @JsonProperty("UpperThresholdNonCritical")
        private Integer upperThresholdNonCritical;
        @JsonProperty("UpperThresholdCritical")
        private Integer upperThresholdCritical;
        @JsonProperty("UpperThresholdFatal")
        private Integer upperThresholdFatal;
        @JsonProperty("LowerThresholdNonCritical")
        private Integer lowerThresholdNonCritical;
        @JsonProperty("LowerThresholdCritical")
        private Integer lowerThresholdCritical;
        @JsonProperty("LowerThresholdFatal")
        private Integer lowerThresholdFatal;
        @JsonProperty("MinReadingRange")
        private Integer minReadingRange;
        @JsonProperty("MaxReadingRange")
        private Integer maxReadingRange;
        @JsonProperty("RelatedItem")
        private List<ODataId> relatedItems = new ArrayList<>();
        @JsonProperty("Redundancy")
        private List<ODataId> redundancy = new ArrayList<>();

        @Override
        public String getMemberId() {
            return memberId;
        }

        @Override
        public String getName() {
            return name;
        }

        @Override
        public PhysicalContext getPhysicalContext() {
            return physicalContext;
        }

        @Override
        public Status getStatus() {
            return status;
        }

        @Override
        public Integer getReading() {
            return reading;
        }

        @Override
        public String getReadingUnits() {
            return readingUnits;
        }

        @Override
        public Integer getUpperThresholdNonCritical() {
            return upperThresholdNonCritical;
        }

        @Override
        public Integer getUpperThresholdCritical() {
            return upperThresholdCritical;
        }

        @Override
        public Integer getUpperThresholdFatal() {
            return upperThresholdFatal;
        }

        @Override
        public Integer getLowerThresholdNonCritical() {
            return lowerThresholdNonCritical;
        }

        @Override
        public Integer getLowerThresholdCritical() {
            return lowerThresholdCritical;
        }

        @Override
        public Integer getLowerThresholdFatal() {
            return lowerThresholdFatal;
        }

        @Override
        public Integer getMinReadingRange() {
            return minReadingRange;
        }

        @Override
        public Integer getMaxReadingRange() {
            return maxReadingRange;
        }

        @Override
        @LinkName("relatedItems")
        public Iterable<ResourceSupplier> relatedItems() throws ExternalServiceApiReaderException {
            return relatedItems.stream()
                    .map(ri -> new ResourceSupplierImpl(webClient, ri.toUri()))
                    .collect(toList());
        }

        @Override
        @LinkName("fanRedundancy")
        public Iterable<ResourceSupplier> redundancy() throws ExternalServiceApiReaderException {
            return redundancy.stream()
                    .map(ri -> new ResourceSupplierImpl(webClient, ri.toUri()))
                    .collect(toList());
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
        public Links getLinks() {
            return new ResourceLinksImpl(this);
        }
    }

    @OdataTypes({
            "ThermalResource\\.Temperature"
    })
    public static class Temperature implements ThermalResource.Temperature {
        private WebClient webClient;
        @JsonProperty("@odata.id")
        private String oDataId;
        @JsonProperty("MemberId")
        private String memberId;
        @JsonProperty("Name")
        private String name;
        @JsonProperty("SensorNumber")
        private Integer sensorNumber;
        @JsonProperty("Status")
        private Status status;
        @JsonProperty("ReadingCelsius")
        private BigDecimal readingCelsius;
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
        public String getMemberId() {
            return memberId;
        }

        @Override
        public String getName() {
            return name;
        }

        @Override
        public Links getLinks() {
            return new ResourceLinksImpl(this);
        }

        @Override
        public Integer getSensorNumber() {
            return sensorNumber;
        }

        @Override
        public Status getStatus() {
            return status;
        }

        @Override
        public BigDecimal getReadingCelsius() {
            return readingCelsius;
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
    }
}
