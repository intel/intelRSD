/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.dto.redfish.attributes.ODataServiceDto;

import java.util.List;

public final class ODataServiceDocumentDto {
    private final List<ODataServiceDto> values;

    private ODataServiceDocumentDto(Builder builder) {
        this.values = builder.values;
    }

    public List<ODataServiceDto> getValues() {
        return values;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private List<ODataServiceDto> values;

        private Builder() {
        }

        public ODataServiceDocumentDto build() {
            return new ODataServiceDocumentDto(this);
        }

        public Builder values(List<ODataServiceDto> values) {
            this.values = values;
            return this;
        }
    }
}
