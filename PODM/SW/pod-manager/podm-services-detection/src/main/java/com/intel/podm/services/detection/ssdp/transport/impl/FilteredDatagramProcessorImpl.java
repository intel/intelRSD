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

package com.intel.podm.services.detection.ssdp.transport.impl;

import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceDetectionConfig;
import com.intel.podm.config.base.dto.ServiceDetectionConfig.Protocols.Ssdp;
import org.apache.commons.net.util.SubnetUtils;
import org.fourthline.cling.model.UnsupportedDataException;
import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.transport.impl.DatagramProcessorImpl;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Specializes;
import javax.inject.Inject;
import java.net.DatagramPacket;
import java.net.InetAddress;

import static java.lang.String.format;

/**
 * Filter for incoming messages. It drops SSDP frames from networks not included SSDP configuration file
 */
@Specializes
@ApplicationScoped
public class FilteredDatagramProcessorImpl extends DatagramProcessorImpl {
    @Inject
    @Config
    private Holder<ServiceDetectionConfig> configuration;

    @Override
    public IncomingDatagramMessage read(InetAddress receivedOnAddress, DatagramPacket datagram) throws UnsupportedDataException {
        if (checkNetworkOrigin(receivedOnAddress)) {
            return super.read(receivedOnAddress, datagram);
        }

        throw new UnsupportedDataException(format("Unauthorized datagram source: %s", receivedOnAddress));
    }

    boolean checkNetworkOrigin(InetAddress receivedOnAddress) {
        Ssdp ssdpConf = configuration.get().getProtocols().getSsdp();
        for (SubnetUtils knownHost : ssdpConf.getSubnets()) {
            if (knownHost.getInfo().isInRange(receivedOnAddress.getHostAddress())) {
                return true;
            }
        }
        return false;
    }

}
