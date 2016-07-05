/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

public interface NetworkProtocolResource extends ExternalServiceResource {
    Status getStatus();
    String getHostName();
    String getFqdn();
    Integer getKvmIpPort();
    Boolean getKvmIpProtocolEnabled();
    Integer getTelnetPort();
    Boolean getTelnetProtocolEnabled();
    Integer getSsdpNotifyTtl();
    Integer getSsdpNotifyMulticastIntervalSeconds();
    NotifyIpV6Scope getSsdpNotifyIpV6Scope();
    Integer getSsdpPort();
    Boolean getSsdpProtocolEnabled();
    Integer getVirtualMediaPort();
    Boolean getVirtualMediaProtocolEnabled();
    Integer getSnmpPort();
    Boolean getSnmpProtocolEnabled();
    Integer getSshPort();
    Boolean getSshProtocolEnabled();
    Integer getIpmiPort();
    Boolean getIpmiProtocolEnabled();
    Integer getHttpsPort();
    Boolean getHttpsProtocolEnabled();
    Integer getHttpPort();
    Boolean getHttpProtocolEnabled();
}
