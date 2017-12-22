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

import com.google.common.util.concurrent.ListeningExecutorService;
import com.intel.podm.client.redfish.response.RedfishClientException;
import com.intel.podm.client.redfish.response.RedfishEntityResponseBody;
import com.intel.podm.client.redfish.response.RedfishResponse;
import com.intel.podm.client.redfish.response.RedfishResponseBody;
import com.intel.podm.client.redfish.http.HttpResponse;
import com.intel.podm.client.redfish.http.SimpleHttpClient;
import com.intel.podm.client.redfish.response.RedfishEntityResponseBodyImpl;
import com.intel.podm.client.redfish.response.RedfishResponseBodyImpl;
import com.intel.podm.common.concurrent.TaskScheduler;
import org.mockito.Mockito;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.net.URI;
import java.time.Duration;
import java.util.concurrent.Callable;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CompletionException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

import static com.google.common.util.concurrent.MoreExecutors.newDirectExecutorService;
import static com.intel.podm.common.types.net.HttpMethod.DELETE;
import static com.intel.podm.common.types.net.HttpMethod.GET;
import static com.intel.podm.common.types.net.HttpMethod.PATCH;
import static com.intel.podm.common.types.net.HttpMethod.POST;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.time.Duration.ZERO;
import static java.util.concurrent.CompletableFuture.supplyAsync;
import static org.mockito.Mockito.when;
import static org.mockito.Mockito.withSettings;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MagicNumber", "checkstyle:MethodName"})
public class RedfishClientTest {

