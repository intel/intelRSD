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

package com.intel.podm.business.dto.redfish.attributes;

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.podm.business.services.context.Context;

import java.util.HashMap;
import java.util.Map;

public final class UnknownOemDto {
    private final String oemPath;
    private final ObjectNode oemValue;
    private final Map<String, Context> foundContexts;

    private UnknownOemDto(Builder builder) {
        this.oemPath = builder.oemPath;
        this.oemValue = builder.oemValue;
        this.foundContexts = builder.foundContexts;
    }

    public String getOemPath() {
        return oemPath;
    }

    public ObjectNode getOemValue() {
        return oemValue;
    }

    public Map<String, Context> getFoundContexts() {
        return foundContexts;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String oemPath;
        private ObjectNode oemValue;
        private Map<String, Context> foundContexts = new HashMap<>();

        private Builder() {
        }

        public UnknownOemDto build() {
            return new UnknownOemDto(this);
        }

        public Builder oemPath(String oemPath) {
            this.oemPath = oemPath;
            return this;
        }

        public Builder oemValue(ObjectNode oemValue) {
            this.oemValue = oemValue;
            return this;
        }

        public Builder foundContexts(Map<String, Context> contexts) {
            this.foundContexts.putAll(contexts);
            return this;
        }
    }
}
