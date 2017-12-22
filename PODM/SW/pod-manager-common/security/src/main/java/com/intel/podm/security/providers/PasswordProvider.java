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

import javax.naming.InitialContext;
import javax.naming.NamingException;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;

public enum PasswordProvider {
    INSTANCE;

    private static Logger logger = getLogger(PasswordProvider.class);

    public String getPassword() {
        try {
            InitialContext ctx = new InitialContext();
            String password = (String) ctx.lookup("java:global/password");
            ctx.close();
            return password;
        } catch (NamingException e) {
            logger.e("Context not found {}", e.getMessage());
            throw new RuntimeException("Context not found. Keystore password could not be retrieved.", e);
        }
    }
}

