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

package com.intel.podm.common.types.net;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static org.testng.AssertJUnit.assertEquals;

public class MacAddressTest {

    private static final byte HEX_255 = (byte) 0xFF;

    @DataProvider(name = "1")
    public static Object[][] macAddressesBytes() {
        return new Object[][] {
               {new byte[] {0, 1, 2, 3, 4, 5 }, "00:01:02:03:04:05" },
               {new byte[] {5, 4, 3, 2, 1, 0 }, "05:04:03:02:01:00" },
               {new byte[] {HEX_255, HEX_255, HEX_255, HEX_255, HEX_255, HEX_255 }, "ff:ff:ff:ff:ff:ff" }
        };
    }

    @DataProvider(name = "2")
    public static String[][] macAddressesStrings() {
        return new String[][] {
                {"00:01:02:03:04:05 ", "00:01:02:03:04:05" },
                {" 05:04:03:02:01:00", "05:04:03:02:01:00" },
                {"Ff:FF:FF:FF:FF:FF", "ff:ff:ff:ff:ff:ff" },
        };
    }

    @Test(dataProvider = "1")
    public void givenValidMacAddressInByteFormat_ShouldReturnCorrespondingStringRepresentation(byte[] macAddress, String expected) {
        MacAddress sut = new MacAddress(macAddress);
        String actual = sut.toString();
        assertEquals(expected, actual);
    }

    @Test(dataProvider = "2")
    public void givenValidMacAddressInStringFormat_ShouldReturnCorrespondingStringRepresentation(String macAddress, String expected) {
        MacAddress sut = new MacAddress(macAddress);
        String actual = sut.toString();
        assertEquals(expected, actual);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void givenTooLongByteRepresentationOfMacAddress_ShouldThrow() {
        byte[] bytes = {0, 1, 2, 3, 4, 5, 6};
        new MacAddress(bytes);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void givenTooShortByteRepresentationOfMacAddress_ShouldThrow() {
        byte[] bytes = {0, 1, 2, 3, 4};
        new MacAddress(bytes);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*must not be null.*")
    public void givenNullByteRepresentationOfMacAddress_ShouldThrow() {
        new MacAddress((byte[]) null);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void givenTooLongStringRepresentationOfMacAddress_ShouldThrow() {
        String macAddress = "00:01:02:03:04:05:06";
        new MacAddress(macAddress);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void givenTooShortStringRepresentationOfMacAddress_ShouldThrow() {
        String macAddress = "00:01:02:03:04";
        new MacAddress(macAddress);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*must not be empty or null.*")
    public void givenNullStringRepresentationOfMacAddress_ShouldThrow() {
        new MacAddress((String) null);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*must not be empty or null.*")
    public void givenEmptyStringRepresentationOfMacAddress_ShouldThrow() {
        new MacAddress("");
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*hex digits.*")
    public void givenUnexpectedValuesStringRepresentationOfMacAddress_ShouldThrow() {
        String macAddress = "AA:BB:CC:DD:EE:G0";
        new MacAddress(macAddress);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*hex digits.*")
    public void givenTooLongValuesStringRepresentationOfMacAddress_ShouldThrow() {
        String macAddress = "111:222:33:44:55:66";
        new MacAddress(macAddress);
    }
}
