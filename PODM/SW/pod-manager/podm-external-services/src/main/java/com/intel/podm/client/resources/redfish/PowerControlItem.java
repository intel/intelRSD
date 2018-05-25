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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.ReferenceableMember;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceLinks;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.PowerLimitException;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.client.resources.UnknownOemsHelper.convertJsonNodeToString;

@OdataTypes({"PowerResource\\.PowerControl"})
@SuppressWarnings({"checkstyle:MethodCount"})
public class PowerControlItem implements ExternalServiceResource, ReferenceableMember {
    @JsonIgnore
    private WebClient webClient;

    @JsonProperty("@odata.id")
    private String oDataId;

    @JsonProperty("MemberId")
    private String memberId;

    @JsonProperty("Name")
    private String name;

    @JsonProperty("PowerConsumedWatts")
    private BigDecimal powerConsumedWatts;

    @JsonProperty("PowerRequestedWatts")
    private BigDecimal powerRequestedWatts;

    @JsonProperty("PowerAvailableWatts")
    private BigDecimal powerAvailableWatts;

    @JsonProperty("PowerCapacityWatts")
    private BigDecimal powerCapacityWatts;

    @JsonProperty("PowerAllocatedWatts")
    private BigDecimal powerAllocatedWatts;

    @JsonProperty("PowerMetrics")
    private PowerMetricsObject powerMetrics;

    @JsonProperty("PowerLimit")
    private PowerLimitObject powerLimit;

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

    public String getName() {
        return name;
    }

    public BigDecimal getPowerConsumedWatts() {
        return powerConsumedWatts;
    }

    public Status getStatus() {
        return status;
    }

    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
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

    public BigDecimal getPowerRequestedWatts() {
        return powerRequestedWatts;
    }

    public BigDecimal getPowerAvailableWatts() {
        return powerAvailableWatts;
    }

    public BigDecimal getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public BigDecimal getPowerAllocatedWatts() {
        return powerAllocatedWatts;
    }

    public PowerMetricsObject getPowerMetrics() {
        return powerMetrics;
    }

    public PowerLimitObject getPowerLimit() {
        return powerLimit;
    }

    @LinkName("relatedItems")
    public Iterable<ResourceSupplier> relatedItems() throws WebClientRequestException {
        return toSuppliers(webClient, relatedItems);
    }

    public String getOem() {
        return oem;
    }

    @JsonProperty("Oem")
    public void setOem(JsonNode jsonNode) throws JsonProcessingException {
        oem = convertJsonNodeToString(jsonNode);
    }

    public static class PowerMetricsObject {
        @JsonProperty("IntervalInMin")
        private Integer intervalInMin;

        @JsonProperty("MinConsumedWatts")
        private BigDecimal minConsumedWatts;

        @JsonProperty("MaxConsumedWatts")
        private BigDecimal maxConsumedWatts;

        @JsonProperty("AverageConsumedWatts")
        private BigDecimal averageConsumedWatts;

        public Integer getIntervalInMin() {
            return intervalInMin;
        }

        public BigDecimal getMinConsumedWatts() {
            return minConsumedWatts;
        }

        public BigDecimal getMaxConsumedWatts() {
            return maxConsumedWatts;
        }

        public BigDecimal getAverageConsumedWatts() {
            return averageConsumedWatts;
        }
    }

    public static class PowerLimitObject {
        @JsonProperty("LimitInWatts")
        private Integer limitInWatts;

        @JsonProperty("LimitException")
        private PowerLimitException limitException;

        @JsonProperty("CorrectionInMs")
        private Integer correctionInMs;

        public Integer getLimitInWatts() {
            return limitInWatts;
        }

        public PowerLimitException getLimitException() {
            return limitException;
        }

        public Integer getCorrectionInMs() {
            return correctionInMs;
        }
    }
}
