/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.common.types.NotifyIpV6Scope;

public final class SimpleServiceDiscoveryProtocolDto {
    private final Boolean protocolEnabled;
    private final Integer port;
    private final Integer notifyMulticastIntervalSeconds;
    private final Integer notifyTtl;
    private final NotifyIpV6Scope notifyIpV6Scope;

    private SimpleServiceDiscoveryProtocolDto(Builder builder) {
        protocolEnabled = builder.protocolEnabled;
        port = builder.port;
        notifyMulticastIntervalSeconds = builder.notifyMulticastIntervalSeconds;
        notifyTtl = builder.notifyTtl;
        notifyIpV6Scope = builder.notifyIpV6Scope;
    }

    public Boolean isProtocolEnabled() {
        return protocolEnabled;
    }

    public Integer getPort() {
        return port;
    }

    public Integer getNotifyMulticastIntervalSeconds() {
        return notifyMulticastIntervalSeconds;
    }

    public Integer getNotifyTtl() {
        return notifyTtl;
    }

    public NotifyIpV6Scope getNotifyIpV6Scope() {
        return notifyIpV6Scope;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private Boolean protocolEnabled;
        private Integer port;
        private Integer notifyMulticastIntervalSeconds;
        private Integer notifyTtl;
        private NotifyIpV6Scope notifyIpV6Scope;

        private Builder() {
        }

        public Builder protocolEnabled(Boolean protocolEnabled) {
            this.protocolEnabled = protocolEnabled;
            return this;
        }

        public Builder port(Integer port) {
            this.port = port;
            return this;
        }

        public Builder notifyMulticastIntervalSeconds(Integer notifyMulticastIntervalSeconds) {
            this.notifyMulticastIntervalSeconds = notifyMulticastIntervalSeconds;
            return this;
        }

        public Builder notifyTtl(Integer notifyTtl) {
            this.notifyTtl = notifyTtl;
            return this;
        }

        public Builder notifyIpV6Scope(NotifyIpV6Scope notifyIpV6Scope) {
            this.notifyIpV6Scope = notifyIpV6Scope;
            return this;
        }

        public SimpleServiceDiscoveryProtocolDto build() {
            return new SimpleServiceDiscoveryProtocolDto(this);
        }
    }
}
