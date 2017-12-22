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

import java.util.Map;

public final class UnknownOemDto {
    private String oemPath;
    private ObjectNode oemValue;
    private Map<String, Context> foundContexts;

    public String getOemPath() {
        return oemPath;
    }

    public void setOemPath(String oemPath) {
        this.oemPath = oemPath;
    }

    public ObjectNode getOemValue() {
        return oemValue;
    }

    public void setOemValue(ObjectNode oemValue) {
        this.oemValue = oemValue;
    }

    public Map<String, Context> getFoundContexts() {
        return foundContexts;
    }

    public void setFoundContexts(Map<String, Context> foundContexts) {
        this.foundContexts = foundContexts;
    }
}
