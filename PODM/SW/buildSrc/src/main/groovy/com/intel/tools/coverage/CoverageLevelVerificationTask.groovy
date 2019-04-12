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

import org.gradle.api.DefaultTask
import org.gradle.api.GradleException
import org.gradle.api.tasks.InputFile
import org.gradle.api.tasks.Optional
import org.gradle.api.tasks.TaskAction
import org.gradle.api.tasks.VerificationTask

class CoverageLevelVerificationTask extends DefaultTask {

    @InputFile
    @Optional
    File xmlReport

    @TaskAction
    void verifyCoverage() {
        def parser = new XmlParser()
        parser.with {
            setFeature("http://apache.org/xml/features/disallow-doctype-decl", false)
            setFeature("http://apache.org/xml/features/nonvalidating/load-external-dtd", false);
        }
        def doc = parser.parse(xmlReport)
        def res = doc.counter.collectEntries { [it.@type.toLowerCase(), [covered: it.@covered as int, total: (it.@missed as int) + (it.@covered as int)]] }
        def computedLineCoverage = (res.line.covered / res.line.total).toFloat()
        def coverageLevel = project.extensions.getByType(CoverageLevelVerification)
        if (computedLineCoverage < coverageLevel.lineCoverageMinThreshold) {
            throw new GradleException("Line coverage level below threshold ($computedLineCoverage < ${coverageLevel.lineCoverageMinThreshold})")
        }
    }
}
