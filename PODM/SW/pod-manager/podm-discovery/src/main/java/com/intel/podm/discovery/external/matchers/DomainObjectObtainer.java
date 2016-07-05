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

import com.google.common.collect.Lists;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import java.util.Optional;

import static java.lang.String.format;

@Dependent
public class DomainObjectObtainer {

    @Inject
    Instance<DomainObjectObtainerHelper<? extends ExternalServiceResource>> helpers;

    @Inject
    GenericDao genericDao;

    @Inject
    ComputerSystemFinder computerSystemFinder;

    public DomainObject obtain(ExternalServiceResource resource) {
        DomainObjectObtainerHelper helper = getHelper(resource);
        ComputerSystemResource computerSystemResource = findComputerSystemResource(resource, helper);
        ComputerSystem computerSystem = computerSystemFinder.findByCorrelatedPsmeComputerSystem(computerSystemResource);
        Optional<DomainObject> domainObject = helper.findDomainObjectFor(computerSystem, resource);

        return domainObject.orElseGet(() -> genericDao.create(helper.getDomainObjectClass()));
    }

    private ComputerSystemResource findComputerSystemResource(ExternalServiceResource resource, DomainObjectObtainerHelper helper) {
        ComputerSystemResource computerSystemResource;
        try {
            computerSystemResource = helper.findComputerSystemResourceFor(resource);
        } catch (ExternalServiceApiReaderException e) {
            throw new IllegalStateException(
                    format("Parent ComputerSystem resource has not been found for '%s'", resource.getUri())
            );
        }
        return computerSystemResource;
    }

    private DomainObjectObtainerHelper getHelper(ExternalServiceResource resource) {
        return Lists.newArrayList(helpers)
                .stream()
                .filter(helper -> helper.getResourceClass().isInstance(resource))
                .findFirst().orElseThrow(() ->
                    new UnsupportedOperationException(
                        format("'%s' is not supported", resource.getClass().getSimpleName())
                    )
                );
    }
}
