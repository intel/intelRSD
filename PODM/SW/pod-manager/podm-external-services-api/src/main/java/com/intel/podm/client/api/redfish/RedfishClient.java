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

package com.intel.podm.client.api.redfish;

import com.intel.podm.client.api.redfish.response.RedfishException;
import com.intel.podm.client.api.redfish.response.RedfishResponse;
import com.intel.podm.common.concurrent.TaskScheduler;

import java.net.URI;
import java.util.concurrent.CompletableFuture;

public interface RedfishClient {
    URI getServiceRootUri();

    RedfishResponse get(String path) throws RedfishException;
    RedfishResponse patch(String path, Object request) throws RedfishException;
    CompletableFuture<RedfishResponse> patchAsync(String path, Object requestBody, TaskScheduler taskScheduler) throws RedfishException;
    CompletableFuture<RedfishResponse> deleteAsync(String path, TaskScheduler taskScheduler) throws RedfishException;
    CompletableFuture<RedfishResponse> postAsync(String path, TaskScheduler taskScheduler) throws RedfishException;
    CompletableFuture<RedfishResponse> postAsync(String path, Object requestBody, TaskScheduler taskScheduler) throws RedfishException;

    void close();
}
