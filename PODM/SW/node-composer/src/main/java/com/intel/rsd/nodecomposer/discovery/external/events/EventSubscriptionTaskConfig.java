/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.events;

import lombok.Data;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

@Component
@Data
public class EventSubscriptionTaskConfig {
    private final String serverAddress;
    private final int serverPort;
    private final int taskDelaySeconds;

    public EventSubscriptionTaskConfig(@Value("${server.address:127.0.0.1}") String serverAddress, @Value("${server.port}") int serverPort,
                                       @Value("${event_subscription_task_delay_seconds:10}") int taskDelaySeconds) {
        this.serverAddress = serverAddress;
        this.serverPort = serverPort;
        this.taskDelaySeconds = taskDelaySeconds;
    }
}
