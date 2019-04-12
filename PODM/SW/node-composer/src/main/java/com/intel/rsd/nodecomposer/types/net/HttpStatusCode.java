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

package com.intel.rsd.nodecomposer.types.net;

@SuppressWarnings("MagicNumber")
public final class HttpStatusCode {
    public static final HttpStatusCode OK = new HttpStatusCode(200);
    public static final HttpStatusCode ACCEPTED = new HttpStatusCode(202);
    public static final HttpStatusCode NOT_FOUND = new HttpStatusCode(404);
    public static final HttpStatusCode METHOD_NOT_ALLOWED = new HttpStatusCode(405);
    public static final HttpStatusCode NOT_IMPLEMENTED = new HttpStatusCode(501);
    public static final HttpStatusCode SERVICE_UNAVAILABLE = new HttpStatusCode(503);
    public static final HttpStatusCode BAD_REQUEST = new HttpStatusCode(400);

    private final int value;

    private HttpStatusCode(int value) {
        this.value = value;
    }

    public static HttpStatusCode httpStatusCode(int value) {
        return new HttpStatusCode(value);
    }

    public boolean isSuccessful() {
        return inRange(200, 299);
    }

    public boolean isRedirection() {
        return inRange(300, 399);
    }

    public boolean isClientError() {
        return inRange(400, 499);
    }

    public boolean isServerError() {
        return inRange(500, 599);
    }

    public boolean isError() {
        return isClientError() || isServerError();
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof HttpStatusCode) {
            HttpStatusCode that = (HttpStatusCode) o;
            return value == that.value;
        }

        return false;
    }

    @Override
    public int hashCode() {
        return value;
    }

    public int toInt() {
        return value;
    }

    @Override
    public String toString() {
        return "" + value;
    }

    private boolean inRange(int min, int max) {
        return value >= min && value <= max;
    }
}
