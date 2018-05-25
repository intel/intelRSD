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

package com.intel.podm.services.detection.ssdp;

import org.fourthline.cling.ManagedUpnpServiceConfiguration;
import org.fourthline.cling.transport.spi.NetworkAddressFactory;
import org.fourthline.cling.transport.spi.StreamClient;
import org.fourthline.cling.transport.spi.StreamServer;

import javax.annotation.Resource;
import javax.enterprise.concurrent.ManagedExecutorService;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Specializes;
import java.util.concurrent.ExecutorService;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.SSDP_EXECUTOR;

@Specializes
@ApplicationScoped
public class SsdpServiceConfiguration extends ManagedUpnpServiceConfiguration {
    @Resource(lookup = "java:/" + SSDP_EXECUTOR)
    private ManagedExecutorService executorService;

    @Override
    public StreamClient createStreamClient() {
        return null;
    }

    @Override
    public StreamServer createStreamServer(NetworkAddressFactory networkAddressFactory) {
        return null;
    }

    @Override
    protected ExecutorService createDefaultExecutorService() {
        return executorService;
    }
}
