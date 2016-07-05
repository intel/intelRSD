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
import com.intel.podm.common.types.AddressRangeType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

import java.util.List;

public final class MemoryChunkDto {
    private final String name;
    private final String description;
    private final Id id;
    private final String memoryChunkName;
    private final Integer memoryChunkUid;
    private final Integer memoryChunkSizeMib;
    private final AddressRangeType addressRangeType;
    private final Boolean isMirrorEnabled;
    private final Boolean isSpare;

    private final Status status;

    private final List<InterleaveSet> interleaveSets;

    private MemoryChunkDto(Builder builder) {
        name = builder.name;
        description = builder.description;
        id = builder.id;
        memoryChunkName = builder.memoryChunkName;
        memoryChunkUid = builder.memoryChunkUid;
        memoryChunkSizeMib = builder.memoryChunkSizeMib;
        addressRangeType = builder.addressRangeType;
        isMirrorEnabled = builder.isMirrorEnabled;
        isSpare = builder.isSpare;
        interleaveSets = builder.interleaveSets;
        status = builder.status;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public Id getId() {
        return id;
    }

    public String getMemoryChunkName() {
        return memoryChunkName;
    }

    public Integer getMemoryChunkUid() {
        return memoryChunkUid;
    }

    public Integer getMemoryChunkSizeMib() {
        return memoryChunkSizeMib;
    }

    public AddressRangeType getAddressRangeType() {
        return addressRangeType;
    }

    public Boolean isMirrorEnabled() {
        return isMirrorEnabled;
    }

    public Boolean isSpare() {
        return isSpare;
    }

    public List<InterleaveSet> getInterleaveSets() {
        return interleaveSets;
    }

    public Status getStatus() {
        return status;
    }


    public static final class InterleaveSet {
        private Context context;
        private String regionId;

        public InterleaveSet(Context context, String regionId) {
            this.context = context;
            this.regionId = regionId;
        }

        public Context getContext() {
            return context;
        }

        public String getRegionId() {
            return regionId;
        }
    }

    public static final class Builder {
        private String name;
        private String description;
        private Id id;
        private String memoryChunkName;
        private Integer memoryChunkUid;
        private Integer memoryChunkSizeMib;
        private AddressRangeType addressRangeType;
        private Boolean isMirrorEnabled;
        private Boolean isSpare;
        private List<InterleaveSet> interleaveSets;
        private Status status;

        private Builder() {
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder memoryChunkName(String val) {
            memoryChunkName = val;
            return this;
        }

        public Builder memoryChunkUid(Integer val) {
            memoryChunkUid = val;
            return this;
        }

        public Builder memoryChunkSizeMib(Integer val) {
            memoryChunkSizeMib = val;
            return this;
        }

        public Builder addressRangeType(AddressRangeType val) {
            addressRangeType = val;
            return this;
        }

        public Builder isMirrorEnabled(Boolean val) {
            isMirrorEnabled = val;
            return this;
        }

        public Builder isSpare(Boolean val) {
            isSpare = val;
            return this;
        }

        public Builder interleaveSets(List<InterleaveSet> val) {
            interleaveSets = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public MemoryChunkDto build() {
            return new MemoryChunkDto(this);
        }
    }
}
