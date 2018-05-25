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

package com.intel.podm.business.redfish.services;

import com.codahale.metrics.Gauge;
import com.codahale.metrics.MetricRegistry;
import com.google.common.cache.CacheBuilder;
import com.google.common.cache.CacheLoader;
import com.google.common.cache.LoadingCache;
import com.google.common.cache.RemovalListener;
import com.google.common.cache.RemovalNotification;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.monitoring.MetricRegistryFactory;
import com.intel.podm.common.types.redfish.RedfishEventArray;

import javax.annotation.PostConstruct;
import javax.annotation.Resource;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.Collection;
import java.util.UUID;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.atomic.AtomicLong;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.DEFAULT_SCHEDULED_EXECUTOR_SERVICE;
import static com.intel.podm.common.utils.IterableHelper.any;
import static java.util.concurrent.TimeUnit.SECONDS;

@ApplicationScoped
public class AutoEvictingIncomingEventsBuffer {

    @Resource(lookup = DEFAULT_SCHEDULED_EXECUTOR_SERVICE)
    private ScheduledExecutorService scheduledExecutorService;

    @Inject
    private Buffer buffer;

    public void scheduleEvictionAtFixedRate(long rate) {
        this.scheduledExecutorService.scheduleAtFixedRate(buffer::evict, 0, rate, SECONDS);
    }

    public void handle(UUID serviceUuid, RedfishEventArray events) {
        buffer.register(serviceUuid, events);
    }

    @Dependent
    static class Buffer {

        private LoadingCache<UUID, Collection<RedfishEventArray>> cache;

        private AtomicLong bufferSize = new AtomicLong();

        private EvictionListener evictionListener;

        private MetricRegistry metricRegistry;

        @Inject
        Buffer(EvictionListener evictionListener, MetricRegistryFactory metricRegistryFactory) {
            this.evictionListener = evictionListener;
            this.metricRegistry = metricRegistryFactory.getOrRegisterNew("PodM.SBEventing");
        }

        @PostConstruct
        private void init() {
            this.cache = CacheBuilder.newBuilder()
                .removalListener(evictionListener)
                .build(new CacheLoader<UUID, Collection<RedfishEventArray>>() {
                    @Override
                    public Collection<RedfishEventArray> load(UUID key) throws Exception {
                        return new ArrayList<>();
                    }
                });

            this.metricRegistry.register("numberOfEventsInTimeWindow", (Gauge<Long>) bufferSize::longValue);
        }

        void evict() {
            cache.invalidateAll();
            bufferSize.set(0);
        }

        public void register(UUID uuid, RedfishEventArray eventArray) {
            cache.getUnchecked(uuid).add(eventArray);
            bufferSize.incrementAndGet();
        }
    }

    @Dependent
    static class EvictionListener implements RemovalListener<UUID, Collection<RedfishEventArray>> {

        @Inject
        private Logger logger;

        @Inject
        private IncomingEventsProcessor incomingEventsProcessor;

        @Override
        public void onRemoval(RemovalNotification<UUID, Collection<RedfishEventArray>> notification) {
            logger.d("Processing notifications for {}", notification.getKey());
            incomingEventsProcessor.handle(notification.getKey(), any(notification.getValue()));
        }
    }
}
