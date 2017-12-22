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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.EthernetSwitchPortMetricsDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortMetrics;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
public class EthernetSwitchPortMetricsReaderServiceImpl implements ReaderService<EthernetSwitchPortMetricsDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        throw new UnsupportedOperationException("Ethernet Switch Port Metrics is a singleton resource!");
    }

    @Override
    @Transactional(REQUIRED)
    public EthernetSwitchPortMetricsDto getResource(Context context) throws ContextResolvingException {
        EthernetSwitchPort switchPort = (EthernetSwitchPort) traverser.traverse(context);
        EthernetSwitchPortMetrics metrics = switchPort.getMetrics();
        if (metrics == null) {
            throw new ContextResolvingException("Ethernet Switch Port Metrics resource not found for specified Ethernet Switch Port.", context, null);
        }

        return (EthernetSwitchPortMetricsDto) entityToDtoMapper.map(metrics);
    }
}
