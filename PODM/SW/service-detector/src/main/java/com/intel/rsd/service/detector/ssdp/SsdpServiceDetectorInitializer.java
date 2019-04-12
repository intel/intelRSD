/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.service.detector.ssdp;

import com.intel.rsd.service.detector.ServiceDescriptor;
import com.intel.rsd.service.detector.ServiceDetectionListener;
import com.intel.rsd.service.detector.config.SsdpConfig;
import com.intel.rsd.service.detector.ssdp.protocol.SsdpProtocolFactory;
import com.intel.rsd.service.detector.ssdp.transport.impl.FilteredDatagramProcessorImpl;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.UpnpServiceImpl;
import org.fourthline.cling.protocol.ProtocolFactory;
import org.fourthline.cling.transport.spi.DatagramProcessor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Profile;
import org.springframework.core.task.support.ExecutorServiceAdapter;
import org.springframework.scheduling.concurrent.CustomizableThreadFactory;
import org.springframework.scheduling.concurrent.ThreadPoolTaskExecutor;
import org.springframework.stereotype.Component;

import java.util.concurrent.ExecutorService;

@Slf4j
@Component
@Profile({"ssdp"})
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount", "checkstyle:MagicNumber"})
public class SsdpServiceDetectorInitializer {

    @Autowired
    public SsdpServiceDetectorInitializer(SsdpConfig ssdpConfig) {
        log.info("{}", ssdpConfig);
    }

    @Bean
    ExecutorService defaultExecutorService(SsdpConfig ssdpConfig) {
        val executor = new ThreadPoolTaskExecutor();
        executor.setCorePoolSize(ssdpConfig.getThreadPoolSize());
        executor.setMaxPoolSize(ssdpConfig.getThreadPoolSize());
        executor.setThreadFactory(new CustomizableThreadFactory("SSDP-"));
        executor.setQueueCapacity(ssdpConfig.getSsdpTaskQueueCapacity());
        executor.setRejectedExecutionHandler((r, e) -> log.warn("Queue is full, rejecting {}", r));
        executor.initialize();
        return new ExecutorServiceAdapter(executor);
    }

    @Bean
    DatagramProcessor datagramProcessor(SsdpConfig ssdpConfig) {
        return new FilteredDatagramProcessorImpl(ssdpConfig);
    }

    @Bean
    SsdpServiceConfiguration ssdpServiceConfiguration(ExecutorService defaultExecutorService,
                                                      DatagramProcessor datagramProcessor) {

        return new SsdpServiceConfiguration(defaultExecutorService, datagramProcessor);
    }

    @Bean
    UpnpService upnpService(@Autowired SsdpRegistryListener ssdpRegistryListener,
                            @Autowired SsdpServiceConfiguration ssdpServiceConfiguration,
                            @Autowired ServiceDescriptor serviceDescriptor,
                            @Autowired SsdpConfig ssdpConfig) {

        return new UpnpServiceImpl(ssdpServiceConfiguration, ssdpRegistryListener) {
            @Override
            protected ProtocolFactory createProtocolFactory() {
                return new SsdpProtocolFactory(this, serviceDescriptor, ssdpConfig);
            }
        };
    }

    @Bean
    SearchRedfishServiceTask searchRedfishServiceTask(UpnpService upnpService, SsdpConfig ssdpConfig) {
        return new SearchRedfishServiceTask(upnpService, ssdpConfig);
    }

    @Bean
    SsdpRegularActionsScheduler ssdpRegularActionsScheduler(SearchRedfishServiceTask provideEndpointsScheduledTask) {
        return new SsdpRegularActionsScheduler(provideEndpointsScheduledTask);
    }

    @Bean
    SsdpRegistryListener ssdpRegistryListener(ServiceDetectionListener serviceDetectionListener) {
        return new SsdpRegistryListener(serviceDetectionListener);
    }
}
