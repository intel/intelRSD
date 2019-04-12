/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.collections;

import lombok.experimental.UtilityClass;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@UtilityClass
public class Lists {

    public static <T> List<T> concat(List<T> left, List<T> right) {
        ArrayList<T> result = new ArrayList<>(left);
        result.addAll(right);
        return result;
    }

    public static <T> List<T> concat(List<T> left, T... right) {
        ArrayList<T> result = new ArrayList<>(left);
        result.addAll(Arrays.asList(right));
        return result;
    }
}
