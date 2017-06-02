/*
 * Copyright (c) 2016-2017 Intel Corporation
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

public enum LineInputVoltageType implements EnumeratedType {
    UNKNOWN("Unknown"),
    AC_LOW_LINE("ACLowLine"),
    AC_MID_LINE("ACMidLine"),
    AC_HIGH_LINE("ACHighLine"),
    DC_NEG_48V("DCNeg48V"),
    DC_380V("DC380V"),
    AC_120V("AC120V"),
    AC_240V("AC240V"),
    AC_277V("AC277V"),
    AC_AND_DC_WIDE_RANGE("ACandDCWideRange"),
    AC_WIDE_RANGE("ACWideRange"),
    DC_240V("DC240V");

    private final String value;

    LineInputVoltageType(String value) {
        this.value = value;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
