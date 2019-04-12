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

package com.intel.rsd.service.detector.ssdp.protocol;

import com.intel.rsd.service.detector.ServiceDescriptor;
import com.intel.rsd.service.detector.config.SsdpConfig;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.model.message.UpnpRequest;
import org.fourthline.cling.model.message.UpnpResponse;
import org.fourthline.cling.model.message.header.UpnpHeader;
import org.fourthline.cling.protocol.ProtocolFactoryImpl;
import org.fourthline.cling.protocol.ReceivingAsync;
import org.fourthline.cling.protocol.async.SendingSearch;


/**
 * Factory overriding default protocol factory to Rack Scale based. It's overriding default implementations for
 * receiving SSDP notifications and responses from M-SEARCH phase
 */
public class SsdpProtocolFactory extends ProtocolFactoryImpl {

    private final ServiceDescriptor serviceDescriptor;

    private final ReceivingSsdpNotificationFactory receivingSsdpNotificationFactory;

    private final int msearchBulkRepeat;

    private final int msearchBulkIntervalMilliseconds;

    public SsdpProtocolFactory(UpnpService upnpService, ServiceDescriptor serviceDescriptor, SsdpConfig config) {
        super(upnpService);
        this.serviceDescriptor = serviceDescriptor;
        this.receivingSsdpNotificationFactory = new ReceivingSsdpNotificationFactory(serviceDescriptor);
        this.msearchBulkRepeat = config.getMSearchBulkRepeat();
        this.msearchBulkIntervalMilliseconds = config.getMSearchBulkIntervalMilliseconds();
    }

    @Override
    protected ReceivingAsync createReceivingNotification(IncomingDatagramMessage<UpnpRequest> incomingRequest) {
        return receivingSsdpNotificationFactory.create(getUpnpService(), incomingRequest);
    }

    @Override
    protected ReceivingAsync createReceivingSearchResponse(IncomingDatagramMessage<UpnpResponse> incomingResponse) {
        return new ReceivingSsdpSearchResponse(getUpnpService(), incomingResponse, serviceDescriptor);
    }

    @Override
    public SendingSearch createSendingSearch(UpnpHeader searchTarget, int mxSeconds) {
        return new SendingSearch(getUpnpService(), searchTarget, mxSeconds) {
            @Override
            public int getBulkRepeat() {
                return msearchBulkRepeat;
            }

            @Override
            public int getBulkIntervalMilliseconds() {
                return msearchBulkIntervalMilliseconds;
            }
        };
    }
}
