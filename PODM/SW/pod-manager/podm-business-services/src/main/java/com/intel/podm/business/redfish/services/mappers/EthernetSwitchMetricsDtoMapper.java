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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.EthernetSwitchMetricsDto;
import com.intel.podm.business.entities.redfish.EthernetSwitchMetrics;

import javax.enterprise.context.Dependent;

@Dependent
class EthernetSwitchMetricsDtoMapper extends DtoMapper<EthernetSwitchMetrics, EthernetSwitchMetricsDto> {
    EthernetSwitchMetricsDtoMapper() {
        super(EthernetSwitchMetrics.class, EthernetSwitchMetricsDto.class);
    }
}
