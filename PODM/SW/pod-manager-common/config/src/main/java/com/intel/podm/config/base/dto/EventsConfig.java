/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.ConfigFile;

import javax.ws.rs.core.UriBuilderException;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

import static com.google.common.base.MoreObjects.toStringHelper;
import static com.intel.podm.common.types.ServiceType.INBAND;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static com.intel.podm.common.utils.Networking.getIpAddressesForNetworkInterface;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.UriBuilder.fromUri;

@ConfigFile(filename = "events.json")
@JsonIgnoreProperties(ignoreUnknown = true)
@SuppressWarnings({"checkstyle:MethodCount"})
public class EventsConfig extends BaseConfig {
    @JsonIgnore
    public static final String EVENT_RECEIVING_ENDPOINT = "/rest/EventListener";

    @JsonIgnore
    private static final Map<ServiceType, String> SERVICE_TYPE_NAMES = new HashMap<>();

    @JsonIgnore
    private static final Map<ServiceType, EventConfiguration> EVENT_CONFIGURATION_CACHE = new ConcurrentHashMap<>();

    @JsonIgnore
    private static final String HTTP_SCHEME = "http";

    @JsonIgnore
    private static final String HTTPS_SCHEME = "https";

    @JsonIgnore
    private static final Integer HTTP_POD_MANAGER_PORT = 8080;

    @JsonIgnore
    private static final Integer HTTPS_POD_MANAGER_PORT = 8443;

    static {
        SERVICE_TYPE_NAMES.put(PSME, "psme");
        SERVICE_TYPE_NAMES.put(RMM, "rmm");
        SERVICE_TYPE_NAMES.put(RSS, "rss");
        SERVICE_TYPE_NAMES.put(INBAND, "inband");
    }

    @JsonProperty("Northbound")
    private NorthboundConfiguration northboundConfiguration = new NorthboundConfiguration();

    @JsonProperty("Southbound")
    private SouthboundConfiguration southboundConfiguration = new SouthboundConfiguration();

    public NorthboundConfiguration getNorthboundConfiguration() {
        return northboundConfiguration;
    }

    public long getEventSubscriptionIntervalSeconds() {
        return southboundConfiguration.eventSubscriptionIntervalSeconds;
    }

    public SouthboundConfiguration getSouthboundConfiguration() {
        return southboundConfiguration;
    }

    public EventConfiguration getEventConfigForServiceType(ServiceType serviceType) {
        if (EVENT_CONFIGURATION_CACHE.containsKey(serviceType)) {
            return EVENT_CONFIGURATION_CACHE.get(serviceType);
        } else {
            EventConfiguration configuration = new EventConfiguration(serviceType, buildEventServiceDestinationUri(serviceType, false),
                buildEventServiceDestinationUri(serviceType, true));
            EVENT_CONFIGURATION_CACHE.putIfAbsent(serviceType, configuration);
            return configuration;
        }
    }

    private URI buildEventServiceDestinationUri(ServiceType serviceType, Boolean sslEnabled) {
        try {
            String ipAddress = getPodManagerIpForReceivingEventsFromServiceOfType(serviceType);
            String path = EVENT_RECEIVING_ENDPOINT + "/" + serviceType.name().toLowerCase();
            return fromUri("{scheme}://{ip}:{port}")
                .resolveTemplate("scheme", sslEnabled ? HTTPS_SCHEME : HTTP_SCHEME)
                .resolveTemplate("ip", ipAddress)
                .resolveTemplate("port", sslEnabled ? HTTPS_POD_MANAGER_PORT : HTTP_POD_MANAGER_PORT)
                .path(path)
                .build();
        } catch (IllegalArgumentException | UriBuilderException e) {
            throw new IllegalArgumentException("Couldn't create valid PodM Event Service destination", e);
        }
    }

    private String getPodManagerIpForReceivingEventsFromServiceOfType(ServiceType serviceType) {
        String ipAddressForEvents = getPodManagerIpAddressForEventsFrom(serviceType);
        String interfaceNameForEvents = getPodManagerInterfaceNameForEventsFrom(serviceType);

        return ofNullable(ipAddressForEvents).orElseGet(() -> getIpAddressByNetworkInterface(interfaceNameForEvents));
    }

    private String getPodManagerIpAddressForEventsFrom(ServiceType serviceType) {
        return getServiceConfiguration(serviceType).podManagerIpAddress;
    }

    private String getPodManagerInterfaceNameForEventsFrom(ServiceType serviceType) {
        return getServiceConfiguration(serviceType).networkInterfaceName;
    }

