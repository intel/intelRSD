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

package com.intel.podm.services.configuration;

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.discovery.ServiceEndpoint;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryServiceConfig;

import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.inject.Named;
import javax.transaction.Transactional;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.SYNCHRONIZED_TASK_EXECUTOR;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.SUPPORTS;

@Singleton
public class ConfigurationTasksManager {
    private final Map<UUID, ScheduledFuture<?>> configurationTasks = new HashMap<>();

    @Inject
    @Named(SYNCHRONIZED_TASK_EXECUTOR)
    private ScheduledExecutorService configurationTaskExecutor;

    @Inject
    @Config
    private Holder<DiscoveryServiceConfig> discoveryServiceConfigHolder;

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private Logger logger;

    /**
     * LockType.WRITE used due to concurrent access to configuration tasks map that modifies it (put operation)
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void scheduleConfigurer(ServiceEndpoint serviceEndpoint) {
        UUID serviceUuid = serviceEndpoint.getServiceUuid();
        if (!configurationTasks.containsKey(serviceUuid)) {
            ScheduledFuture<?> configurationTask = scheduleConfigurationTask(serviceEndpoint);
            configurationTasks.put(serviceUuid, configurationTask);
        } else {
            logger.w("Configuration task is already scheduled for service {}", serviceUuid);
        }
    }

    /**
     * LockType.WRITE used due to concurrent access to configuration tasks map that modifies it (remove operation)
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void cancelConfigurer(UUID serviceUuid) {
        ScheduledFuture<?> configurationTask = configurationTasks.remove(serviceUuid);
        if (configurationTask != null) {
            logger.d("Configurer cancelled for service {}", serviceUuid);
            configurationTask.cancel(false);
        } else {
            logger.w("Configurer was already cancelled for service {} or was not scheduled", serviceUuid);
        }
    }

    private ScheduledFuture<?> scheduleConfigurationTask(ServiceEndpoint serviceEndpoint) {
        DiscoveryServiceConfig discoveryServiceConfig = discoveryServiceConfigHolder.get();
        long configurerIntervalSeconds = discoveryServiceConfig.getConfigurerIntervalSeconds();
        long configurerDelaySeconds = 0;
        logger.i("Scheduled configuration task for service {} that will execute every {}s", serviceEndpoint, configurerIntervalSeconds);

        return configurationTaskExecutor.scheduleWithFixedDelay(
            createDiscoveryServiceConfigurationTask(serviceEndpoint),
            configurerDelaySeconds,
            configurerIntervalSeconds,
            SECONDS
        );
    }

    private DiscoveryServiceConfigurationTask createDiscoveryServiceConfigurationTask(ServiceEndpoint serviceEndpoint) {
        DiscoveryServiceConfigurationTask task = beanFactory.create(DiscoveryServiceConfigurationTask.class);
        task.setServiceEndpoint(serviceEndpoint);
        return task;
    }
}

