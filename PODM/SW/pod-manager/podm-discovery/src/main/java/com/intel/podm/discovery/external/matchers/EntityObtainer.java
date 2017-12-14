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

import com.intel.podm.business.entities.dao.DiscoverableEntityDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalLinkDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.redfish.ComputerSystemResource;

import javax.enterprise.context.Dependent;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;

import static java.lang.String.format;
import static java.util.stream.StreamSupport.stream;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class EntityObtainer {

    @Inject
    Instance<EntityObtainerHelper<? extends ExternalServiceResource>> helpers;

    @Inject
    ComputerSystemFinder computerSystemFinder;

    @Inject
    private ExternalLinkDao externalLinkDao;

    @Inject
    private DiscoverableEntityDao discoverableEntityDao;

    public DiscoverableEntity obtain(ExternalService service, ExternalServiceResource resource) {
        EntityObtainerHelper entityObtainerHelper = getHelper(resource);
        if (entityObtainerHelper == null) {
            return null;
        }

        ComputerSystemResource computerSystemResource = findComputerSystemResource(resource, entityObtainerHelper);
        ComputerSystem computerSystem = computerSystemFinder.findByCorrelatedPsmeComputerSystem(computerSystemResource);
        return getEntity(service, resource, entityObtainerHelper, computerSystem);
    }

    private DiscoverableEntity getEntity(ExternalService service, ExternalServiceResource resource,
                                         EntityObtainerHelper entityObtainerHelper, ComputerSystem computerSystem) {

        DiscoverableEntity entity = (DiscoverableEntity) entityObtainerHelper
            .findEntityFor(computerSystem, resource)
            .orElseGet(() -> discoverableEntityDao.createEntity(
                service,
                resource.getGlobalId(service.getId(), resource.getUri()),
                entityObtainerHelper.getEntityClass())
            );

        externalLinkDao.createIfNotExisting(resource.getUri(), service, entity);
        return entity;
    }

    private ComputerSystemResource findComputerSystemResource(ExternalServiceResource resource, EntityObtainerHelper helper) {
        ComputerSystemResource computerSystemResource;
        try {
            computerSystemResource = helper.findComputerSystemResourceFor(resource);
        } catch (WebClientRequestException e) {
            throw new IllegalStateException(format("Parent ComputerSystem resource has not been found for '%s'", resource.getUri()));
        }
        return computerSystemResource;
    }

    private EntityObtainerHelper getHelper(ExternalServiceResource resource) {
        return stream(helpers.spliterator(), false)
            .filter(helper -> helper.getResourceClass().isInstance(resource))
            .findFirst().orElse(null);
    }
}
