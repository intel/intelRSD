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

package com.intel.tools.coverage

import org.gradle.api.Plugin
import org.gradle.api.Project

class CoverageLimits {
    Float lineCoverageMinThreshold

    /* how much can overall code coverage exceed the minThreshold until we consider threshold too low */
    Float lineCoverageExcessMax = 0.025

    protected CoverageLimits() { }

    @Override
    String toString() {
        return """{minThreshold: ${lineCoverageMinThreshold}, excessMax: ${lineCoverageExcessMax}}"""
    }
}

class CoverageLevelVerification extends CoverageLimits {
    Map<String, CoverageLimits> packages

    static CoverageLimits limits(Closure configureClosure) {
        def res = new CoverageLimits()
        res.with configureClosure
        return res
    }
}

class CoverageLevelPlugin implements Plugin<Project> {
    @Override
    void apply(Project target) {
        target.extensions.create('coverageLevel', CoverageLevelVerification)
    }
}
