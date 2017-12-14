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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.EthernetSwitchPortMetrics;
import com.intel.podm.business.entities.redfish.embeddables.SwitchPortMetrics;
import com.intel.podm.client.resources.redfish.EthernetSwitchPortMetricsResource;
import com.intel.podm.mappers.EntityMapper;

import javax.enterprise.context.Dependent;

@Dependent
public class EthernetSwitchPortMetricsResourceMapper extends EntityMapper<EthernetSwitchPortMetricsResource, EthernetSwitchPortMetrics> {
    protected EthernetSwitchPortMetricsResourceMapper() {
        super(EthernetSwitchPortMetricsResource.class, EthernetSwitchPortMetrics.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchPortMetricsResource source, EthernetSwitchPortMetrics target) {
        super.performNotAutomatedMapping(source, target);
        SwitchPortMetrics receivedMetrics = target.getReceived() == null ? new SwitchPortMetrics() : target.getReceived();
        mapMetrics(source.getReceived(), receivedMetrics);
        target.setReceived(receivedMetrics);

        SwitchPortMetrics transmittedMetrics = target.getTransmitted() == null ? new SwitchPortMetrics() : target.getTransmitted();
        mapMetrics(source.getTransmitted(), transmittedMetrics);
        target.setTransmitted(transmittedMetrics);
    }

    private void mapMetrics(EthernetSwitchPortMetricsResource.Metrics source, SwitchPortMetrics target) {
        if (source == null) {
            return;
        }
        target.setPackets(source.getPackets());
        target.setDroppedPackets(source.getDroppedPackets());
        target.setErrorPackets(source.getErrorPackets());
        target.setBroadcastPackets(source.getBroadcastPackets());
        target.setMulticastPackets(source.getMulticastPackets());
        target.setBytes(source.getBytes());
        target.setErrors(source.getErrors());
    }
}
