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

package com.intel.podm.client.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.actions.DriveUpdateDefinition;

public class DriveUpdateRequest {

    @JsonProperty("AssetTag")
    private String assetTag;

    public DriveUpdateRequest(DriveUpdateDefinition driveUpdateDefinition) {
        this.assetTag = driveUpdateDefinition.getAssetTag();
    }
}
