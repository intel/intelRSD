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

package com.intel.rsd.nodecomposer.utils;

import java.util.List;

import static java.lang.String.join;
import static java.util.Collections.sort;
import static org.testng.Assert.fail;

public final class FailManager {
    private FailManager() {
    }

    public static void failWithMessageIfAnyError(List<String> errors, String errorTitle) {
        if (errors != null && !errors.isEmpty()) {
            sort(errors);
            String error = "\n" + errors.size() + " problem(s) found - " + errorTitle + ":\n" + join("\n", errors);
            fail(error);
        }
    }
}
