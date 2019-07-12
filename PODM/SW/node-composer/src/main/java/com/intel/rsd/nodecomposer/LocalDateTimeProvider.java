/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer;

import java.time.Clock;
import java.time.LocalDateTime;
import java.time.ZoneId;

import static java.time.Clock.fixed;
import static java.time.Clock.systemDefaultZone;
import static java.time.ZoneId.systemDefault;

public class LocalDateTimeProvider {

    private static final ZoneId DEFAULT_ZONE_ID = systemDefault();

    private static Clock clock = systemDefaultZone();

    public static LocalDateTime now() {
        return LocalDateTime.now(clock);
    }

    public static void setFixedClockAt(LocalDateTime localDateTime) {
        clock = fixed(localDateTime.atZone(DEFAULT_ZONE_ID).toInstant(), DEFAULT_ZONE_ID);
    }

    public static void setSystemDefaultZoneClock() {
        clock = systemDefaultZone();
    }
}
