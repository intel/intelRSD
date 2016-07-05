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

package com.intel.podm.business.entities.base.converters;

import com.intel.podm.common.types.NeighborInfo;

public class NeighborInfoConverter implements PropertyTypeConverter<String, NeighborInfo> {
    @Override
    public String convertFromMappedType(NeighborInfo neighborInfo) {
        return neighborInfo == null
                ? null
                : neighborInfo.toString();
    }

    @Override
    public NeighborInfo convertToMappedType(String neighborInfoString) {
        return neighborInfoString == null
                ? null
                : new NeighborInfo(neighborInfoString);
    }
}

