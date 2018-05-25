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

package com.intel.podm.common.types;

public class ConnectionParameters {
    private long serviceConnectionTimeout;
    private long serviceSocketTimeout;
    private int connectionPoolSize;
    private int maxPooledPerRoute;

    public ConnectionParameters(long serviceConnectionTimeout, long serviceSocketTimeout, int connectionPoolSize, int maxPooledPerRoute) {
        this.serviceConnectionTimeout = serviceConnectionTimeout;
        this.serviceSocketTimeout = serviceSocketTimeout;
        this.connectionPoolSize = connectionPoolSize;
        this.maxPooledPerRoute = maxPooledPerRoute;
    }

    public long getServiceConnectionTimeout() {
        return serviceConnectionTimeout;
    }

    public long getServiceSocketTimeout() {
        return serviceSocketTimeout;
    }

    public int getConnectionPoolSize() {
        return connectionPoolSize;
    }

    public int getMaxPooledPerRoute() {
        return maxPooledPerRoute;
    }
}
