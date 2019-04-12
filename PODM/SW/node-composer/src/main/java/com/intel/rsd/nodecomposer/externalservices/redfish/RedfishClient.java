/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.redfish;

import com.intel.rsd.nodecomposer.concurrent.TaskScheduler;
import com.intel.rsd.nodecomposer.externalservices.redfish.http.HttpRequest;
import com.intel.rsd.nodecomposer.externalservices.redfish.http.HttpResponse;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishClientException;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishResponse;
import com.intel.rsd.nodecomposer.types.net.HttpMethod;

import java.net.URI;
import java.util.concurrent.CompletableFuture;

import static com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishResponseFactory.redfishResponseFromHttpResponse;
import static com.intel.rsd.nodecomposer.types.net.HttpMethod.DELETE;
import static com.intel.rsd.nodecomposer.types.net.HttpMethod.GET;
import static com.intel.rsd.nodecomposer.types.net.HttpMethod.PATCH;
import static com.intel.rsd.nodecomposer.types.net.HttpMethod.POST;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

public final class RedfishClient implements AutoCloseable {
    private final URI serviceRootUri;
    private final SocketErrorAwareHttpClient socketErrorAwareHttpClient;

    public RedfishClient(URI serviceRootUri, SocketErrorAwareHttpClient socketErrorAwareHttpClient) {
        this.serviceRootUri = requiresNonNull(serviceRootUri, "serviceRootUri");
        this.socketErrorAwareHttpClient = requiresNonNull(socketErrorAwareHttpClient, "socketErrorAwareHttpClient");
    }

    public URI getServiceRootUri() {
        return serviceRootUri;
    }

    public RedfishResponse get(String path) throws RedfishClientException {
        requiresNonNull(path, "path");
        HttpRequest httpRequest = new HttpRequest(GET, serviceRootUri.resolve(path), null);
        HttpResponse httpResponse = socketErrorAwareHttpClient.call(httpRequest);
        return redfishResponseFromHttpResponse(httpResponse, httpRequest);
    }

    public RedfishResponse patch(String path, Object requestBody) throws RedfishClientException {
        return invokeHttpCall(PATCH, path, requestBody);
    }

    public RedfishResponse post(String path, Object requestBody) throws RedfishClientException {
        return invokeHttpCall(POST, path, requestBody);
    }

    private RedfishResponse invokeHttpCall(HttpMethod method, String path, Object requestBody) throws RedfishClientException {
        requiresNonNull(path, "path");
        requiresNonNull(requestBody, "requestBody");

        HttpRequest httpRequest = new HttpRequest(method, serviceRootUri.resolve(path), requestBody);
        HttpResponse httpResponse = socketErrorAwareHttpClient.call(httpRequest);
        return redfishResponseFromHttpResponse(httpResponse, httpRequest);
    }

    public CompletableFuture<RedfishResponse> patchAsync(String path, Object requestBody, TaskScheduler taskScheduler) throws RedfishClientException {
        requiresNonNull(requestBody, "requestBody");
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return monitorRequest(new HttpRequest(PATCH, serviceRootUri.resolve(path), requestBody), taskScheduler);
    }

    public CompletableFuture<RedfishResponse> deleteAsync(String path, TaskScheduler taskScheduler) throws RedfishClientException {
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return monitorRequest(new HttpRequest(DELETE, serviceRootUri.resolve(path), null), taskScheduler);
    }

    public CompletableFuture<RedfishResponse> postAsync(String path, TaskScheduler taskScheduler) throws RedfishClientException {
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return monitorRequest(new HttpRequest(POST, serviceRootUri.resolve(path), null), taskScheduler);
    }

    public CompletableFuture<RedfishResponse> postAsync(String path, Object requestBody, TaskScheduler taskScheduler) throws RedfishClientException {
        requiresNonNull(requestBody, "requestBody");
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return monitorRequest(new HttpRequest(POST, serviceRootUri.resolve(path), requestBody), taskScheduler);
    }

    private RedfishTaskMonitor monitorRequest(HttpRequest httpRequest, TaskScheduler taskScheduler) throws RedfishClientException {
        return new RedfishTaskMonitor(
            socketErrorAwareHttpClient,
            serviceRootUri,
            httpRequest,
            socketErrorAwareHttpClient.call(httpRequest),
            taskScheduler
        );
    }

    public void close() {
        socketErrorAwareHttpClient.close();
    }
}
