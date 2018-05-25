/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Optional;
import java.util.Set;

import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class DiscoveryFinalizer {
    @Inject
    private Instance<ServiceTypeSpecializedDiscoveryFinalizer> discoveryFinalizers;

    private Collection<ServiceTypeSpecializedDiscoveryFinalizer> finalizers;

    @Transactional(MANDATORY)
    public void finalizeDiscovery(Set<DiscoverableEntity> discoveredEntities, ExternalService service) {
        service.markAsReachable();
        tryFindFinalizer(service)
            .ifPresent(finalizer -> finalizer.finalize(discoveredEntities, service));
    }

    private Optional<ServiceTypeSpecializedDiscoveryFinalizer> tryFindFinalizer(ExternalService service) {
        return finalizers.stream()
            .filter(discoveryFinalizer -> discoveryFinalizer.isAppropriateForServiceType(service.getServiceType()))
            .findAny();
    }

    @PostConstruct
    private void init() {
        finalizers = stream(discoveryFinalizers.spliterator(), false).collect(toList());
    }
}
