/*
 * Copyright (c) 2015 Intel Corporation
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
import com.intel.podm.config.base.ConfigFile;

import java.util.Objects;

@ConfigFile(filename = "events.json")
public class EventsConfig extends BaseConfig {
    @JsonProperty("EventSubscriptionIntervalSeconds")
    private long eventSubscriptionIntervalSeconds = 90;

    @JsonProperty("PodManagerIpAddressForEventsFromPsme")
    private String podManagerIpAddressForEventsFromPsme;

    @JsonProperty("NetworkInterfaceNameForEventsFromPsme")
    private String networkInterfaceNameForEventsFromPsme;

    @JsonProperty("PodManagerIpAddressForEventsFromRss")
    private String podManagerIpAddressForEventsFromRss;

    @JsonProperty("NetworkInterfaceNameForEventsFromRss")
    private String networkInterfaceNameForEventsFromRss;

    @JsonProperty("PodManagerIpAddressForEventsFromRmm")
    private String podManagerIpAddressForEventsFromRmm;

    @JsonProperty("NetworkInterfaceNameForEventsFromRmm")
    private String networkInterfaceNameForEventsFromRmm;

    public long getEventSubscriptionIntervalSeconds() {
        return eventSubscriptionIntervalSeconds;
    }

    public String getPodManagerIpAddressForEventsFromPsme() {
        return podManagerIpAddressForEventsFromPsme;
    }

    public String getNetworkInterfaceNameForEventsFromPsme() {
        return networkInterfaceNameForEventsFromPsme;
    }

    public String getPodManagerIpAddressForEventsFromRss() {
        return podManagerIpAddressForEventsFromRss;
    }

    public String getNetworkInterfaceNameForEventsFromRss() {
        return networkInterfaceNameForEventsFromRss;
    }

    public String getPodManagerIpAddressForEventsFromRmm() {
        return podManagerIpAddressForEventsFromRmm;
    }

    public String getNetworkInterfaceNameForEventsFromRmm() {
        return networkInterfaceNameForEventsFromRmm;
    }

    @Override
    public boolean configUpdateIsAccepted(BaseConfig updatedConfig) {
        if (!(updatedConfig instanceof EventsConfig)) {
            return false;
        }

        EventsConfig newConfig = (EventsConfig) updatedConfig;
        return samePsmeConfig(newConfig) && sameRssConfig(newConfig) && sameRmmConfig(newConfig);
    }

    private boolean samePsmeConfig(EventsConfig newConfig) {
        return Objects.equals(newConfig.getPodManagerIpAddressForEventsFromPsme(), getPodManagerIpAddressForEventsFromPsme())
                && Objects.equals(newConfig.getNetworkInterfaceNameForEventsFromPsme(), getNetworkInterfaceNameForEventsFromPsme());
    }

    private boolean sameRssConfig(EventsConfig newConfig) {
        return Objects.equals(newConfig.getPodManagerIpAddressForEventsFromRss(), getPodManagerIpAddressForEventsFromRss())
                && Objects.equals(newConfig.getNetworkInterfaceNameForEventsFromRss(), getNetworkInterfaceNameForEventsFromRss());
    }

    private boolean sameRmmConfig(EventsConfig newConfig) {
        return Objects.equals(newConfig.getPodManagerIpAddressForEventsFromRmm(), getPodManagerIpAddressForEventsFromRmm())
                && Objects.equals(newConfig.getNetworkInterfaceNameForEventsFromRmm(), getNetworkInterfaceNameForEventsFromRmm());
    }
}
