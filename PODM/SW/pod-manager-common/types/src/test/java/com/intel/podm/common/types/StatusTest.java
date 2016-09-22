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

package com.intel.podm.common.types;

import com.google.common.collect.ImmutableMap;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.common.types.Health.Critical;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.Health.Warning;
import static com.intel.podm.common.types.State.ABSENT;
import static com.intel.podm.common.types.State.DISABLED;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.types.State.OFFLINE;
import static com.intel.podm.common.types.Status.fromString;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNull;

public class StatusTest {

    @DataProvider(name = "statusStrings")
    private static Object[][] statusStrings() {
        return new Object[][] {
                {new Status(ENABLED, OK, OK), "State=Enabled,Health=OK,HealthRollup=OK" },
                {new Status(DISABLED, null, Critical), "State=Disabled,HealthRollup=Critical"},
                {new Status(ABSENT, Warning, null), "State=Absent,Health=Warning"},
                {new Status(OFFLINE, null, null), "State=UnavailableOffline"},
                {new Status(null, null, null), null},
                {new Status(null, Warning, OK), "Health=Warning,HealthRollup=OK"}
        };
    }


    @Test
    public void fromNullString_ShouldReturnNull() {
        assertNull(fromString(null));
    }

    @Test
    public void fromEmptyString_ShouldThrow() {
        assertNull(fromString(""));
    }

    @Test
    public void fromCorrectString_ShouldReturnProperInstance() {
        String statusString = "State=Enabled,Health=OK,HealthRollup=OK";

        Status expected = new Status(ENABLED, OK, OK);
        Status status = fromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromCorrectStringWithoutHealthRollup_ShouldReturnProperInstance() {
        String statusString = "State=Enabled,Health=OK";

        Status expected = new Status(ENABLED, OK, null);

        Status status = fromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromCorrectStringWithoutHealth_ShouldReturnProperInstance() {
        String statusString = "State=Enabled,HealthRollup=OK";

        Status expected = new Status(ENABLED, null, OK);

        Status status = fromString(statusString);

        assertEquals(status, expected);
    }

    @Test
    public void fromStringWithoutState_ShouldReturnProperInstance() {
        String statusString = "Health=OK,HealthRollup=OK";
        Status expected = new Status(null, OK, OK);
        Status status = fromString(statusString);

        assertEquals(status, expected);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*State value is incorrect.*")
    public void fromStringWithWrongState_ShouldThrowWithProperMessage() {
        String statusString = "State=Online";

        fromString(statusString);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*Health value is incorrect.*")
    public void fromStringWithWrongHealthValue_ShouldThrowWithProperMessage() {
        String statusString = "State=Enabled,Health=OKAY,HealthRollup=OK";

        fromString(statusString);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*Health value is incorrect.*")
    public void fromStringWithWrongHealthRollupValue_ShouldThrowWithProperMessage() {
        String statusString = "State=Enabled,Health=OK,HealthRollup=OKAY";

        fromString(statusString);
    }

    @Test(dataProvider = "statusStrings")
    public void toString_ShouldReturnProperString(Status actual, String expected) {
        assertEquals(actual.toString(), expected);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*Error while parsing.*")
    public void fromStringWithSyntaxError_ShouldThrowWithProperMessage() {
        String statusString = "State=Enabled,Health;OK,HealthRollup=OK";
        fromString(statusString);
    }

    @Test
    public void fromMapWithMissingState_ShouldReturnProperInstance() {
        Map<String, String> map = new ImmutableMap.Builder().
                put("Health", "OK").
                put("HealthRollup", "OK").
                build();
        Status expected = new Status(null, OK, OK);
        Status status = Status.fromMap(map);
        assertEquals(status, expected);
    }

    @Test
    public void fromEmptyMap_ShouldReturnNull() {
        Map<String, String> map = new HashMap<>();
        Status status = Status.fromMap(map);

        assertNull(status);
    }
}
