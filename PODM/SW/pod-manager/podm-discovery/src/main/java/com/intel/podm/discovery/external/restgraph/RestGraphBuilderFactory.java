/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.discovery.external.restgraph;

import com.intel.podm.common.synchronization.CancelableChecker;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class RestGraphBuilderFactory {
    @Inject
    private RestGraphBuilderImpl restGraphBuilder;

    public RestGraphBuilder createWithCancelableChecker(CancelableChecker cancelableChecker) {
        restGraphBuilder.setCancelableChecker(cancelableChecker);
        return restGraphBuilder;
    }
}
