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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.ServiceType;

import java.util.Objects;
import java.util.Set;

public abstract class ServiceTypeSpecializedDiscoveryFinalizer {
    private final ServiceType serviceType;

    public ServiceTypeSpecializedDiscoveryFinalizer(ServiceType serviceType) {
        this.serviceType = serviceType;
    }

    public final boolean isAppropriateForServiceType(ServiceType serviceType) {
        return Objects.equals(serviceType, this.serviceType);
    }

    public abstract void finalize(Set<Entity> discoveredEntities, ExternalService service);
}
