/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.utils;

import java.math.BigDecimal;

import static java.lang.Math.pow;
import static java.math.BigDecimal.valueOf;
import static java.math.RoundingMode.DOWN;

public final class Converters {
    private static final double BYTES_IN_GIBIBYTE = pow(2, 30);
    private static final int SCALE = 2;

    private Converters() {
    }

    public static BigDecimal convertBytesToGib(Long bytes) {
        if (bytes == null) {
            return null;
        }

        return convertBytesToGib(BigDecimal.valueOf(bytes));
    }

    public static BigDecimal convertBytesToGib(BigDecimal bytes) {
        if (bytes == null) {
            return null;
        }

        return bytes.divide(valueOf(BYTES_IN_GIBIBYTE), SCALE, DOWN);
    }

    public static BigDecimal convertGibToBytes(BigDecimal gibs) {
        if (gibs == null) {
            return null;
        }

        return gibs.multiply(valueOf(BYTES_IN_GIBIBYTE));
    }
}
