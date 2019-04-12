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

import com.intel.rsd.service.detector.config.SsdpConfig;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.message.header.ServiceTypeHeader;

import static org.fourthline.cling.model.types.ServiceType.valueOf;

public class SearchRedfishServiceTask implements Runnable {

    public static final ServiceTypeHeader REDFISH_SERVICE_TYPE_HEADER
            = new ServiceTypeHeader(valueOf("urn:dmtf-org:service:redfish-rest:1"));

    private UpnpService upnpService;
    private SsdpConfig ssdpConfig;

    public SearchRedfishServiceTask(UpnpService upnpService, SsdpConfig ssdpConfig) {
        this.upnpService = upnpService;
        this.ssdpConfig = ssdpConfig;
    }

    @Override
    public void run() {
        upnpService.getControlPoint().search(REDFISH_SERVICE_TYPE_HEADER, ssdpConfig.getMx());
    }
}
