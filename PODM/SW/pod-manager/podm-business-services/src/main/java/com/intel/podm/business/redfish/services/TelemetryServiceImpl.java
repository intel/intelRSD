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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.TelemetryServiceDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;

import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.METRIC_DEFINITION_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.METRIC_REPORT_DEFINITION_RESOURCE_NAME;

@RequestScoped
public class TelemetryServiceImpl implements ReaderService<TelemetryServiceDto> {
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        throw new UnsupportedOperationException("Telemetry service is a singleton resource!");
    }

    @Override
    public TelemetryServiceDto getResource(Context context) throws ContextResolvingException {
        TelemetryServiceDto telemetryServiceDto = new TelemetryServiceDto();
        telemetryServiceDto.setId("TelemetryService");
        telemetryServiceDto.setName("TelemetryService");
        telemetryServiceDto.setDescription("TelemetryService");

        telemetryServiceDto.setMetricDefinitions(singletonContextOf(context, METRIC_DEFINITION_RESOURCE_NAME));
        telemetryServiceDto.setMetricReportDefinitions(singletonContextOf(context, METRIC_REPORT_DEFINITION_RESOURCE_NAME));

        return telemetryServiceDto;
    }
}
