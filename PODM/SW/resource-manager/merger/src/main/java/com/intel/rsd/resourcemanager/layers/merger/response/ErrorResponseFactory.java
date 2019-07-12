/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.merger.response;

import com.intel.rsd.redfish.RedfishError;
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.experimental.UtilityClass;
import lombok.val;

import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static org.springframework.http.ResponseEntity.status;

@UtilityClass
final class ErrorResponseFactory {
    public static Response createErrorResponse(RedfishError redfishError) {
        val responseEntity = status(redfishError.getHttpStatus()).body(objectToJsonNode(redfishError));
        return new Response(responseEntity);
    }
}
