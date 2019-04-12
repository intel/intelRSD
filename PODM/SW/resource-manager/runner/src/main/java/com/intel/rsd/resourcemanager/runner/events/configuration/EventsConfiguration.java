/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.runner.events.configuration;

import lombok.Data;
import lombok.ToString;
import org.apache.commons.net.util.SubnetUtils;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.context.annotation.Configuration;

import java.net.URI;
import java.time.Duration;
import java.util.List;
import java.util.StringJoiner;

import static com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.ReceiverType.Fixed;

@ConfigurationProperties("events")
@Data
@Configuration
public class EventsConfiguration {
    private Submitter submitter;
    private Receiver receiver;

    @Data
    public static class Submitter {
        private String endpoint;
    }

    @Data
    @ToString
    public static class Receiver {
        private ReceiverType type = Fixed;
        private String endpoint;
        private FixedProvider fixed;
        private DynamicProvider dynamic;
        private SubscriptionTask subscriptionTask;

        @Data
        public static class SubscriptionTask {
            private Duration taskDelay;
            private Duration taskSchedulerDelay;

            @Override
            public String toString() {
                return new StringJoiner(", ", SubscriptionTask.class.getSimpleName() + "[", "]")
                    .add("taskDelay=" + taskDelay)
                    .add("taskSchedulerDelay=" + taskSchedulerDelay)
                    .toString();
            }
        }

        @Data
        public static class FixedProvider {
            private URI targetUri;

            @Override
            public String toString() {
                return new StringJoiner(", ", FixedProvider.class.getSimpleName() + "[", "]")
                    .add("targetUri=" + targetUri)
                    .toString();
            }
        }

        @Data
        public static class DynamicProvider {
            private Integer targetPort;
            private String targetProtocol;
            private List<DynamicProviderMapping> mapping;

            @Override
            public String toString() {
                return new StringJoiner(", ", DynamicProvider.class.getSimpleName() + "[", "]")
                    .add("targetPort=" + targetPort)
                    .add("targetProtocol=" + targetProtocol)
                    .add("mapping=" + mapping)
                    .toString();
            }
        }

        @Data
        public static class DynamicProviderMapping {
            private SubnetUtils sourceSubnet;
            private List<String> targetIpAddresses;

            @Override
            public String toString() {
                return new StringJoiner(", ", DynamicProviderMapping.class.getSimpleName() + "[", "]")
                    .add("sourceSubnet=" + sourceSubnet.getInfo().getCidrSignature())
                    .add("targetIpAddresses=" + targetIpAddresses)
                    .toString();
            }
        }

        public enum ReceiverType {
            Fixed,
            Dynamic
        }
    }
}
