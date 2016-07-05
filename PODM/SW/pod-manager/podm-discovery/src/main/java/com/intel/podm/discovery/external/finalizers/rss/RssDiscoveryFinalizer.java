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

package com.intel.podm.discovery.external.finalizers.rss;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.components.ComposedNodeUpdater;
import com.intel.podm.discovery.external.finalizers.DiscoveryFinalizer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Set;

import static com.intel.podm.common.types.ServiceType.RSS;

@Dependent
public class RssDiscoveryFinalizer extends DiscoveryFinalizer {

    @Inject
    private ComposedNodeUpdater composedNodeUpdater;

    public RssDiscoveryFinalizer() {
        super(RSS);
    }

    @Override
    public void finalize(Set<DomainObject> discoveredDomainObjects, ExternalService service) {
        composedNodeUpdater.updateRelatedComposedNodes(discoveredDomainObjects);
    }
}
