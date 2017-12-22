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

package com.intel.podm.common.types;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.common.types.Health.CRITICAL;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.Health.WARNING;
import static com.intel.podm.common.types.State.ABSENT;
import static com.intel.podm.common.types.State.DISABLED;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.types.State.UNAVAILABLE_OFFLINE;
import static com.intel.podm.common.types.Status.statusFromMap;
import static com.intel.podm.common.types.Status.statusFromString;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertNull;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MethodCount"})
public class StatusTest {

    @DataProvider(name = "statusStrings")
    private static Object[][] statusStrings() {
        return new Object[][]{
            {new Status(ENABLED, OK, OK), "State=Enabled,Health=OK,HealthRollup=OK"},
            {new Status(DISABLED, null, CRITICAL), "State=Disabled,HealthRollup=CRITICAL"},
            {new Status(ABSENT, WARNING, null), "State=Absent,Health=WARNING"},
            {new Status(UNAVAILABLE_OFFLINE, null, null), "State=UnavailableOffline"},
            {new Status(null, null, null), null},
            {new Status(null, WARNING, OK), "Health=WARNING,HealthRollup=OK"}
        };
    }

    @Test
    public void fromNullString_ShouldReturnNull() {
        Status expected = new Status(null, null, null);
        Status status = statusFromString(null);
        assertEquals(status, expected);
    }

    @Test
    public void fromEmptyString_ShouldThrow() {
        Status expected = new Status(null, null, null);
        Status status = statusFromString("");
        assertEquals(status, expected);
    }

    @Test
    public void fromCorrectString_ShouldReturnProperInstance() {
        String statusString = "State=Enabled,Health=OK,HealthRollup=OK";

        Status expected = new Status(ENABLED, OK, OK);
        Status status = statusFromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromCorrectStringWithoutHealthRollup_ShouldReturnProperInstance() {
        String statusString = "State=Enabled,Health=OK";

        Status expected = new Status(ENABLED, OK, null);

        Status status = statusFromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromCorrectStringWithoutHealth_ShouldReturnProperInstance() {
        String statusString = "State=Enabled,HealthRollup=OK";

        Status expected = new Status(ENABLED, null, OK);

        Status status = statusFromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromStringWithoutState_ShouldReturnProperInstance() {
        String statusString = "Health=OK,HealthRollup=OK";
        Status expected = new Status(null, OK, OK);
        Status status = statusFromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromStringWithWrongState_ShouldReturnNullState() {
        String statusString = "State=Online";

        Status status = statusFromString(statusString);
        assertNull(status.getState());
    }

    @Test
    public void fromStringWithWrongHealthValue_ShouldReturnHealthNull() {
        String statusString = "State=Enabled,Health=OKAY,HealthRollup=OK";

        Status status = statusFromString(statusString);
        assertNotNull(status.getState());
        assertNotNull(status.getHealthRollup());
        assertNull(status.getHealth());
    }

    @Test
    public void fromStringWithWrongHealthRollupValue_ShouldReturnHealthRollupNull() {
        String statusString = "State=Enabled,Health=OK,HealthRollup=OKAY";

        Status status = statusFromString(statusString);
        assertNotNull(status.getState());
        assertNotNull(status.getHealth());
        assertNull(status.getHealthRollup());
    }

    @Test(dataProvider = "statusStrings")
    public void toString_ShouldReturnProperString(Status actual, String expected) {
        assertEquals(actual.toString(), expected);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*Error while parsing.*")
    public void fromStringWithSyntaxError_ShouldThrowWithProperMessage() {
        String statusString = "State=Enabled,Health;OK,HealthRollup=OK";
        statusFromString(statusString);
    }

    @Test
    public void fromMapWithMissingState_ShouldReturnProperInstance() {
        Map<String, String> map = new HashMap<String, String>() {
            private static final long serialVersionUID = -2611007892635015315L;

            {
                put("Health", "OK");
                put("HealthRollup", "OK");
            }
        };
        Status expected = new Status(null, OK, OK);
        Status status = statusFromMap(map);
        assertEquals(status, expected);
    }

    @Test
    public void fromEmptyMap_ShouldReturnNull() {
        Map<String, String> map = new HashMap<>();
        Status expected = new Status(null, null, null);
        Status status = statusFromMap(map);
        assertEquals(status, expected);
    }

    @Test
    public void fromStateLegacyOffline_ShouldReturnStateUnavailableOffline() {
        String statusString = "State=Offline";
        Status expected = new Status(UNAVAILABLE_OFFLINE, null, null);

        Status status = statusFromString(statusString);
        assertEquals(status, expected);
    }
}
