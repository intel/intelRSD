/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.services.detection.dhcp;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConnectionConfig;
import com.intel.podm.services.detection.dhcp.filesystem.TmpLeasesRecord;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.List;

import static java.util.stream.Collectors.toList;
import static javax.ws.rs.core.UriBuilder.fromUri;

@Dependent
public class UrlProvider {

    private static final String DEFAULT_ROOT = "/redfish/v1";
    private static final String HTTP_SCHEME = "http";
    private static final String HTTPS_SCHEME = "https";

    @Inject
    private Logger logger;

    @Inject
    @Config
    private Holder<ServiceConnectionConfig> connectionConfig;

    public URI getEndpointUri(String urlString) {
        return fromUri(urlString).build();
    }

    public List<URI> getEndpointUris(TmpLeasesRecord record) {
        ServiceType serviceType = record.getServiceType();

        ServiceConnectionConfig config = connectionConfig.get();
        boolean sslEnabled = config.getConnectionSecurity().isSslEnabledForServicesOfType(record.getServiceType());
        List<Integer> ports = sslEnabled
            ? config.getConnectionSecurity().getSslPortsForServicesOfType(serviceType)
            : config.getConnectionSecurity().getDefaultPortsForServicesOfType(serviceType);

        return ports.stream()
            .map(port -> fromUri("{scheme}://{ip}:{port}")
                .resolveTemplate("scheme", sslEnabled ? HTTPS_SCHEME : HTTP_SCHEME)
                .resolveTemplate("ip", record.getIpAddress())
                .resolveTemplate("port", port)
                .path(DEFAULT_ROOT)
                .build())
            .collect(toList());
    }
}
