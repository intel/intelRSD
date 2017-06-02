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

package com.intel.podm.client.redfish.response;

import com.intel.podm.client.api.redfish.response.RedfishConnectionException;
import com.intel.podm.client.api.redfish.response.RedfishException;
import com.intel.podm.client.api.redfish.response.RedfishResponse;
import com.intel.podm.client.redfish.http.HttpResponse;
import org.apache.commons.lang.NotImplementedException;

import javax.ws.rs.ProcessingException;
import java.net.SocketException;
import java.net.SocketTimeoutException;

import static com.intel.podm.common.utils.Contracts.requires;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static org.apache.commons.lang.exception.ExceptionUtils.getRootCause;

public final class RedfishResponseFactory {
    private RedfishResponseFactory() {
    }

    public static RedfishResponse invoke(RedfishCallable callable) throws RedfishException {
        requiresNonNull(callable, "callable");

        HttpResponse httpResponse;
        try {
            httpResponse = callable.call();
        } catch (ProcessingException e) {
            throw isCausedBySocketException(e)
                    ? new RedfishConnectionException(e)
                    : new RedfishException(e);
        }

        return create(httpResponse);
    }

    public static RedfishResponse create(HttpResponse httpResponse) throws RedfishException {
        requiresNonNull(httpResponse, "httpResponse");

        if (httpResponse.getStatusCode().isSuccessful()) {
            return new RedfishResponseImpl(httpResponse);
        } else if (httpResponse.getStatusCode().isError()) {
            return createException(httpResponse);
        }

        throw new NotImplementedException();
    }

    private static RedfishResponse createException(HttpResponse httpResponse) throws RedfishException {
        requiresNonNull(httpResponse, "response");
        requires(httpResponse.getStatusCode().isError(), "status code " + httpResponse.getStatusCode() + " does not indicate error");

        String msg = format("Service replied with %s", httpResponse);
        throw new RedfishException(msg);
    }

    private static boolean isCausedBySocketException(ProcessingException e) {
        return getRootCause(e) instanceof SocketException || getRootCause(e) instanceof SocketTimeoutException;
    }

    public interface RedfishCallable {
        HttpResponse call();
    }
}
