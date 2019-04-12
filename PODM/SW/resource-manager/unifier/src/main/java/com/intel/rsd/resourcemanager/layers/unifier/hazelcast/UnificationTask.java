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

package com.intel.rsd.resourcemanager.layers.unifier.hazelcast;

import com.google.common.base.Stopwatch;
import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.layers.CrawlerFactory;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.unifier.UnificationLayer;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactory;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;

import java.io.Serializable;

@Slf4j
@SpringAware
public final class UnificationTask implements Runnable, Serializable {
    private final ServiceId serviceId;
    @Autowired
    private transient UnificationLayer unificationLayer;
    @Autowired
    private transient UnifiedOdataIdResolverFactory resolverFactory;
    @Autowired
    private transient UnificationTaskGauge unificationTaskGauge;
    @Autowired
    private transient CrawlerFactory crawlerFactoryFactory;

    UnificationTask(ServiceId serviceId) {
        this.serviceId = serviceId;
    }

    @Override
    public void run() {
        log.info("Crawling on: {} started", serviceId);
        unificationTaskGauge.increment();
        Stopwatch stopwatch = Stopwatch.createStarted();
        try {
            val crawler = crawlerFactoryFactory.builder()
                .useAccessLayer(unificationLayer)
                .forService(serviceId)
                .buildCurrentThreadBasedCrawler();

            val resources = crawler.crawl();
            log.info("Crawled over {} resources from {} in {}", resources.size(), serviceId, stopwatch);
            val resolver = resolverFactory.createUnifiedOdataIdResolver(serviceId.toString());
            resolver.updateMapping(resources);
            resources.stream()
                .map(Resource::getOdataId)
                .forEach(resolver::toUnified);
        } finally {
            unificationTaskGauge.decrement();
        }
    }
}