    private ServiceConfiguration getServiceConfiguration(ServiceType serviceType) {
        String serviceTypeName = SERVICE_TYPE_NAMES.get(serviceType);

        return southboundConfiguration.serviceConfigurations.stream()
            .filter(serviceConfiguration -> Objects.equals(serviceConfiguration.serviceType, serviceTypeName))
            .findFirst()
            .orElseThrow(() -> new IllegalArgumentException(format("Couldn't get Service Configuration for service type '%s'", serviceType)));
    }

    private String getIpAddressByNetworkInterface(String ifaceName) {
        return getIpAddressesForNetworkInterface(ifaceName).stream()
            .findFirst()
            .orElseThrow(() -> new IllegalArgumentException(format("Couldn't get PodM IP address for network interface type '%s'", ifaceName)));
    }

    @Override
    public boolean configUpdateIsAccepted(BaseConfig updatedConfig) {
        if (!(updatedConfig instanceof EventsConfig)) {
            return false;
        }

        EventsConfig newConfig = (EventsConfig) updatedConfig;

        return isSameConfigForServiceType(newConfig, PSME)
            && isSameConfigForServiceType(newConfig, RSS)
            && isSameConfigForServiceType(newConfig, RMM)
            && isSameConfigForServiceType(newConfig, INBAND);
    }

    private boolean isSameConfigForServiceType(EventsConfig newConfig, ServiceType serviceType) {
        return Objects.equals(newConfig.getServiceConfiguration(serviceType), getServiceConfiguration(serviceType));
    }

    public static class NorthboundConfiguration {
        @JsonProperty("DeliveryRetryAttempts")
        private long deliveryRetryAttempts = 3;

        @JsonProperty("DeliveryRetryIntervalSeconds")
        private long deliveryRetryIntervalSeconds = 3;

        public long getDeliveryRetryAttempts() {
            return deliveryRetryAttempts;
        }

        public long getDeliveryRetryIntervalSeconds() {
            return deliveryRetryIntervalSeconds;
        }
    }

    public static class SouthboundConfiguration {
        @JsonProperty("EventSubscriptionIntervalSeconds")
        long eventSubscriptionIntervalSeconds = 90;

        @JsonProperty("ServicesConfiguration")
        List<ServiceConfiguration> serviceConfigurations = new ArrayList<>();

        @JsonProperty("BufferedEventProcessing")
        private BufferedEventProcessing bufferedEventProcessing;

        public boolean isBufferedEventProcessingEnabled() {
            return bufferedEventProcessing != null;
        }

        public BufferedEventProcessing getBufferedEventProcessing() {
            return bufferedEventProcessing;
        }

        @Override
        public String toString() {
            return toStringHelper(this)
                .add("eventSubscriptionIntervalSeconds", eventSubscriptionIntervalSeconds)
                .add("serviceConfigurations", serviceConfigurations)
                .toString();
        }
    }

    public static class BufferedEventProcessing {
        @JsonProperty("ProcessingWindowSizeInSeconds")
        long processingWindowSizeInSeconds = 5;

        public long getProcessingWindowSizeInSeconds() {
            return processingWindowSizeInSeconds;
        }
    }

    private static class ServiceConfiguration {
        @JsonProperty("ServiceType")
        String serviceType;

        @JsonProperty("NetworkInterfaceName")
        String networkInterfaceName;

        @JsonProperty("PodManagerIpAddress")
        String podManagerIpAddress;

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            ServiceConfiguration that = (ServiceConfiguration) o;
            return Objects.equals(serviceType, that.serviceType)
                && Objects.equals(networkInterfaceName, that.networkInterfaceName)
                && Objects.equals(podManagerIpAddress, that.podManagerIpAddress);
        }

        @Override
        public int hashCode() {
            return Objects.hash(serviceType, networkInterfaceName, podManagerIpAddress);
        }

        @Override
        public String toString() {
            return toStringHelper(this)
                .add("serviceType", serviceType)
                .add("networkInterfaceName", networkInterfaceName)
                .add("podManagerIpAddress", podManagerIpAddress)
                .toString();
        }
    }

    public static final class EventConfiguration {
        private final ServiceType serviceType;
        private final URI defaultPodManagerEventReceivingEndpoint;
        private final URI securePodManagerEventReceivingEndpoint;

        EventConfiguration(ServiceType serviceType, URI defaultPodManagerEventReceivingEndpoint,
                           URI securePodManagerEventReceivingEndpoint) {
            this.serviceType = serviceType;
            this.defaultPodManagerEventReceivingEndpoint = defaultPodManagerEventReceivingEndpoint;
            this.securePodManagerEventReceivingEndpoint = securePodManagerEventReceivingEndpoint;
        }

        public ServiceType getServiceType() {
            return serviceType;
        }

        public URI getDefaultPodManagerEventReceivingEndpoint() {
            return defaultPodManagerEventReceivingEndpoint;
        }

        public URI getSecurePodManagerEventReceivingEndpoint() {
            return securePodManagerEventReceivingEndpoint;
        }
    }
}
