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

package com.intel.podm.common.utils;

import java.net.Inet4Address;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.List;

import static java.util.Collections.emptyList;
import static java.util.Collections.list;
import static java.util.stream.Collectors.toList;

public final class Networking {
    private Networking() {
    }

    public static List<String> getIpAddressesForNetworkInterface(String networkInterfaceName) {
        NetworkInterface networkInterface;
        try {
            networkInterface = NetworkInterface.getByName(networkInterfaceName);

            if (networkInterface == null) {
                throw new IllegalArgumentException("There is no network interface with name: " + networkInterfaceName);
            }

            return list(networkInterface.getInetAddresses()).stream()
                .filter(inetAddress -> inetAddress instanceof Inet4Address)
                .map(inet4Address -> inet4Address.getHostAddress())
                .collect(toList());
        } catch (SocketException e) {
            return emptyList();
        }
    }
}
