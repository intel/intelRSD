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

package com.intel.podm.security.providers;

import com.intel.podm.config.base.dto.SecurityConfig.CertificateType;

import javax.naming.InitialContext;
import javax.naming.NamingException;

import static java.lang.String.format;

public enum PasswordProvider {
    INSTANCE;

    public String getPassword(CertificateType type) throws NamingException {
        InitialContext ctx = null;
        try {
            ctx = new InitialContext();

            return getPassword(type, ctx);
        } finally {
            if (ctx != null) {
                ctx.close();
            }
        }
    }

    public String getPassword(CertificateType type, InitialContext ctx) throws NamingException {
        switch (type) {
            case CLIENT:
                return (String) ctx.lookup("java:global/clientPassword");
            case SERVER:
                return (String) ctx.lookup("java:global/serverPassword");
            default:
                throw new UnsupportedOperationException(format("Password for certificate of type %s does not exist.", type));
        }
    }
}

