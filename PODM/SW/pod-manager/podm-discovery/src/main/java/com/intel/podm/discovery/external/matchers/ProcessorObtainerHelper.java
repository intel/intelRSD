/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.discovery.external.matchers;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.resources.redfish.ProcessorResource;

import javax.enterprise.context.Dependent;
import java.util.Objects;
import java.util.Optional;

@Dependent
public class ProcessorObtainerHelper implements EntityObtainerHelper<ProcessorResource> {
    @Override
    public ComputerSystemResource findComputerSystemResourceFor(ProcessorResource resource) throws WebClientRequestException {
        return (ComputerSystemResource) resource.getComputerSystem().get();
    }

    @Override
    public Optional<? extends Entity> findEntityFor(ComputerSystem computerSystem, ProcessorResource resource) {
        String resourceProcessorSocket = resource.getSocket().orElse(null);

        return computerSystem.getProcessors().stream()
            .filter(processor -> Objects.equals(processor.getSocket(), resourceProcessorSocket))
            .findFirst();
    }

    @Override
    public Class<? extends Entity> getEntityClass() {
        return Processor.class;
    }

    @Override
    public Class<ProcessorResource> getResourceClass() {
        return ProcessorResource.class;
    }
}
