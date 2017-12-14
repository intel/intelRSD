/*
 * Copyright (c) 2017 Intel Corporation
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

import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({"ThermalResource\\.Fan",
    "#Thermal" + VERSION_PATTERN + "Fan"})
@SuppressWarnings({"checkstyle:MethodCount"})
public class ThermalFanItem implements ExternalServiceResource, ReferenceableMember {
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

    public String getName() {
        return name;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getReading() {
        return reading;
    }

    public String getReadingUnits() {
        return readingUnits;
    }

    public Integer getUpperThresholdNonCritical() {
        return upperThresholdNonCritical;
    }

    public Integer getUpperThresholdCritical() {
        return upperThresholdCritical;
    }

    public Integer getUpperThresholdFatal() {
        return upperThresholdFatal;
    }

    public Integer getLowerThresholdNonCritical() {
        return lowerThresholdNonCritical;
    }

    public Integer getLowerThresholdCritical() {
        return lowerThresholdCritical;
    }

    public Integer getLowerThresholdFatal() {
        return lowerThresholdFatal;
    }

    public Integer getMinReadingRange() {
        return minReadingRange;
    }

    public Integer getMaxReadingRange() {
        return maxReadingRange;
    }

    @LinkName("relatedItems")
    public Iterable<ResourceSupplier> relatedItems() throws WebClientRequestException {
        return toSuppliers(webClient, relatedItems);
    }

    @LinkName("fanRedundancy")
    public Iterable<ResourceSupplier> redundancy() throws WebClientRequestException {
        return toSuppliers(webClient, redundancy);
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

    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }
}
