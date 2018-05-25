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

package com.intel.podm.common.enterprise.utils.beans;

public final class JndiNames {
    public static final String SYNCHRONIZED_TASK_EXECUTOR = "TasksExecutor";
    public static final String EVENT_SUBSCRIPTION_TASK_EXECUTOR = "EventsExecutor";
    public static final String DEEP_DISCOVERY_EXECUTOR = "DeepDiscoveryExecutor";
    public static final String SSDP_EXECUTOR = "SSDPExecutor";
    public static final String DEFAULT_SCHEDULED_EXECUTOR_SERVICE = "java:jboss/ee/concurrency/scheduler/default";

    private JndiNames() {
    }
}
