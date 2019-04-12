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

package com.intel.rsd.nodecomposer.externalservices.redfish;

import com.intel.rsd.nodecomposer.externalservices.redfish.http.HttpRequest;
import com.intel.rsd.nodecomposer.externalservices.redfish.http.HttpResponse;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishClientException;
import com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishResponse;
import com.intel.rsd.nodecomposer.concurrent.TaskScheduler;

import java.net.URI;
import java.time.Duration;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;

import static com.intel.rsd.nodecomposer.externalservices.redfish.response.RedfishResponseFactory.redfishResponseFromHttpResponse;
import static com.intel.rsd.nodecomposer.types.net.HttpMethod.GET;
import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.ACCEPTED;
import static java.time.Duration.ofSeconds;

final class RedfishTaskMonitor extends CompletableFuture<RedfishResponse> {
    private final SocketErrorAwareHttpClient socketErrorAwareHttpClient;
    private final Duration recheckPeriod = ofSeconds(10);
    private final TaskScheduler taskScheduler;
    private final String monitorPath;
    private final URI serviceRootUri;
    private final HttpRequest httpRequest;

    RedfishTaskMonitor(SocketErrorAwareHttpClient socketErrorAwareHttpClient, URI serviceRootUri,
                       HttpRequest request, HttpResponse response, TaskScheduler taskScheduler) {
        this.socketErrorAwareHttpClient = socketErrorAwareHttpClient;
        this.taskScheduler = taskScheduler;
        this.serviceRootUri = serviceRootUri;
        this.httpRequest = request;
        if (isDone(response)) {
            this.monitorPath = null;
            onDone(response);
        } else {
            this.monitorPath = response.getLocation()
                .orElseThrow(() -> new IllegalArgumentException("Only responses containing Location may be monitored!"))
                .getPath();
            checkIfDone(response);
        }
    }

    private boolean isDone(HttpResponse response) {
        return !Objects.equals(response.getStatusCode(), ACCEPTED);
    }

    private void onDone(HttpResponse httpResponse) {
        try {
            complete(redfishResponseFromHttpResponse(httpResponse, httpRequest));
        } catch (RedfishClientException e) {
            completeExceptionally(e);
        }
    }

    private void checkIfDone(HttpResponse response) {
        taskScheduler.schedule(() -> {
            try {
                if (!isDone(response)) {
                    checkIfDone(socketErrorAwareHttpClient.call(new HttpRequest(GET, serviceRootUri.resolve(monitorPath), null)));
                } else {
                    onDone(response);
                }
            } catch (RedfishClientException e) {
                completeExceptionally(e);
            }
        }, recheckPeriod).exceptionally(e -> {
            completeExceptionally(e);
            return null;
        });
    }
}
