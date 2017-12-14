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

package com.intel.podm.eventing.observers;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.types.RedfishEvent;

import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.enterprise.event.Observes;
import javax.inject.Inject;
import javax.inject.Named;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ScheduledExecutorService;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.common.enterprise.utils.beans.JndiNames.EVENT_SUBSCRIPTION_TASK_EXECUTOR;
import static com.intel.podm.eventing.observers.EventDispatcher.dispatch;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Singleton
public class RedfishEventObserver {
    @Inject
    private GenericDao genericDao;

    @Inject
    @Named(EVENT_SUBSCRIPTION_TASK_EXECUTOR)
    private ScheduledExecutorService eventSubscriptionTaskExecutor;

    @Inject
    private EventSenderFactory eventSenderFactory;

    /**
     * LockType.WRITE used to assure order of events which are going to be sent.
     */
    @Lock(WRITE)
    @Transactional(REQUIRES_NEW)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void observe(@Observes List<RedfishEvent> events) {
        Set<RedfishEvent> occurredEvents = newHashSet(events);
        Set<EventSubscription> allSubscribers = newHashSet(genericDao.findAll(EventSubscription.class));

        dispatch(occurredEvents, allSubscribers).entrySet().stream()
            .map(entry -> eventSenderFactory.createEventSender(entry.getKey().getId(), entry.getValue()))
            .forEach(eventSender -> eventSubscriptionTaskExecutor.schedule(eventSender, 0, SECONDS));
    }
}
