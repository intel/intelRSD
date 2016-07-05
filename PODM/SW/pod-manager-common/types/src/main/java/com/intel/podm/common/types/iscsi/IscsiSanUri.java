/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.common.types.iscsi;


import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.google.common.base.Strings.isNullOrEmpty;
import static java.lang.String.format;
import static java.util.Objects.hash;

/**
 * Represents iSCSI SAN URI
 * <a href=http://tools.ietf.org/html/rfc4173>rfc4173</a>
 */
public final class IscsiSanUri {
    private static final Pattern PATTERN = Pattern.compile(
            "iscsi:(?<serverName>[^:]*)::(?<port>[0-9]*):(?<lun>[0-9a-fA-F]*):(?<targetName>.+)"
    );

    private static final int DEFAULT_PORT = 3260;
    private static final int DEFAULT_LUN = 0;

    /**
     * <a href="https://tools.ietf.org/html/rfc3720#section-3.2.6.3">rfc3720</a>
     * TODO correct validation must be provided
     */
    private final String targetName;
    private final String serverName;
    private final int port;
    private final int lun;

    public IscsiSanUri(String targetName, String serverName, Integer port, Integer lun) {
        if (isNullOrEmpty(targetName)) {
            throw new IllegalArgumentException("targetName must not be null or empty");
        }

        this.targetName = targetName;
        this.serverName = serverName;
        this.port = port == null
                ? DEFAULT_PORT
                : port;
        this.lun = lun == null
                ? DEFAULT_LUN
                : lun;
    }

    public IscsiSanUri(String iscsiSanUri) {
        if (iscsiSanUri == null) {
            throw new IllegalArgumentException("iscsiSanUri must not be null");
        }

        Matcher matcher = PATTERN.matcher(iscsiSanUri);
        if (!matcher.find()) {
            String msg = format("iSCSI SAN URI '%s' has incorrect format", iscsiSanUri);
            throw new IllegalArgumentException(msg);
        }

        serverName = nullIfEmpty(matcher.group("serverName"));
        targetName = matcher.group("targetName");

        final int portRadix = 10;
        final int lunRadix = 16;
        port = parseIntOrDefaultIfEmpty(matcher.group("port"), portRadix, DEFAULT_PORT);
        lun = parseIntOrDefaultIfEmpty(matcher.group("lun"), lunRadix, DEFAULT_LUN);
    }

    private String nullIfEmpty(String string) {
        return string.isEmpty()
                ? null
                : string;
    }

    private static Integer parseIntOrDefaultIfEmpty(String integer, int radix, int defaultValue) {
        if (integer.isEmpty()) {
            return defaultValue;
        }

        return Integer.valueOf(integer, radix);
    }

    public String getTargetName() {
        return targetName;
    }

    public String getServerName() {
        return serverName;
    }

    public int getPort() {
        return port;
    }

    public int getLun() {
        return lun;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof IscsiSanUri)) {
            return false;
        }

        IscsiSanUri that = (IscsiSanUri) o;
        return Objects.equals(port, that.port)
                && Objects.equals(lun, that.lun)
                && Objects.equals(targetName, that.targetName)
                && Objects.equals(serverName, that.serverName);
    }

    @Override
    public int hashCode() {
        return hash(targetName, serverName);
    }

    @Override
    public String toString() {
        return format(
                "iscsi:%s::%d:%h:%s",
                Objects.toString(serverName, ""),
                port,
                lun,
                targetName
        );
    }
}
