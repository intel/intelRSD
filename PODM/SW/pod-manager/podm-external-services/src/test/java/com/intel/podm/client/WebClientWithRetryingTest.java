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

package com.intel.podm.client;

import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import org.testng.annotations.Test;

import java.net.ConnectException;
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
            when(mock.get(any())).thenThrow(connectionExceptionOnReading());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.get(any());
        } catch (ExternalServiceApiReaderException e) {
            verify(mock, times(3)).get(any());
            return;
        }
        fail("ExternalServiceApiReaderException was not thrown");
    }

    @Test
    public void whenPostingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            when(mock.post(any(), any())).thenThrow(connectionExceptionOnAction());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.post(any(), any());
        } catch (ExternalServiceApiActionException e) {
            verify(mock, times(3)).post(any(), any());
            return;
        }
        fail("ExternalServiceApiActionException was not thrown");
    }

    @Test
    public void whenPatchingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            doThrow(connectionExceptionOnAction()).when(mock).patch(any(), any());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.patch(any(), any());
        } catch (ExternalServiceApiActionException e) {
            verify(mock, times(3)).patch(any(), any());
            return;
        }
        fail("ExternalServiceApiActionException was not thrown");
    }

    @Test
    public void whenDeletingUnavailableUri_shouldRetryThreeTimes() throws Exception {
        WebClient mock = mock(WebClient.class);
        try {
            doThrow(connectionExceptionOnAction()).when(mock).delete(any());
            WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
            webClientWithRetrying.delete(any());
        } catch (ExternalServiceApiActionException e) {
            verify(mock, times(3)).delete(any());
            return;
        }
        fail("ExternalServiceApiActionException was not thrown");
    }

    @Test(expectedExceptions = { ExternalServiceApiReaderException.class })
    public void whenGettingValidUri_shouldThrowExternalServiceApiReaderException() throws Exception {
        WebClient mock = mock(WebClient.class);
        when(mock.get(any())).thenThrow(nonConnectionExceptionOnReading());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.get(any());
    }

    @Test(expectedExceptions = { ExternalServiceApiActionException.class })
    public void whenPostingValidUri_shouldThrowExternalServiceApiActionException() throws Exception {
        WebClient mock = mock(WebClient.class);
        when(mock.post(any(), any())).thenThrow(nonConnectionExceptionOnAction());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.post(any(), any());
    }

    @Test(expectedExceptions = { ExternalServiceApiActionException.class })
    public void whenPatchingValidUri_shouldThrowExternalServiceApiActionException() throws Exception {
        WebClient mock = mock(WebClient.class);
        doThrow(nonConnectionExceptionOnAction()).when(mock).patch(any(), any());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.patch(any(), any());
    }

    @Test(expectedExceptions = { ExternalServiceApiActionException.class })
    public void whenDeletingValidUri_shouldThrowExternalServiceApiActionException() throws Exception {
        WebClient mock = mock(WebClient.class);
        doThrow(nonConnectionExceptionOnAction()).when(mock).patch(any(), any());

        WebClientWithRetrying webClientWithRetrying = new WebClientWithRetrying(mock);
        webClientWithRetrying.patch(any(), any());
    }

    private ExternalServiceApiActionException nonConnectionExceptionOnAction() {
        return new ExternalServiceApiActionException("", URI.create(""), new IllegalStateException());
    }

    private ExternalServiceApiReaderException nonConnectionExceptionOnReading() {
        return new ExternalServiceApiReaderException("", URI.create(""), new IllegalStateException());
    }

    private ExternalServiceApiActionException connectionExceptionOnAction() {
        return new ExternalServiceApiActionException("", URI.create(""), new ConnectException(""));
    }

    private ExternalServiceApiReaderException connectionExceptionOnReading() {
        return new ExternalServiceApiReaderException("", URI.create(""), new ConnectException(""));
    }
}
