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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.Diff;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.types.RedfishEvent;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.event.Event;
import javax.inject.Inject;
import java.util.List;

import static com.intel.podm.common.types.events.EventType.RESOURCE_ADDED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_REMOVED;
import static com.intel.podm.common.types.events.EventType.RESOURCE_UPDATED;

@ApplicationScoped
public class CollectingObserver implements EntityEventObserver {
    @Inject
    private Logger logger;

    @Inject
    private Event<List<RedfishEvent>> redfishEventPublisher;

    @Inject
    private DiffProcessor diffProcessor;

    @Inject
    private EventTypeConverter eventTypeConverter;

    @Inject
    private EntityEventCollector entityEventCollector;

    @Inject
    private ByEventSourceContextGroupingConverter byEventSourceContextGroupingConverter;

    @Inject
    private EventRedirections eventRedirections;

    @Override
    public void resourceAdded(Entity entity) {
        entityEventCollector.add(entity, RESOURCE_ADDED);
    }

    @Override
    public void resourceUpdated(Entity entity, Diff diff) {
        eventRedirections.redirectIfRequired(
            entity,
            diff::wasUpdatedFromOneValueToAnother,
            target -> entityEventCollector.add(target, RESOURCE_UPDATED)
        );

        entityEventCollector.add(entity, diffProcessor.determineEventTypes(diff));
    }

    @Override
    public void collectionResourceUpdated(Entity entity, String collectionName) {
        entityEventCollector.add(entity, RESOURCE_UPDATED);
    }

    @Override
    public void resourceRemoved(Entity entity) {
        entityEventCollector.add(entity, RESOURCE_REMOVED);
    }

    @Override
    public void onTransactionCompletion() {
        logger.t("onTransactionCompletion...");
        List<RedfishEvent> redfishEvents = entityEventCollector.convertThenEvict(byEventSourceContextGroupingConverter);
        if (redfishEvents.isEmpty()) {
            return;
        }

        logger.d("Transaction committed. Number of resources modified during transaction: {}", redfishEvents.size());
        redfishEventPublisher.fire(redfishEvents);
    }

    @Override
    public void onBeforeTransactionCompletion() {
        logger.t("onBeforeTransactionCompletion...");
        entityEventCollector.convertEventTypes(eventTypeConverter);
    }

    @Override
    public void onTransactionFailed() {
        entityEventCollector.reset();
    }
}
