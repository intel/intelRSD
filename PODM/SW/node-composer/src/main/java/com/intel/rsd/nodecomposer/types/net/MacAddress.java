/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import java.io.Serializable;
import java.util.Arrays;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static java.lang.Integer.parseInt;
import static java.lang.String.format;
import static java.util.regex.Pattern.compile;
import static org.apache.commons.lang3.StringUtils.trimToNull;

/**
 * Provides lower case string representation of provided MAC Address.
 */
public class MacAddress implements Serializable {
    private static final long serialVersionUID = 5747417246966085116L;
    private static final int MAC_LENGTH = 6;
    private static final int HEX_RADIX = 16;
    private static final Pattern MAC_PATTERN = compile("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$");

    private final byte[] macAddressBytes;

    public MacAddress(byte[] macAddressBytes) {
        requires(macAddressBytes != null, "MAC Address must not be null");
        requires(macAddressBytes.length == MAC_LENGTH, "MAC Address length is " + macAddressBytes.length + " bytes instead of " + MAC_LENGTH + " bytes");

        this.macAddressBytes = macAddressBytes.clone();
    }

    public MacAddress(String macAddressString) {
        String localMacAddressString = trimToNull(macAddressString);
        requires(localMacAddressString != null, "MAC Address must not be empty or null");

        Matcher matcher = MAC_PATTERN.matcher(localMacAddressString);

        requires(matcher.matches(), "MAC Address must contain 12 hex digits separated pairwise by ':' (provided MAC Address: [" + macAddressString + "])");

        String[] hexPairs = localMacAddressString.split("[:-]");

        byte[] macAddressInBytes = new byte[MAC_LENGTH];
        for (int i = 0; i < MAC_LENGTH; i++) {
            macAddressInBytes[i] = (byte) parseInt(hexPairs[i], HEX_RADIX);
        }

        this.macAddressBytes = macAddressInBytes;
    }

    public static MacAddress macAddressOrNull(String macAddressString) {
        MacAddress macAddress;
        try {
            macAddress = new MacAddress(macAddressString);
        } catch (IllegalArgumentException iea) {
            macAddress = null;
        }

        return macAddress;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder(format("%02X", macAddressBytes[0]));
        for (int i = 1; i < macAddressBytes.length; ++i) {
            sb.append(format("%s%02X", ":", macAddressBytes[i]));
        }
        return sb.toString().toLowerCase();
    }

    @Override
    public boolean equals(Object otherMacAddress) {
        if (this == otherMacAddress) {
            return true;
        }
        if (otherMacAddress == null || getClass() != otherMacAddress.getClass()) {
            return false;
        }

        return Arrays.equals(macAddressBytes, ((MacAddress) otherMacAddress).macAddressBytes);
    }

    @Override
    public int hashCode() {
        return Arrays.hashCode(macAddressBytes);
    }
}
