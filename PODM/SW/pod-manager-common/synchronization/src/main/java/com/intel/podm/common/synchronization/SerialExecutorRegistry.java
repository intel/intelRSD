/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.common.synchronization;

import com.codahale.metrics.MetricRegistry;
import com.intel.podm.common.logger.Logger;

import javax.ejb.Lock;
import javax.ejb.LockType;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.inject.Named;
import java.util.HashMap;
import java.util.concurrent.ExecutorService;

@Singleton
public class SerialExecutorRegistry {
    @Inject
    private Logger log;

    @Inject
    @Named("TasksExecutor")
    private ExecutorService executorService;

    @Inject
    private MetricRegistry metricRegistry;

    private HashMap<Object, SerialExecutor> registry = new HashMap<>();

    private TaskExecutorProvider taskExecutorProvider = new TaskExecutorProvider();

    @Lock(LockType.WRITE)
    public SerialExecutor getExecutor(Object key) {
        if (!registry.containsKey(key)) {
            log.i("Creating serial executorService for {}", key);
            registry.put(key, createExecutor(key));
        }
        return registry.get(key);
    }

    private SerialExecutor createExecutor(Object key) {
        return new InstrumentedSerialExecutor(
            new SerialExecutorImpl(key, executorService, taskExecutorProvider),
            metricRegistry
        );
    }

    @Lock(LockType.WRITE)
    public void unregisterExecutor(Object synchronizationKey) {
        registry.remove(synchronizationKey);
    }
}
