/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.common.utils;

import org.testng.annotations.Test;

import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNull;

@SuppressWarnings({"checkstyle:MethodName"})
public class IterableHelperTest {

    /**
     * Tests for IterableHelper.single
     */
    @Test
    public void whenPassingOneElementIterableToSingleMethod_ShouldReturnContainedElement() {
        String originalTestString = "testString";
        final Iterable<String> stringIterable = asList(originalTestString);

        String testValue = single(stringIterable);
        assertEquals(testValue, originalTestString);
    }

    @Test(expectedExceptions = IllegalStateException.class)
    public void whenPassingEmptyIterableToSingleMethod_ShouldThrow() {
        single(emptyList());
    }

    @Test(expectedExceptions = IllegalStateException.class)
    public void whenPassingIterableWithMoreThanOneElementToSingleMethod_ShouldThrow() {
        final Iterable<String> stringIterable = asList("one", "two");
        single(stringIterable);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenPassingNullToSingleMethod_ShouldThrow() {
        single(null);
    }

    /**
     * Tests for IterableHelper.singleOrNull
     */
    @Test
    public void whenPassingOneElementIterableToSingleOrNullMethod_ShouldReturnContainedElement() {
        String originalTestString = "testString";
        final Iterable<String> stringIterable = asList(originalTestString);

        String testValue = singleOrNull(stringIterable);
        assertEquals(testValue, originalTestString);
    }

    @Test
    public void whenPassingEmptyIterableToSingleOrNullMethod_ShouldReturnNull() {
        assertNull(singleOrNull(emptyList()));
    }

    @Test(expectedExceptions = IllegalStateException.class)
    public void whenPassingIterableWithMoreThanOneElementToSingleOrNullMethod_ShouldThrow() {
        final Iterable<String> stringIterable = asList("one", "two");
        singleOrNull(stringIterable);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenPassingNullToSingleOrNullMethod_ShouldThrow() {
        singleOrNull(null);
    }
}
