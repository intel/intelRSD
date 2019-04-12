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

package com.intel.rsd.service.detector.ssdp.transport.impl;

import com.intel.rsd.service.detector.config.SsdpConfig;
import org.apache.commons.net.util.SubnetUtils;
import org.fourthline.cling.model.UnsupportedDataException;
import org.fourthline.cling.model.message.IncomingDatagramMessage;
import org.fourthline.cling.transport.impl.DatagramProcessorImpl;

import java.net.DatagramPacket;
import java.net.InetAddress;
import java.util.Collection;
import java.util.function.Predicate;

/**
 * Filter for incoming messages. It drops SSDP frames from networks not included SSDP configuration file
 */
public class FilteredDatagramProcessorImpl extends DatagramProcessorImpl {

    private final Predicate<String> whiteListedAddresses;

    public FilteredDatagramProcessorImpl(SsdpConfig ssdpConfig) {
        setInclusiveHostCountForExclusions(ssdpConfig.getExclusions());
        this.whiteListedAddresses = createPredicate(ssdpConfig.getExclusions()).negate().and(createPredicate(ssdpConfig.getSubnets()));
    }

    private Predicate<String> createPredicate(Collection<SubnetUtils> exclusions) {
        return exclusions.stream()
            .map(SubnetUtils::getInfo)
            .map(subnetInfo -> (Predicate<String>) subnetInfo::isInRange)
            .reduce(Predicate::or).orElse(s -> false);
    }

    @Override
    public IncomingDatagramMessage read(InetAddress receivedOnAddress, DatagramPacket datagram) throws UnsupportedDataException {
        if (isWhiteListed(datagram.getAddress().getHostAddress())) {
            return super.read(receivedOnAddress, datagram);
        }

        throw new UnsupportedDataException(String.format("Unauthorized datagram source %s received on address %s", datagram.getAddress(), receivedOnAddress));
    }

    boolean isWhiteListed(String address) {
        return whiteListedAddresses.test(address);
    }

    private void setInclusiveHostCountForExclusions(Collection<SubnetUtils> exclusions) {
        exclusions.forEach(subnetUtils -> subnetUtils.setInclusiveHostCount(true));
    }
}
