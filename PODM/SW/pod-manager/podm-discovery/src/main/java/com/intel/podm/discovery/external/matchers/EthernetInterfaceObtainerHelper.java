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
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.api.resources.redfish.EthernetInterfaceResource;

import javax.enterprise.context.Dependent;
import java.util.Objects;
import java.util.Optional;

@Dependent
public class EthernetInterfaceObtainerHelper implements DomainObjectObtainerHelper<EthernetInterfaceResource> {
    @Override
    public ComputerSystemResource findComputerSystemResourceFor(EthernetInterfaceResource resource) throws ExternalServiceApiReaderException {
        return (ComputerSystemResource) resource.getComputerSystem().get();
    }

    @Override
    public Optional<EthernetInterface> findDomainObjectFor(ComputerSystem computerSystem, EthernetInterfaceResource resource) {
        return computerSystem.getEthernetInterfaces().stream()
                .filter(eth -> Objects.equals(eth.getMacAddress(), resource.getMacAddress()))
                .findFirst();
    }

    @Override
    public Class<? extends DomainObject> getDomainObjectClass() {
        return EthernetInterface.class;
    }

    @Override
    public Class<EthernetInterfaceResource> getResourceClass() {
        return EthernetInterfaceResource.class;
    }
}
