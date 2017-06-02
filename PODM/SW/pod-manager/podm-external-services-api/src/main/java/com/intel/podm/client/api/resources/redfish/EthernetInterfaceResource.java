/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.api.resources.redfish;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public interface EthernetInterfaceResource extends ExternalServiceResource {
    Ref<Boolean> getInterfaceEnabled();
    Ref<Integer> getSpeedMbps();
    Ref<Boolean> getAutoNeg();
    Ref<Boolean> isFullDuplex();
    Ref<MacAddress> getPermanentMacAddress();
    Ref<MacAddress> getMacAddress();
    Ref<String> getHostName();
    Ref<String> getFqdn();
    Ref<Integer> getMaxIPv6StaticAddresses();
    Ref<String> getIpV6DefaultGateway();
    Ref<List<String>> getNameServers();
    Ref<List<IpV4AddressObject>> getIpV4Addresses();
    Ref<List<IpV6AddressObject>> getIpV6Addresses();
    Ref<List<IpV6AddressObject>> getIpV6StaticAddresses();
    Ref<List<IpV6AddressPolicyObject>> getIpV6AddressesPolicyTable();
    Ref<Integer> getMtuSize();
    Ref<Boolean> getVlanEnable();
    Ref<Status> getStatus();
    Ref<Integer> getVlanId();

    ResourceSupplier getComputerSystem();

    Iterable<ResourceSupplier> getVlans() throws ExternalServiceApiReaderException;
}
