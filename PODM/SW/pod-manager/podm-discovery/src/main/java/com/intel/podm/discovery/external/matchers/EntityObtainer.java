/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import java.util.Optional;

import static java.lang.String.format;
import static java.util.stream.StreamSupport.stream;

@Dependent
public class EntityObtainer {

    @Inject
    Instance<EntityObtainerHelper<? extends ExternalServiceResource>> helpers;

    @Inject
    ExternalServiceDao externalServiceDao;

    @Inject
    ComputerSystemFinder computerSystemFinder;

    public Entity obtain(ExternalService service, ExternalServiceResource resource) {
        EntityObtainerHelper helper = getHelper(resource);
        if (helper == null) {
            return null;
        }

        ComputerSystemResource computerSystemResource = findComputerSystemResource(resource, helper);
        ComputerSystem computerSystem = computerSystemFinder.findByCorrelatedPsmeComputerSystem(computerSystemResource);
        Optional<Entity> entity = helper.findEntityFor(computerSystem, resource);

        return entity.orElseGet(() -> externalServiceDao.createEntity(service, resource.getUri(), helper.getEntityClass()));
    }

    private ComputerSystemResource findComputerSystemResource(ExternalServiceResource resource, EntityObtainerHelper helper) {
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

    private EntityObtainerHelper getHelper(ExternalServiceResource resource) {
        return stream(helpers.spliterator(), false)
                .filter(helper -> helper.getResourceClass().isInstance(resource))
                .findFirst().orElse(null);
    }
}
