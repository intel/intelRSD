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

package com.intel.podm.redfish;

import java.util.HashMap;
import java.util.Map;

public final class RedfishResponseHeadersProvider {
    private RedfishResponseHeadersProvider() {
    }

    public static Map<String, String> getRedfishResponseHeaders() {
        Map<String, String> headers = new HashMap<>();
        headers.put("OData-Version", "4.0");
        headers.put("Cache-Control", "max-age=0");
        headers.put("Access-Control-Allow-Origin", "*");
        return headers;
    }
}
