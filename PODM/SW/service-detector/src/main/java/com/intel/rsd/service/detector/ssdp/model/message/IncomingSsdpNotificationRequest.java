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

package com.intel.rsd.service.detector.ssdp.model.message;

import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.model.message.UpnpRequest;
import org.fourthline.cling.model.message.discovery.IncomingNotificationRequest;
import org.fourthline.cling.model.message.header.LocationHeader;

import java.net.URL;

import static com.intel.rsd.service.detector.ssdp.model.message.Headers.AL_HEADER;

/**
 * Class overriding default incoming SSDP Notification to retrieve information from AL header
 */
@SuppressWarnings({"checkstyle:AbbreviationAsWordInName"})
public class IncomingSsdpNotificationRequest extends IncomingNotificationRequest {
    public IncomingSsdpNotificationRequest(IncomingDatagramMessage<UpnpRequest> source) {
        super(source);
    }

    @Override
    public URL getLocationURL() {
        String alHeader = getHeaders().getFirstHeader(AL_HEADER);
        if (alHeader != null) {
            return new LocationHeader(alHeader).getValue();
        }

        return null;
    }
}