    @Test
    public void whenGettingResource_shouldReturnCorrectResponse() throws RedfishClientException {
        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        RedfishEntityResponseBody expectedBody = mockEntity();

        when(client.call(GET, uri("http://host:123/redfish/v1/Xyz"), null, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(200, expectedBody, null));

        RedfishResponse actual = sut.get("/redfish/v1/Xyz");

        assertEquals(actual.getEntityOrNull(), expectedBody);
        assertFalse(actual.getLocation().isPresent());
    }

    @Test(expectedExceptions = RedfishClientException.class)
    public void whenGettingResourceWhichDoesNotExist_shouldThrow() throws RedfishClientException {
        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        RedfishResponseBody expected = mock(RedfishResponseBodyImpl.class);

        when(client.call(GET, uri("http://host:123/redfish/v1/error"), null, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(400, expected, null));

        sut.get("/redfish/v1/error");
    }

    @Test
    public void whenInvokingSyncAction_shouldReturnDoneFuture() throws ExecutionException, InterruptedException, RedfishClientException {

        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        RedfishEntityResponseBody expectedEntity = mockEntity();

        when(client.call(POST, uri("http://host:123/redfish/v1/Nodes/1/Actions/Assemble"), null, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(200, expectedEntity, null));

        Future<RedfishResponse> response = sut.postAsync("/redfish/v1/Nodes/1/Actions/Assemble", createTaskScheduler());

        assertTrue(response.isDone());
        assertFalse(response.isCancelled());
        assertEquals(response.get().getEntityOrNull(), expectedEntity);
    }

    @Test(invocationTimeOut = 5000)
    public void whenInvokingAsyncAction_shouldFinishAsynchronously() throws ExecutionException, InterruptedException, RedfishClientException {

        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        RedfishEntityResponseBody expectedEntity = mockEntity();

        URI taskMonitorUri = uri("/TaskMonitors/1");

        when(client.call(POST, uri("http://host:123/redfish/v1/Nodes/1/Actions/Assemble"), null, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(202, null, taskMonitorUri));

        HttpResponse taskMonitorResponse = new HttpResponse(202, null, null);
        HttpResponse result = new HttpResponse(200, expectedEntity, null);
        when(client.call(GET, uri("http://host:123/TaskMonitors/1"), null, RedfishResponseBodyImpl.class))
            .thenReturn(taskMonitorResponse, taskMonitorResponse, taskMonitorResponse, result);


        Future<RedfishResponse> response = sut.postAsync("/redfish/v1/Nodes/1/Actions/Assemble", createTaskScheduler());

        assertFalse(response.isCancelled());

        RedfishEntityResponseBody actualEntity = response.get().getEntityOrNull();
        assertEquals(actualEntity, expectedEntity);
    }

    @Test
    @SuppressWarnings({"unchecked"})
    public void whenAsyncActionFailed_shouldCompleteExceptionally() throws RedfishClientException, InterruptedException {
        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        URI taskMonitorUri = uri("/TaskMonitors/1");

        HttpResponse taskMonitorResponse = new HttpResponse(202, null, taskMonitorUri);
        when(client.call(POST, uri("http://host:123/redfish/v1/Nodes/2/Actions/Assemble"), null, RedfishResponseBodyImpl.class))
            .thenReturn(taskMonitorResponse);

        when(client.call(GET, uri("http://host:123/TaskMonitors/1"), null, RedfishResponseBodyImpl.class))
            .thenReturn(taskMonitorResponse, taskMonitorResponse).thenThrow(NullPointerException.class);

        CompletableFuture<RedfishResponse> future = sut.postAsync("/redfish/v1/Nodes/2/Actions/Assemble", createTaskScheduler());

        try {
            future.get();
            Assert.fail("Exception has not been thrown");
        } catch (ExecutionException e) {
            assertTrue(e.getCause() instanceof NullPointerException);
        }
    }

    @Test
    public void whenPostingWithBody_shouldPassBodyObjectToInternalHttpClient() throws ExecutionException, InterruptedException, RedfishClientException {

        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        Object request = new Object();
        RedfishEntityResponseBody expectedEntity = mockEntity();

        when(client.call(POST, uri("http://host:123/redfish/v1/Nodes/2/Actions/Assemble"), request, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(200, expectedEntity, null));

        Future<RedfishResponse> future = sut.postAsync("/redfish/v1/Nodes/2/Actions/Assemble", request, createTaskScheduler());

        assertEquals(future.get().getEntityOrNull(), expectedEntity);
    }

    @Test
    public void whenDeleting_shouldDeleteAndReturnCorrectResponse() throws ExecutionException, InterruptedException, RedfishClientException {

        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        when(client.call(DELETE, uri("http://host:123/redfish/v1/Nodes/3"), null, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(204, null, null));

        Future<RedfishResponse> future = sut.deleteAsync("/redfish/v1/Nodes/3", createTaskScheduler());

        assertFalse(future.get().getEntity().isPresent());
    }

    private TaskScheduler createTaskScheduler() {
        return new CurrentThreadTaskSchedulerWithoutDelay();
    }

    @Test
    public void whenPatching_shouldPatchAndReturnCorrectResponse() throws RedfishClientException {
        SimpleHttpClient client = mock(SimpleHttpClient.class);
        RedfishClient sut = createSimpleRedfishClient(client);

        Object request = new Object();
        RedfishEntityResponseBody expectedEntity = mockEntity();

        when(client.call(PATCH, uri("http://host:123/redfish/v1/Nodes/2"), request, RedfishResponseBodyImpl.class))
            .thenReturn(new HttpResponse(200, expectedEntity, null));

        RedfishResponse response = sut.patch("/redfish/v1/Nodes/2", request);

        assertEquals(response.getEntityOrNull(), expectedEntity);
    }

    <T> T mock(Class<T> clazz) {
        return Mockito.mock(clazz, withSettings().verboseLogging());
    }

    private RedfishClient createSimpleRedfishClient(SimpleHttpClient client) {
        SocketErrorAwareHttpClient socketErrorAwareHttpClient = new SocketErrorAwareHttpClient(client);
        return new RedfishClient(uri("http://host:123/redfish/v1"), socketErrorAwareHttpClient);
    }

    private URI uri(String uri) {
        return URI.create(uri);
    }

    private RedfishEntityResponseBody mockEntity() {
        return mock(RedfishEntityResponseBodyImpl.class);
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private static final class CurrentThreadTaskSchedulerWithoutDelay implements TaskScheduler {
        final ListeningExecutorService executorService = newDirectExecutorService();

        @Override
        public <T> CompletableFuture<T> schedule(Callable<T> callable, Duration delay) {
            requiresNonNull(callable, "callable");
            requiresNonNull(delay, "delay");

            return supplyAsync(() -> {
                try {
                    return callable.call();
                } catch (Exception e) {
                    throw new CompletionException(e);
                }
            }, executorService);
        }

        @Override
        public <T> CompletableFuture<T> submit(Callable<T> callable) {
            return schedule(callable, ZERO);
        }
    }
}
