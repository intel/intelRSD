/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.common.types.redfish;

import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;
import com.intel.podm.common.types.Ref;

public interface RedfishNetworkDeviceFunction extends RedfishResource {
    Ref<Boolean> getDeviceEnabled();

    Ref<IscsiBoot> getIscsiBoot();

    Ref<Ethernet> getEthernet();

    interface Ethernet {
        Ref<String> getMacAddress();
    }

    @SuppressWarnings({"checkstyle:MethodCount"})
    interface IscsiBoot {
        Ref<IpAddressType> getIpAddressType();

        Ref<String> getInitiatorIpAddress();

        Ref<String> getInitiatorName();

        Ref<String> getInitiatorDefaultGateway();

        Ref<String> getInitiatorNetmask();

        Ref<Boolean> getTargetInfoViaDhcp();

        Ref<String> getPrimaryTargetName();

        Ref<String> getPrimaryTargetIpAddress();

        Ref<Integer> getPrimaryTargetTcpPort();

        Ref<Integer> getPrimaryLun();

        Ref<Boolean> getPrimaryVlanEnable();

        Ref<Integer> getPrimaryVlanId();

        Ref<String> getPrimaryDns();

        Ref<String> getSecondaryTargetName();

        Ref<String> getSecondaryTargetIpAddress();

        Ref<Integer> getSecondaryTargetTcpPort();

        Ref<Integer> getSecondaryLun();

        Ref<Boolean> getSecondaryVlanEnable();

        Ref<Integer> getSecondaryVlanId();

        Ref<String> getSecondaryDns();

        Ref<Boolean> getIpMaskDnsViaDhcp();

        Ref<Boolean> getRouterAdvertisementEnabled();

        Ref<AuthenticationMethod> getAuthenticationMethod();

        Ref<String> getChapUsername();

        Ref<String> getChapSecret();

        Ref<String> getMutualChapUsername();

        Ref<String> getMutualChapSecret();
    }
}
