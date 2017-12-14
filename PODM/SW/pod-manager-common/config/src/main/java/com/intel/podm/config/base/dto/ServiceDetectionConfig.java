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

package com.intel.podm.config.base.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.common.types.discovery.DiscoveryProtocols;
import com.intel.podm.config.base.ConfigFile;
import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols.ServiceTypeMapping;
import org.apache.commons.net.util.SubnetUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static com.intel.podm.common.types.ServiceType.INBAND;
import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static com.intel.podm.common.types.discovery.DiscoveryProtocols.SSDP;
import static java.util.Collections.singletonList;

@ConfigFile(filename = "service-detection.json")
public class ServiceDetectionConfig extends BaseConfig {
    private static final String ALL_NETWORKS = "0.0.0.0/0";

    @JsonProperty("EnabledProtocols")
    private List<DiscoveryProtocols> enabledProtocols = singletonList(SSDP);

    @JsonProperty("ServiceTypeMapping")
    private ServiceTypeMapping serviceTypeMapping = new ServiceTypeMapping();

    @JsonProperty("Protocols")
    private Protocols protocols = new Protocols();

    public boolean isProtocolEnabled(DiscoveryProtocols protocol) {
        return enabledProtocols.contains(protocol);
    }

    public ServiceTypeMapping getServiceTypeMapping() {
        return serviceTypeMapping;
    }

    public Protocols getProtocols() {
        return protocols;
    }

    public static class Protocols {
        @JsonProperty("DHCP")
        private Dhcp dhcp;

        @JsonProperty("SSDP")
        private Ssdp ssdp;

        public Dhcp getDhcp() {
            return dhcp != null ? dhcp : new Dhcp();
        }

        public Ssdp getSsdp() {
            return ssdp != null ? ssdp : new Ssdp();
        }

        public static class Dhcp {
            @JsonProperty("FilesCheckIntervalInSeconds")
            private long filesCheckIntervalInSeconds = 10L;

            @JsonProperty("FailedEndpointRecheckIntervalInSeconds")
            private long failedEndpointRecheckInterval = 300L;

            @JsonProperty("NumberOfRetriesForFailedServiceCheck")
            private long retriesForFailedServiceCheck = 5;

            public long getFilesCheckIntervalInSeconds() {
                return filesCheckIntervalInSeconds;
            }

            public long getFailedEndpointRecheckInterval() {
                return failedEndpointRecheckInterval;
            }

            public long getRetriesForFailedServiceCheck() {
                return retriesForFailedServiceCheck;
            }
        }

        public static class Ssdp {
            @JsonProperty("MX")
            private Integer mx = 5;

            private Integer msearchBulkRepeat = 3;

            private Integer msearchBulkIntervalMilliseconds = 500;

            @JsonProperty("AnnouncementFrequencyInSeconds")
            private Long announcementFrequencyInSeconds = 600L;

            @JsonProperty("Subnets")
            private List<SubnetUtils> subnets = new ArrayList<SubnetUtils>() {
                {
                    add(new SubnetUtils(ALL_NETWORKS));
                }
            };

            public List<SubnetUtils> getSubnets() {
                return subnets;
            }

            public Integer getMx() {
                return mx;
            }

            public Long getAnnouncementFrequencyInSeconds() {
                return announcementFrequencyInSeconds;
            }

            public Integer getMsearchBulkRepeat() {
                return msearchBulkRepeat;
            }

            public Integer getMsearchBulkIntervalMilliseconds() {
                return msearchBulkIntervalMilliseconds;
            }
        }

        public static class ServiceTypeMapping {
            @JsonProperty("PSME")
            private String psmeServiceName = "PSME Service Root";

            @JsonProperty("RSS")
            private String rssServiceName = "RSS Service Root";

            @JsonProperty("LUI")
            private String luiServiceName = "LUI Service Root";

            @JsonProperty("RMM")
            private String rmmServiceName = "Root Service";

            @JsonProperty("INBAND")
            private String inBandServiceName = "In Band Service";

            private Map<String, ServiceType> mappingType = new HashMap<>();

            public ServiceType getServiceTypeForName(String serviceName) {
                if (mappingType.isEmpty()) {
                    mappingType.put(luiServiceName, LUI);
                    mappingType.put(psmeServiceName, PSME);
                    mappingType.put(rmmServiceName, RMM);
                    mappingType.put(rssServiceName, RSS);
                    mappingType.put(inBandServiceName, INBAND);
                }
                return mappingType.get(serviceName);
            }
        }
    }
}
