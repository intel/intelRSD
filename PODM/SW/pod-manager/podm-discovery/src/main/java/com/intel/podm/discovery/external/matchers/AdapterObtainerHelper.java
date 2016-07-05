/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.Adapter;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.resources.redfish.AdapterResource;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;

import javax.enterprise.context.Dependent;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Predicate;

@Dependent
public class AdapterObtainerHelper implements DomainObjectObtainerHelper<AdapterResource> {
    @Override
    public ComputerSystemResource findComputerSystemResourceFor(AdapterResource adapterResource) throws ExternalServiceApiReaderException {
        return (ComputerSystemResource) adapterResource.getComputerSystem().get();
    }

    @Override
    public Optional<Adapter> findDomainObjectFor(ComputerSystem computerSystem, AdapterResource adapterResource) {
        Predicate<Adapter> byPhysicalIdEquality = adapter ->
                Objects.equals(adapter.getBusInfo(), adapterResource.getBusInfo());

        return computerSystem.getAdapters().stream()
                            .filter(byPhysicalIdEquality)
                            .findFirst();
    }

    @Override
    public Class<? extends DomainObject> getDomainObjectClass() {
        return Adapter.class;
    }

    @Override
    public Class<AdapterResource> getResourceClass() {
        return AdapterResource.class;
    }
}
