/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.ipxesupplier;

import java.text.MessageFormat;
import java.util.Objects;

public final class SanbootIpxeScript implements IpxeScript {
    private final Integer port;
    private final String serverName;
    private final String lun;
    private final String targetName;
    private final String initiatorIqn;

    private SanbootIpxeScript(Builder builder) {
        initiatorIqn = builder.initiatorIqn;
        port = builder.port;
        serverName = builder.serverName;
        lun = builder.lun;
        targetName = builder.targetName;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    @Override
    public String toIpxeScript() {
        return MessageFormat.format(
                "#!ipxe\n"
                + "set initiator-iqn {0}\n"
                + "sanboot iscsi:{1}::{2}:{3}:{4}",
                Objects.toString(initiatorIqn, ""),
                Objects.toString(serverName, ""),
                Objects.toString(port, ""),
                Objects.toString(lun, ""),
                Objects.toString(targetName, "")
        );
    }

    public static final class Builder {
        private String initiatorIqn;
        private Integer port;
        private String serverName;
        private String lun;
        private String targetName;

        private Builder() {
        }

        public Builder initiatorIqn(String initiatorIqn) {
            this.initiatorIqn = initiatorIqn;
            return this;
        }

        public Builder port(Integer port) {
            this.port = port;
            return this;
        }

        public Builder serverName(String serverName) {
            this.serverName = serverName;
            return this;
        }

        public Builder lun(String lun) {
            this.lun = lun;
            return this;
        }

        public Builder targetName(String targetName) {
            this.targetName = targetName;
            return this;
        }

        public SanbootIpxeScript build() {
            return new SanbootIpxeScript(this);
        }
    }
}
