/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.security.providers;

import com.intel.podm.common.logger.Logger;

import javax.annotation.PostConstruct;
import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.net.ssl.KeyManager;
import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;

import static javax.net.ssl.SSLContext.getInstance;

@ApplicationScoped
public class SslContextProvider {
    private static final String PROTOCOL = "TLSv1.2";

    @Inject
    private Logger logger;

    @Inject
    private SslConnectionManagersProvider keyManagersProvider;

    private SSLContext context;

    public SSLContext getContext() {
        return context;
    }

    @PostConstruct
    private void init() {
        try {
            if (context == null) {
                KeyManager[] keyManagers = keyManagersProvider.getKeyManagersArray();
                TrustManager[] trustManagers = keyManagersProvider.getTrustManagersWhichTrustAllIssuers();
                context = getInstance(PROTOCOL);
                context.init(keyManagers, trustManagers, null);
            }
        } catch (NoSuchAlgorithmException | KeyManagementException e) {
            logger.e("Keystore has not been initialized {}", e.getMessage());
            throw new RuntimeException("Keystore has not been initialized", e);
        }
    }
}
