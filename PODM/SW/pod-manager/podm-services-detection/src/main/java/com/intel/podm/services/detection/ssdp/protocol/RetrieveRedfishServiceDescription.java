/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.services.detection.ssdp.protocol;

import com.intel.podm.discovery.external.ServiceDescriptor;
import com.intel.podm.common.types.discovery.ServiceEndpoint;
import com.intel.podm.discovery.external.UnrecognizedServiceTypeException;
import org.fourthline.cling.UpnpService;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.meta.DeviceDetails;
import org.fourthline.cling.model.meta.RemoteDevice;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.logging.Level;
import java.util.logging.Logger;

import static java.util.Collections.emptyList;

class RetrieveRedfishServiceDescription implements Runnable {

    private static final Logger LOG = Logger.getLogger(RetrieveRedfishServiceDescription.class.getName());

    private final UpnpService upnpService;

    private final RemoteDevice remoteDevice;

    private final ServiceDescriptor serviceDescriptor;

    RetrieveRedfishServiceDescription(UpnpService upnpService, RemoteDevice rd, ServiceDescriptor serviceDescriptor) {
        this.upnpService = upnpService;
        this.remoteDevice = rd;
        this.serviceDescriptor = serviceDescriptor;
    }

    @Override
    public void run() {
        try {
            URI serviceUri = remoteDevice.getIdentity().getDescriptorURL().toURI();
            ServiceEndpoint serviceEndpoint = serviceDescriptor.describe(serviceUri);
            RemoteDevice currentDevice = remoteDevice.newInstance(
                    remoteDevice.getIdentity().getUdn(),
                    remoteDevice.getVersion(),
                    remoteDevice.getType(),
                    new DeviceDetails(serviceEndpoint.getServiceType().name()),
                    remoteDevice.findIcons(),
                    remoteDevice.getServices(),
                    emptyList()
            );
            upnpService.getRegistry().addDevice(currentDevice);
        } catch (URISyntaxException | ValidationException | UnrecognizedServiceTypeException e) {
            LOG.log(Level.SEVERE, e.getMessage(), e);
        }
    }
}
