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

package com.intel.rsd.resourcemanager.layers.completer;

import com.intel.rsd.collections.IterableHelper;
import com.intel.rsd.crawler.Crawler;
import com.intel.rsd.resourcemanager.common.services.ApiCrawlingTask;
import com.intel.rsd.resourcemanager.layers.CrawlerFactory;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.services.common.hazelcast.ScheduledWithFixedDelayOnLeaderTask;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Service;

import java.time.Duration;
import java.util.Collection;

import static com.google.common.base.Stopwatch.createStarted;
import static java.time.Duration.ofSeconds;

@Service
public class PeriodicCrawlerService extends ScheduledWithFixedDelayOnLeaderTask implements ApiCrawlingTask {
    private final Layer accessLayer;
    private final Collection<ApiCrawlingTask.ResourceCollectionConsumer> consumers;
    private final CrawlerFactory crawlerFactory;
    private final int periodicCrawlerPeriodInSeconds;

    public PeriodicCrawlerService(Completer accessLayer,
                                  TaskScheduler taskScheduler,
                                  Collection<ApiCrawlingTask.ResourceCollectionConsumer> consumers,
                                  CrawlerFactory crawlerFactory,
                                  @Value("${periodic_crawler_period_in_seconds:5}") int periodicCrawlerPeriodInSeconds) {
        super(taskScheduler);
        this.accessLayer = accessLayer;
        this.consumers = consumers;
        this.crawlerFactory = crawlerFactory;
        this.periodicCrawlerPeriodInSeconds = periodicCrawlerPeriodInSeconds;
    }

    @Override
    protected Duration getFixedDelay() {
        return ofSeconds(periodicCrawlerPeriodInSeconds);
    }

    @Override
    public String toString() {
        return "Resource Manager Periodic Crawler";
    }

    @Override
    public void run() {
        val serviceId = determineUnderlyingServiceId();
        val crawler = createCrawler(serviceId);
        val stopwatch = createStarted();
        val resources = crawler.crawl();
        log.info("Crawled over {} resources from {} in {}", resources.size(), serviceId, stopwatch);
        distribute(consumers, resources);
    }

    private Crawler createCrawler(ServiceId serviceId) {
        return crawlerFactory.builder().useAccessLayer(accessLayer).forService(serviceId).buildThreadPoolBasedCrawler();
    }

    private ServiceId determineUnderlyingServiceId() {
        return IterableHelper.single(accessLayer.getServices());
    }
}
