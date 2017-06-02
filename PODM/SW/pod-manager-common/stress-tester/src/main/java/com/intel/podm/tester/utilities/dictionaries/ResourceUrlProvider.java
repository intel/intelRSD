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
package com.intel.podm.tester.utilities.dictionaries;

import java.net.URI;

import static java.net.URI.create;

public class ResourceUrlProvider {
    private static final String BASE_URL = "/redfish/v1";
    private static final String ETHERNET_SWITCHES = "/EthernetSwitches";
    private static final String ETHERNET_SWITCH_PORTS = "/Ports";
    private final String podmBaseUrl;

    public ResourceUrlProvider(String podmBaseUrl) {
        this.podmBaseUrl = podmBaseUrl;
    }

    public URI getUrlWithPodmBaseUrlAdded(String url) {
        return create(podmBaseUrl + url);
    }

    public URI getEthernetSwitchCollectionUrl() {
        return create(podmBaseUrl + BASE_URL + ETHERNET_SWITCHES);
    }

    public URI getEthernetSwitchPortsUrl(String ethernetSwitchPortOdataId) {
        return create(podmBaseUrl + ethernetSwitchPortOdataId + ETHERNET_SWITCH_PORTS);
    }
}
