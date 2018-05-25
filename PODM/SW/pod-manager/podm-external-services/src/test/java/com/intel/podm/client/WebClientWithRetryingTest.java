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

package com.intel.podm.client;

import com.intel.podm.client.redfish.response.RedfishClientException;
import org.testng.annotations.Test;

import java.net.SocketException;
import java.net.URI;

import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.fail;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class WebClientWithRetryingTest {

    @Test
    public void whenGettingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            when(mock.get(any())).thenThrow(connectionException());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.get(any());
        } catch (WebClientRequestException e) {
            verify(mock, times(3)).get(any());
            return;
        }
        fail("WebClientRequestException was not thrown");
    }

    @Test
    public void whenPostingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            when(mock.post(any(), any())).thenThrow(connectionException());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.post(any(), any());
        } catch (WebClientRequestException e) {
            verify(mock, times(3)).post(any(), any());
            return;
        }
        fail("ExternalServiceApiActionException was not thrown");
    }

    @Test
    public void whenPatchingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            doThrow(connectionException()).when(mock).patch(any(), any());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.patch(any(), any());
        } catch (WebClientRequestException e) {
            verify(mock, times(3)).patch(any(), any());
            return;
        }
        fail("ExternalServiceApiActionException was not thrown");
    }

    @Test
    public void whenDeletingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            doThrow(connectionException()).when(mock).delete(any());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.delete(any());
        } catch (WebClientRequestException e) {
            verify(mock, times(3)).delete(any());
            return;
        }
        fail("ExternalServiceApiActionException was not thrown");
    }

    @Test(expectedExceptions = {WebClientRequestException.class})
    public void whenGettingValidUri_shouldThrowExternalServiceApiReaderException() throws Exception {
        WebClient mock = mock(WebClient.class);
        when(mock.get(any())).thenThrow(nonConnectionException());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.get(any());
    }

    @Test(expectedExceptions = {WebClientRequestException.class})
    public void whenPostingValidUri_shouldThrowExternalServiceApiActionException() throws Exception {
        WebClient mock = mock(WebClient.class);
        when(mock.post(any(), any())).thenThrow(nonConnectionException());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.post(any(), any());
    }

    @Test(expectedExceptions = {WebClientRequestException.class})
    public void whenPatchingValidUri_shouldThrowExternalServiceApiActionException() throws Exception {
        WebClient mock = mock(WebClient.class);
        doThrow(nonConnectionException()).when(mock).patch(any(), any());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.patch(any(), any());
    }

    @Test(expectedExceptions = {WebClientRequestException.class})
    public void whenDeletingValidUri_shouldThrowExternalServiceApiActionException() throws Exception {
        WebClient mock = mock(WebClient.class);
        doThrow(nonConnectionException()).when(mock).patch(any(), any());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.patch(any(), any());
    }

    private WebClientRequestException nonConnectionException() {
        return new WebClientRequestException("", URI.create(""), new IllegalStateException());
    }

    private WebClientRequestException connectionException() {
        return new WebClientRequestException("", URI.create(""), createRedfishClientExceptionCausedBySocketConnectionException());
    }

    private RedfishClientException createRedfishClientExceptionCausedBySocketConnectionException() {
        return new RedfishClientException(
            new WebClientConnectionException("", URI.create(""), new SocketException(""))
        );
    }
}
