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
import com.intel.rsd.service.detector.ssdp.model.message.IncomingSsdpNotificationRequest;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.model.message.UpnpRequest;
import org.fourthline.cling.protocol.ReceivingAsync;

class ReceivingSsdpNotificationFactory {

    private static final ReceivingAsync NOT_RECOGNIZED_DATAGRAM = null;
    private final MessageProcessor aliveMessageProcessor;
    private final MessageProcessor byeByeMessageProcessor;

    ReceivingSsdpNotificationFactory(ServiceDescriptor serviceDescriptor) {
        this.aliveMessageProcessor = new AliveMessageProcessor(serviceDescriptor);
        this.byeByeMessageProcessor = new ByeByeMessageProcessor();
    }

    public ReceivingAsync create(UpnpService upnpService, IncomingDatagramMessage<UpnpRequest> incomingRequest) {
        IncomingSsdpNotificationRequest incomingNotificationRequest = new IncomingSsdpNotificationRequest(incomingRequest);

        if (incomingNotificationRequest.isAliveMessage()) {
            return new ReceivingSsdpNotification(upnpService, incomingNotificationRequest, aliveMessageProcessor);
        }

        if (incomingNotificationRequest.isByeByeMessage()) {
            return new ReceivingSsdpNotification(upnpService, incomingNotificationRequest, byeByeMessageProcessor);
        }

        return NOT_RECOGNIZED_DATAGRAM;
    }
}
