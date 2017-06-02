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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Status;

import java.util.Set;

public final class RedundancyDto {
    private final String memberId;
    private final String name;
    private final Boolean redundancyEnabled;
    private final Set<Context> redundancySet;
    private final String mode;
    private final Status status;
    private final Integer minNumNeeded;
    private final Integer maxNumSupported;
    private final Context context;

    private RedundancyDto(Builder builder) {
        this.memberId = builder.memberId;
        this.name = builder.name;
        this.redundancyEnabled = builder.redundancyEnabled;
        this.redundancySet = builder.redundancySet;
        this.mode = builder.mode;
        this.status = builder.status;
        this.minNumNeeded = builder.minNumNeeded;
        this.maxNumSupported = builder.maxNumSupported;
        this.context = builder.context;
    }

    public static Builder newRedundancyDto() {
        return new Builder();
    }

    public String getMemberId() {
        return memberId;
    }

    public String getName() {
        return name;
    }

    public Boolean getRedundancyEnabled() {
        return redundancyEnabled;
    }

    public Set<Context> getRedundancySet() {
        return redundancySet;
    }

    public String getMode() {
        return mode;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getMinNumNeeded() {
        return minNumNeeded;
    }

    public Integer getMaxNumSupported() {
        return maxNumSupported;
    }

    public Context getContext() {
        return context;
    }

    public static final class Builder {
        private String memberId;
        private String name;
        private Boolean redundancyEnabled;
        private Set<Context> redundancySet;
        private String mode;
        private Status status;
        private Integer minNumNeeded;
        private Integer maxNumSupported;
        private Context context;

        private Builder() {
        }

        public RedundancyDto build() {
            return new RedundancyDto(this);
        }

        public Builder memberId(String memberId) {
            this.memberId = memberId;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder redundancyEnabled(Boolean redundancyEnabled) {
            this.redundancyEnabled = redundancyEnabled;
            return this;
        }

        public Builder redundancySet(Set<Context> redundancySet) {
            this.redundancySet = redundancySet;
            return this;
        }

        public Builder mode(String mode) {
            this.mode = mode;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder minNumNeeded(Integer minNumNeeded) {
            this.minNumNeeded = minNumNeeded;
            return this;
        }

        public Builder maxNumSupported(Integer maxNumSupported) {
            this.maxNumSupported = maxNumSupported;
            return this;
        }

        public Builder context(Context context) {
            this.context = context;
            return this;
        }
    }
}
