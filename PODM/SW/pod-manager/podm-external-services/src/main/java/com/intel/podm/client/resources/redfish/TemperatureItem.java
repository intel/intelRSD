/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.client.ReferenceableMember;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceLinks;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

@OdataTypes({
    "ThermalResource\\.Temperature"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class TemperatureItem implements ExternalServiceResource, ReferenceableMember {
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
    @JsonProperty("MinReadingRangeTemp")
    private BigDecimal minReadingRangeTemp;
    @JsonProperty("MaxReadingRangeTemp")
    private BigDecimal maxReadingRangeTemp;
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

    public String getName() {
        return name;
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }

    public Integer getSensorNumber() {
        return sensorNumber;
    }

    public Status getStatus() {
        return status;
    }

    public BigDecimal getReadingCelsius() {
        return readingCelsius;
    }

    public BigDecimal getUpperThresholdNonCritical() {
        return upperThresholdNonCritical;
    }

    public BigDecimal getUpperThresholdCritical() {
        return upperThresholdCritical;
    }

    public BigDecimal getUpperThresholdFatal() {
        return upperThresholdFatal;
    }

    public BigDecimal getLowerThresholdNonCritical() {
        return lowerThresholdNonCritical;
    }

    public BigDecimal getLowerThresholdCritical() {
        return lowerThresholdCritical;
    }

    public BigDecimal getLowerThresholdFatal() {
        return lowerThresholdFatal;
    }

    public BigDecimal getMinReadingRangeTemp() {
        return minReadingRangeTemp;
    }

    public BigDecimal getMaxReadingRangeTemp() {
        return maxReadingRangeTemp;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    @LinkName("relatedItems")
    public Iterable<ResourceSupplier> relatedItems() throws WebClientRequestException {
        return toSuppliers(webClient, relatedItems);
    }
}
