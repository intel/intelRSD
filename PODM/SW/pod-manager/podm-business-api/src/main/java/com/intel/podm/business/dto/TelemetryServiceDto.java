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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "metricDefinitions", "metricReportDefinitions", "triggers", "oem"})
public final class TelemetryServiceDto extends RedfishDto {
    @JsonInclude(NON_NULL)
    private Status status;

    private SingletonContext metricDefinitions;

    private SingletonContext metricReportDefinitions;

    // TODO: RSASW-6796
    @JsonIgnore
    private SingletonContext triggers;

    private DefaultTopLevelOemDto oem;

    public TelemetryServiceDto() {
        super("#TelemetryService.v1_0_0.TelemetryService");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public SingletonContext getMetricDefinitions() {
        return metricDefinitions;
    }

    public void setMetricDefinitions(SingletonContext metricDefinitions) {
        this.metricDefinitions = metricDefinitions;
    }

    public SingletonContext getMetricReportDefinitions() {
        return metricReportDefinitions;
    }

    public void setMetricReportDefinitions(SingletonContext metricReportDefinitions) {
        this.metricReportDefinitions = metricReportDefinitions;
    }

    public SingletonContext getTriggers() {
        return triggers;
    }

    public void setTriggers(SingletonContext triggers) {
        this.triggers = triggers;
    }
}
