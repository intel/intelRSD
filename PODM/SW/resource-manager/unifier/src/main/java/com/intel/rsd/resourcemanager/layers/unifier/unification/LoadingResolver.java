/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.intel.rsd.crawler.Crawler;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactoryImpl.UnificationMappings;
import lombok.NonNull;
import lombok.val;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Optional;

import static java.util.Comparator.comparing;
import static java.util.Optional.empty;

class LoadingResolver implements UnifiedOdataIdResolver {
    private final Crawler crawler;
    private final UnifiedOdataIdResolver delegate;
    private final List<Resource> unresolvedResources = new ArrayList<>();

    LoadingResolver(Crawler crawler, UnifiedOdataIdResolver delegate) {
        this.crawler = crawler;
        this.delegate = delegate;
    }

    @Override
    public Optional<String> toUnified(@NonNull String serviceSpecificOdataId) {
        Optional<String> resolvedUri = delegate.toUnified(serviceSpecificOdataId);
        return resolvedUri.isPresent() ? resolvedUri : tryResolve(serviceSpecificOdataId);
    }

    private Optional<String> tryResolve(@NonNull String serviceSpecificOdataId) {
        val iterator = crawler.iterator(serviceSpecificOdataId);
        while (iterator.hasNext()) {
            Resource nextResource = iterator.next();
            if (updateMapping(nextResource)) {
                tryResolveUnresolved();
                return delegate.toUnified(serviceSpecificOdataId);
            }
            unresolvedResources.add(nextResource);
        }
        return empty();
    }

    private void tryResolveUnresolved() {
        unresolvedResources.sort(comparing(Resource::getOdataId));
        unresolvedResources.removeIf(this::updateMapping);
    }

    @Override
    public Optional<String> toServiceSpecific(String unifiedOdataId) {
        return delegate.toServiceSpecific(unifiedOdataId);
    }

    @Override
    public boolean updateMapping(Resource resource) {
        return delegate.updateMapping(resource);
    }

    @Override
    public void updateMapping(Collection<Resource> resources) {
        delegate.updateMapping(resources);
    }

    @Override
    public UnificationMappings getUnificationMappings() {
        return delegate.getUnificationMappings();
    }
}
