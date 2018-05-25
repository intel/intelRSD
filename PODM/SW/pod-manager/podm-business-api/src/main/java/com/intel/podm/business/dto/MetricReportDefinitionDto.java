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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.MetricReportActionType;
import com.intel.podm.common.types.MetricReportType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.TransmitFormat;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "metricReportType", "metricReportDestination", "metricReportActions",
    "pollingIntervalMilliSeconds", "reportIntervalMilliSeconds", "transmitFormat", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class MetricReportDefinitionDto extends RedfishDto {
    private final Actions actions = new Actions();
    private final Links links = new Links();
    private Status status;
    private MetricReportType metricReportType;
    private String metricReportDestination;
    private Collection<MetricReportActionType> metricReportActions = new ArrayList<>();
    private BigDecimal pollingIntervalMilliSeconds;
    private BigDecimal reportIntervalMilliSeconds;
    private TransmitFormat transmitFormat;

    public MetricReportDefinitionDto() {
        super("#MetricReportDefinition.v1_0_0.MetricReportDefinition");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public MetricReportType getMetricReportType() {
        return metricReportType;
    }

    public void setMetricReportType(MetricReportType metricReportType) {
        this.metricReportType = metricReportType;
    }

    public String getMetricReportDestination() {
        return metricReportDestination;
    }

    public void setMetricReportDestination(String metricReportDestination) {
        this.metricReportDestination = metricReportDestination;
    }

    public Collection<MetricReportActionType> getMetricReportActions() {
        return metricReportActions;
    }

    public void setMetricReportActions(Collection<MetricReportActionType> metricReportActions) {
        this.metricReportActions = metricReportActions;
    }

    public BigDecimal getPollingIntervalMilliSeconds() {
        return pollingIntervalMilliSeconds;
    }

    public void setPollingIntervalMilliSeconds(BigDecimal pollingIntervalMilliSeconds) {
        this.pollingIntervalMilliSeconds = pollingIntervalMilliSeconds;
    }

    public BigDecimal getReportIntervalMilliSeconds() {
        return reportIntervalMilliSeconds;
    }

    public void setReportIntervalMilliSeconds(BigDecimal reportIntervalMilliSeconds) {
        this.reportIntervalMilliSeconds = reportIntervalMilliSeconds;
    }

    public TransmitFormat getTransmitFormat() {
        return transmitFormat;
    }

    public void setTransmitFormat(TransmitFormat transmitFormat) {
        this.transmitFormat = transmitFormat;
    }

    public Actions getActions() {
        return actions;
    }

    public Links getLinks() {
        return links;
    }

    @JsonPropertyOrder({"metricValue", "metricDefinition"})
    public static final class MetricItemDto {
        private String metricValue;
        private Context metricDefinition;

        public String getMetricValue() {
            return metricValue;
        }

        public void setMetricValue(String metricValue) {
            this.metricValue = metricValue;
        }

        public Context getMetricDefinition() {
            return metricDefinition;
        }

        public void setMetricDefinition(Context metricDefinition) {
            this.metricDefinition = metricDefinition;
        }
    }

    public class Actions extends RedfishActionsDto {
    }

    @JsonPropertyOrder({"metrics", "oem"})
    public final class Links extends RedfishLinksDto {
        private Collection<MetricItemDto> metrics = new HashSet<>();

        public Collection<MetricItemDto> getMetrics() {
            return metrics;
        }

        public void setMetrics(Collection<MetricItemDto> metrics) {
            this.metrics = metrics;
        }
    }
}
