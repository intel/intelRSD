/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.utils.measures;

import com.intel.rsd.nodecomposer.utils.measures.exceptions.FractionalBytesException;
import com.intel.rsd.nodecomposer.utils.measures.prefixes.Prefix;

import java.math.BigDecimal;
import java.math.RoundingMode;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

public final class AmountOfInformation implements Comparable<AmountOfInformation> {
    private final long value;

    private AmountOfInformation(BigDecimal value, Prefix scale) {
        requiresNonNull(value, "value");
        requiresNonNull(scale, "scale");
        requires(value.compareTo(BigDecimal.ZERO) >= 0, "value");

        BigDecimal numberOfBytes = value.multiply(scale.getScaleFactor());

        if (hasFractionalPart(numberOfBytes)) {
            throw new FractionalBytesException("Amount of bytes can't be fractional.");
        }

        this.value = numberOfBytes.longValue();
    }

    /**
     * Creates object representing amount of information
     * @param value value used to create amount of information
     * @param scale scale used as prefix
     * @return object representing amount of information
     * @throws FractionalBytesException if amount of information (treated as amount of bytes) has fractional part
     */
    public static AmountOfInformation createAmountOfInformation(BigDecimal value, Prefix scale) {
        return new AmountOfInformation(value, scale);
    }

    public BigDecimal getQuantityOf(Prefix scale) {
        BigDecimal numberOfBytes = new BigDecimal(value);

        return numberOfBytes.divide(scale.getScaleFactor(), RoundingMode.UNNECESSARY);
    }

    @Override
    public int compareTo(AmountOfInformation that) {
        requiresNonNull(that, "that");

        if (value < that.value) {
            return -1;
        } else if (value == that.value) {
            return 0;
        } else {
            return 1;
        }
    }

    @Override
    public boolean equals(Object other) {
        if (this == other) {
            return true;
        }
        if (other == null || getClass() != other.getClass()) {
            return false;
        }

        AmountOfInformation that = (AmountOfInformation) other;
        return this.compareTo(that) == 0;
    }

    @Override
    public int hashCode() {
        return Long.hashCode(value);
    }

    private boolean hasFractionalPart(BigDecimal number) {
        BigDecimal fractionalPart = number.remainder(BigDecimal.ONE);
        return fractionalPart.compareTo(BigDecimal.ZERO) != 0;
    }
}
