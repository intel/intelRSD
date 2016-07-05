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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.services.context.Context;

import java.util.List;

public final class RemoteTargetIscsiAddressDto {

    private final List<TargetLunDto> targetLun;
    private final String targetIqn;
    private final String targetPortalIp;
    private final Integer targetPortalPort;

    private RemoteTargetIscsiAddressDto(Builder builder) {
        this.targetLun = builder.targetLun;
        this.targetIqn = builder.targetIqn;
        this.targetPortalIp = builder.targetPortalIp;
        this.targetPortalPort = builder.targetPortalPort;
    }

    public List<TargetLunDto> getTargetLun() {
        return targetLun;
    }

    public String getTargetIqn() {
        return targetIqn;
    }

    public String getTargetPortalIp() {
        return targetPortalIp;
    }

    public int getTargetPortalPort() {
        return targetPortalPort;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {

        private List<TargetLunDto> targetLun;
        private String targetIqn;
        private String targetPortalIp;
        private Integer targetPortalPort;

        private Builder() {

        }

        public RemoteTargetIscsiAddressDto build() {
            return new RemoteTargetIscsiAddressDto(this);
        }

        public Builder targetPortalPort(Integer targetPortalPort) {
            this.targetPortalPort = targetPortalPort;
            return this;
        }

        public Builder targetPortalIp(String targetPortalIp) {
            this.targetPortalIp = targetPortalIp;
            return this;
        }

        public Builder targetIqn(String targetIqn) {
            this.targetIqn = targetIqn;
            return this;
        }

        public Builder targetLun(List<TargetLunDto> targetLun) {
            this.targetLun = targetLun;
            return this;
        }
    }

    public static class TargetLunDto {
        private final Integer lun;
        private final Context logicalDrive;

        public TargetLunDto(Integer lun, Context logicalDrive) {
            this.lun = lun;
            this.logicalDrive = logicalDrive;
        }

        public Integer getLun() {
            return lun;
        }

        public Context getLogicalDrive() {
            return logicalDrive;
        }
    }
}
