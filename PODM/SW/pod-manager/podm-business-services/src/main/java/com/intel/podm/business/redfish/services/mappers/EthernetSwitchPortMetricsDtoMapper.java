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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.EthernetSwitchPortMetricsDto;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortMetrics;
import com.intel.podm.business.entities.redfish.embeddables.SwitchPortMetrics;

import javax.enterprise.context.Dependent;

@Dependent
public class EthernetSwitchPortMetricsDtoMapper extends DtoMapper<EthernetSwitchPortMetrics, EthernetSwitchPortMetricsDto> {
    protected EthernetSwitchPortMetricsDtoMapper() {
        super(EthernetSwitchPortMetrics.class, EthernetSwitchPortMetricsDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchPortMetrics source, EthernetSwitchPortMetricsDto target) {
        super.performNotAutomatedMapping(source, target);
        target.setReceived(mapMetrics(source.getReceived()));
        target.setTransmitted(mapMetrics(source.getTransmitted()));
    }

    private EthernetSwitchPortMetricsDto.Metrics mapMetrics(SwitchPortMetrics sourceMetrics) {
        if (sourceMetrics == null) {
            return null;
        }

        EthernetSwitchPortMetricsDto.Metrics targetMetrics = new EthernetSwitchPortMetricsDto.Metrics();
        targetMetrics.setPackets(sourceMetrics.getPackets());
        targetMetrics.setDroppedPackets(sourceMetrics.getDroppedPackets());
        targetMetrics.setErrorPackets(sourceMetrics.getErrorPackets());
        targetMetrics.setBroadcastPackets(sourceMetrics.getBroadcastPackets());
        targetMetrics.setMulticastPackets(sourceMetrics.getMulticastPackets());
        targetMetrics.setBytes(sourceMetrics.getBytes());
        targetMetrics.setErrors(sourceMetrics.getErrors());

        return targetMetrics;
    }
}
