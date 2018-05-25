/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.detach;

import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.services.context.ContextType.DRIVE;
import static com.intel.podm.business.services.context.ContextType.ENDPOINT;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;

@ApplicationScoped
public class DetachResourceStrategyFactory {
    private final Map<ContextType, DetachResourceStrategy> resourceTypeToStrategyMapping = new HashMap<>();

    @Inject
    private BeanFactory beanFactory;

    @PostConstruct
    public void init() {
        resourceTypeToStrategyMapping.put(DRIVE, createsStrategy(DriveDetachStrategy.class));
        resourceTypeToStrategyMapping.put(VOLUME, createsStrategy(VolumeDetachStrategy.class));
        resourceTypeToStrategyMapping.put(ENDPOINT, createsStrategy(EndpointDetachStrategy.class));
    }

    private DetachResourceStrategy createsStrategy(Class<? extends DetachResourceStrategy> detachStrategyClass) {
        return beanFactory.create(detachStrategyClass);
    }

    public DetachResourceStrategy getStrategyForResource(ContextType contextType) throws RequestValidationException {
        requiresNonNull(contextType, "contextType");
        DetachResourceStrategy detachStrategy = resourceTypeToStrategyMapping.get(contextType);

        if (detachStrategy == null) {
            throw new RequestValidationException(createWithViolations(format("Detach action is not supported for %s.", contextType)));
        }

        return detachStrategy;
    }
}
