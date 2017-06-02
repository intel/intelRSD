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

package com.intel.podm.business.entities.observers;

import com.intel.podm.business.entities.EntityToUriConverter;
import com.intel.podm.business.entities.interceptors.EventOriginInfoProvider;
import com.intel.podm.business.entities.interceptors.EventSuppressions;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.types.EntityAdded;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.event.Observes;
import javax.enterprise.event.TransactionPhase;
import javax.inject.Inject;

@ApplicationScoped
public class EventableEntityEventSourceContextInitializer {

    @Inject
    private EntityToUriConverter entityToUriConverter;

    @Inject
    private EventSuppressions eventSuppressions;

    @Inject
    private EventOriginInfoProvider eventOriginInfoProvider;

    public void onEntityAdded(@EntityAdded @Observes(during = TransactionPhase.BEFORE_COMPLETION) Entity entity) {
        if (!eventSuppressions.isSuppressed(entity)) {
            entityToUriConverter.entityToUri(eventOriginInfoProvider.findEventOrigin(entity))
                .ifPresent(entity::setEventSourceContext);
        }
    }
}
