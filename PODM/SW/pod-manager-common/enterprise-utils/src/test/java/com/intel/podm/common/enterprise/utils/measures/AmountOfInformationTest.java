/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.common.enterprise.utils.measures;

import com.intel.podm.common.enterprise.utils.measures.exceptions.FractionalBytesException;
import org.testng.annotations.Test;

import java.math.BigDecimal;

import static org.testng.Assert.assertEquals;

import static com.intel.podm.common.enterprise.utils.measures.prefixes.Prefix.MEBI;
import static com.intel.podm.common.enterprise.utils.measures.prefixes.Prefix.NONE;
import static com.intel.podm.common.enterprise.utils.measures.AmountOfInformation.createAmountOfInformation;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MagicNumber"})
public class AmountOfInformationTest {
    @Test
    public void whenComparingTheSameAmountsOfInformationButCreatedWithDifferentPrefix_ShouldTreatThemAsEqual() {
        AmountOfInformation left = createAmountOfInformation(new BigDecimal("42"), NONE);
        AmountOfInformation right = createAmountOfInformation(new BigDecimal("0.0000400543212890625"), MEBI);

        assertEquals(left, right);
    }

    @Test(expectedExceptions = FractionalBytesException.class)
    public void whenCreatingAmountOfInformationWithFractionalValueOfBytes_ShouldThrow() {
        AmountOfInformation fractionalBytes = createAmountOfInformation(new BigDecimal("0.5"), NONE);
    }
}
