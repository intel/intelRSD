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

package com.intel.podm.business.redfish.services.attach;

import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.services.context.ContextType;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Collections.unmodifiableMap;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.StreamSupport.stream;

@ApplicationScoped
public class AttachResourceStrategyMapping {
    @Inject
    private Instance<AttachResourceStrategy> attachResourceStrategyInstances;

    private Map<ContextType, AttachResourceStrategy> resourceToStrategyMapping;

    @PostConstruct
    private void init() {
        Map<ContextType, AttachResourceStrategy> map = new HashMap<>();
        Set<AttachResourceStrategy> strategies = stream(attachResourceStrategyInstances.spliterator(), false).collect(toSet());
        strategies.forEach(strategy -> map.put(strategy.supportedType(), strategy));

        resourceToStrategyMapping = unmodifiableMap(map);
    }

    public AttachResourceStrategy getStrategyForResource(ContextType contextType) throws RequestValidationException {
        requiresNonNull(contextType, "contextType");

        AttachResourceStrategy strategy = resourceToStrategyMapping.get(contextType);
        if (strategy == null) {
            throw new RequestValidationException(createWithViolations(format("Attach action is not supported for ContextType %s.", contextType)));
        }

        return strategy;
    }
}
