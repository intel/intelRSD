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

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static org.testng.Assert.assertEquals;

public class IscsiSanUriTest {
    @Test(dataProvider = "validUris")
    public void testParsingValidUri(String uri, String targetName, String server, int port, int lun) throws Exception {
        IscsiSanUri sut = new IscsiSanUri(uri);

        assertEquals(sut.getTargetName(), targetName);
        assertEquals(sut.getServerName(), server);
        assertEquals(sut.getPort(), port);
        assertEquals(sut.getLun(), lun);
    }

    @DataProvider
    public Object[][] validUris() {
        return new Object[][] {
                {
                        "iscsi:servername::111:3:targetname", //iSCSI SAN URI
                        "targetname",
                        "servername",
                        111, // port
                        3 // LUN
                },
                {
                        "iscsi:serverName::666:2:targetName",
                        "targetName",
                        "serverName",
                        666,
                        2
                },
                {
                        "iscsi:10.0.4.1::3260:1:iqn.2010-04.org.ipxe.dolphin:storage",
                        "iqn.2010-04.org.ipxe.dolphin:storage",
                        "10.0.4.1",
                        3260,
                        1
                },
                {
                        "iscsi:::::iqn.targetname",
                        "iqn.targetname",
                        null,
                        3260,
                        0
                },
                {
                        "iscsi::::AA:iqn.targetname",
                        "iqn.targetname",
                        null,
                        3260,
                        0xAA
                }
        };
    }

    @Test(dataProvider = "invalidUris", expectedExceptions = IllegalArgumentException.class)
    public void testParsingInvalidUris(String invalidUri) {
        new IscsiSanUri(invalidUri);
    }

    @DataProvider
    public Object[][] invalidUris() {
        return new Object[][] {
                {null},
                {"iscsi::::iqn.targetname"},
                {"iscsi:10.0.4.1::INCORRECT_PORT:1:iqn.2010-04.org.ipxe.dolphin:storage"},
                {"iscsi:10.0.4.1::111:INCORRECT_LUN:iqn.2010-04.org.ipxe.dolphin:storage"},
                {"iscsi::::"},
        };
    }

    @Test(dataProvider = "urisAndToStringValue")
    public void testToString(String uri, String toStringValue) throws Exception {
        IscsiSanUri sut = new IscsiSanUri(uri);

        assertEquals(sut.toString(), toStringValue);
    }

    @DataProvider
    public Object[][] urisAndToStringValue() {
        return new Object[][] {
                {
                        "iscsi:10.0.4.1::3260:1:iqn.2010-04.org.ipxe.dolphin:storage",
                        "iscsi:10.0.4.1::3260:1:iqn.2010-04.org.ipxe.dolphin:storage",
                },
                {
                        "iscsi:serverName::1:ff:targetName",
                        "iscsi:serverName::1:ff:targetName",
                },
                {
                        "iscsi:::::targetName",
                        "iscsi:::3260:0:targetName",
                },
        };
    }
}
