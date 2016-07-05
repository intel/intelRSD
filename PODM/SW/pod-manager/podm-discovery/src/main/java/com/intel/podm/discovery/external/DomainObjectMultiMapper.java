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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.discovery.external.matchers.DomainObjectObtainer;
import com.intel.podm.mappers.Mapper;
import com.intel.podm.mappers.MapperFinder;
import com.intel.podm.mappers.MappingMethod;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.google.common.base.Preconditions.checkArgument;
import static com.intel.podm.mappers.MappingMethod.BASIC;
import static com.intel.podm.mappers.MappingMethod.DEEP;
import static com.intel.podm.mappers.MappingMethod.FULL;
import static java.lang.Boolean.TRUE;
import static java.util.Objects.nonNull;
import static java.util.stream.Collectors.toSet;

@Dependent
public class DomainObjectMultiMapper {

    @Inject
    private DomainObjectObtainer domainObjectObtainer;

    @Inject
    private GenericDao genericDao;

    @Inject
    private Logger logger;

    @Inject @Config
    private Holder<DiscoveryConfig> config;

    @Inject
    private MapperFinder mapperFinder;

    public Map<ExternalServiceResource, DomainObject> map(Collection<ExternalServiceResource> resources, ExternalService externalService) {
        checkArgument(nonNull(externalService), "externalService should not be null");

        Map<ExternalServiceResource, DomainObject> result = new HashMap<>();

        for (ExternalServiceResource resource : resources) {
            DomainObject domainObject = map(resource, externalService);
            if (domainObject != null) {
                domainObject.setWasLastUpdatedByDiscovery(true);
            }
            result.put(resource, domainObject);
        }

        deleteStaleDomainObjects(resources, externalService);

        return result;
    }

    private void deleteStaleDomainObjects(Collection<ExternalServiceResource> resources, ExternalService externalService) {
        List<Discoverable> objectList = externalService.getDiscoverables();
        Collection<URI> resourceUris = resources.stream().map(ExternalServiceResource::getUri).collect(toSet());

        objectList.stream().
                filter(discoverable -> discoverable.getSourceUri() != null).
                filter(discoverable -> !resourceUris.contains(discoverable.getSourceUri())).
                filter(discoverable -> TRUE.equals(((DomainObject) discoverable).getWasLastUpdatedByDiscovery())).
                forEach(discoverable -> genericDao.remove((DomainObject) discoverable));
    }

    private DomainObject map(ExternalServiceResource resource, ExternalService service) {
        MappingMethod mappingMethod = determineMappingMethod(service.getServiceType());

        Optional<Mapper<ExternalServiceResource, DomainObject>> mapper = mapperFinder.find(resource, mappingMethod);

        return mapper.isPresent()
                ? mapWithMapper(resource, service, mapper.get())
                : null;
    }

    private DomainObject mapWithMapper(ExternalServiceResource resource, ExternalService service, Mapper mapper) {
        DomainObject target;
        try {
            target = matchResourceByServiceType(resource, service, mapper.getTargetClass());
        } catch (IllegalStateException e) {
            logger.e("Problem while matching resource: '{}'\n{}", resource.getUri(), e.getMessage());
            return null;
        }

        mapper.map(resource, target);
        return target;
    }

    private DomainObject matchResourceByServiceType(ExternalServiceResource resource, ExternalService service, Class targetClass) {
        switch (service.getServiceType()) {
            case LUI:
                return domainObjectObtainer.obtain(resource);
            default:
                return service.findOrCreate(resource.getUri(), targetClass);
        }
    }

    private MappingMethod determineMappingMethod(ServiceType serviceType) {
        switch (serviceType) {
            case LUI:
                return DEEP;
            case RMM:
                return FULL;
            default:
                return config.get().isDeepDiscoveryEnabled() ? BASIC : FULL;
        }
    }
}
