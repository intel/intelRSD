/*
 * Copyright (c) 2017 Intel Corporation
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

import com.intel.podm.common.types.ChapType;

public final class ChapDto {
    private final ChapType type;
    private final String username;
    private final String mutualUsername;

    private ChapDto(Builder builder) {
        type = builder.type;
        username = builder.username;
        mutualUsername = builder.mutualUsername;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public ChapType getType() {
        return type;
    }

    public String getUsername() {
        return username;
    }

    public String getMutualUsername() {
        return mutualUsername;
    }

    public static final class Builder {
        private ChapType type;
        private String username;
        private String mutualUsername;

        private Builder() {
        }

        public Builder type(ChapType type) {
            this.type = type;
            return this;
        }

        public Builder username(String username) {
            this.username = username;
            return this;
        }

        public Builder mutualUsername(String mutualUsername) {
            this.mutualUsername = mutualUsername;
            return this;
        }

        public ChapDto build() {
            return new ChapDto(this);
        }
    }
}
