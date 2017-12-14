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

package com.intel.podm.business.entities.resolvers;

import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import java.util.List;

import static java.util.Collections.unmodifiableList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;

@ApplicationScoped
public class MultiSourceEntityResolverProvider {
    @Inject
    private Instance<MultiSourceEntityResolver<? extends DiscoverableEntity>> multiSourceResolvers;

    private List<MultiSourceEntityResolver> cachedMultiSourceEntityResolvers;

    @PostConstruct
    public void postConstruct() {
        this.cachedMultiSourceEntityResolvers = stream(multiSourceResolvers.spliterator(), false).collect(toList());
    }

    public List<MultiSourceEntityResolver> getCachedMultiSourceEntityResolvers() {
        return unmodifiableList(cachedMultiSourceEntityResolvers);
    }
}
