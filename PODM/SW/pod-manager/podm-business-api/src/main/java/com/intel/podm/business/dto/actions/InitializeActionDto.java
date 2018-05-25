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

package com.intel.podm.business.dto.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.intel.podm.business.services.redfish.requests.InitializeRequest;
import com.intel.podm.common.types.actions.InitializeType;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

public class InitializeActionDto implements InitializeRequest {

    @JsonInclude(NON_NULL)
    private InitializeType initializeType;

    public InitializeActionDto() {
    }

    public InitializeActionDto(InitializeType initializeType) {
        this.initializeType = initializeType;
    }

    @Override
    public InitializeType getInitializeType() {
        return initializeType;
    }

}
