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

package com.intel.podm.client.redfish;

import com.intel.podm.client.api.redfish.RedfishClient;
import com.intel.podm.client.api.redfish.response.RedfishException;
import com.intel.podm.client.api.redfish.response.RedfishResponse;
import com.intel.podm.client.redfish.http.HttpResponse;
import com.intel.podm.client.redfish.http.SimpleHttpClient;
import com.intel.podm.client.redfish.response.RedfishResponseBodyImpl;
import com.intel.podm.client.redfish.response.RedfishResponseFactory;
import com.intel.podm.common.concurrent.TaskScheduler;

import javax.ws.rs.ProcessingException;
import java.net.URI;
import java.time.Duration;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;

import static com.intel.podm.client.redfish.http.HttpStatusCode.ACCEPTED;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.time.Duration.ofSeconds;

public final class RedfishClientImpl implements AutoCloseable, RedfishClient {
    private final URI serviceRootUri;
    private final SimpleHttpClient client;

    public RedfishClientImpl(URI serviceRootUri, SimpleHttpClient client) {
        this.serviceRootUri = requiresNonNull(serviceRootUri, "serviceRootUri");
        this.client = requiresNonNull(client, "client");
    }

    @Override
    public URI getServiceRootUri() {
        return serviceRootUri;
    }

    @Override
    public RedfishResponse get(String path) throws RedfishException {
        requiresNonNull(path, "path");

        return invoke("GET", path, null);
    }

    @Override
    public RedfishResponse patch(String path, Object request) throws RedfishException {
        requiresNonNull(path, "path");
        requiresNonNull(request, "request");

        return invoke("PATCH", path, request);
    }

    @Override
    public CompletableFuture<RedfishResponse> patchAsync(String path, Object requestBody, TaskScheduler taskScheduler) throws RedfishException {
        requiresNonNull(requestBody, "requestBody");
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return new TaskMonitor(call("PATCH", path, requestBody), taskScheduler);
    }

    @Override
    public CompletableFuture<RedfishResponse> deleteAsync(String path, TaskScheduler taskScheduler) throws RedfishException {
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return new TaskMonitor(call("DELETE", path, null), taskScheduler);
    }

    @Override
    public CompletableFuture<RedfishResponse> postAsync(String path, TaskScheduler taskScheduler) throws RedfishException {
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return new TaskMonitor(call("POST", path, null), taskScheduler);
    }

    @Override
    public CompletableFuture<RedfishResponse> postAsync(String path, Object requestBody, TaskScheduler taskScheduler) throws RedfishException {
        requiresNonNull(requestBody, "requestBody");
        requiresNonNull(path, "path");
        requiresNonNull(taskScheduler, "taskScheduler");

        return new TaskMonitor(call("POST", path, requestBody), taskScheduler);
    }

    @Override
    public void close() {
        client.close();
    }

    private RedfishResponse invoke(String methodName, String path, Object requestBody) throws RedfishException {
        URI uri = serviceRootUri.resolve(path);
        return RedfishResponseFactory.invoke(() -> client.call(methodName, uri, requestBody, RedfishResponseBodyImpl.class));
    }

    private HttpResponse call(String methodName, String path, Object requestBody) throws RedfishException {
        URI uri = serviceRootUri.resolve(path);

        try {
            return client.call(methodName, uri, requestBody, RedfishResponseBodyImpl.class);
        } catch (ProcessingException e) {
            throw new RedfishException(e);
        }
    }

    private final class TaskMonitor extends CompletableFuture<RedfishResponse> {
        private final Duration recheckPeriod = ofSeconds(10);
        private final TaskScheduler taskScheduler;
        private final String monitorPath;


        TaskMonitor(HttpResponse response, TaskScheduler taskScheduler) {
            this.taskScheduler = taskScheduler;
            if (isDone(response)) {
                this.monitorPath = null;
                complete(response);
            } else {
                this.monitorPath = response.getLocation().get().getPath();
                checkIfDone(response);
            }
        }

        void checkIfDone(HttpResponse response) {
            taskScheduler.schedule(() -> {
                try {
                    if (!isDone(response)) {
                        checkIfDone(call("GET", monitorPath, null));
                    } else {
                        complete(response);
                    }
                } catch (RedfishException e) {
                    completeExceptionally(e);
                }
            }, recheckPeriod).exceptionally(e -> {
                completeExceptionally(e);
                return null;
            });
        }

        private void complete(HttpResponse httpResponse) {
            try {
                complete(RedfishResponseFactory.create(httpResponse));
            } catch (RedfishException e) {
                completeExceptionally(e);
            }
        }

        private boolean isDone(HttpResponse response) {
            return !Objects.equals(response.getStatusCode(), ACCEPTED);
        }
    }
}
