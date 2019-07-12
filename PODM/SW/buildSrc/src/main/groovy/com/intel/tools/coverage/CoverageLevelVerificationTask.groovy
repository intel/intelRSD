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
        Map<String, Map<String, Integer>> packageCoverage = doc.package.collectEntries { [it.@name.replace('/', '.'), summarizePackage(it)['line']] }
        def interpolated = new TreeMap<String, Object>(interpolatePackages(packageCoverage))
        CoverageLevelVerification coverageLevel = project.extensions.getByType(CoverageLevelVerification)
        ([""] + coverageLevel.packages.keySet()).each { packageName ->
            verifyLineCoverage(packageName, interpolated)
        }
    }

    def verifyLineCoverage(String packageName, Map<String, Map<String, Integer>> coverage) {
        CoverageLevelVerification coverageLevel = project.extensions.getByType(CoverageLevelVerification)
        CoverageLimits packageCoverageLimits = packageName ? coverageLevel.packages[packageName] : coverageLevel
        Float computedLineCoverage = (coverage[packageName].covered / coverage[packageName].total).toFloat().trunc(3)
        packageCoverageLimits.with {
            def packageChoice = packageName ? "for '$packageName'" : "(global)"
            project.logger.quiet("""verifyCoverage: line coverage $packageChoice bounds = [{}, {}], computed coverage = {}""",
                    lineCoverageMinThreshold, lineCoverageMinThreshold + lineCoverageExcessMax, computedLineCoverage)
            if (computedLineCoverage < lineCoverageMinThreshold) {
                throw new GradleException("Line coverage $packageChoice level below threshold ($computedLineCoverage < $lineCoverageMinThreshold)")
            }
            if (computedLineCoverage > lineCoverageMinThreshold + lineCoverageExcessMax) {
                throw new GradleException("Line coverage $packageChoice minimal threshold set too low ($computedLineCoverage > $lineCoverageMinThreshold + $lineCoverageExcessMax); increase minThreshold")
            }
        }
    }

    private static Map<String, Map<String, Integer>> summarizePackage(def pkg) {
        return pkg.counter.collectEntries {
            [it.@type.toLowerCase(), [covered: it.@covered as int, total: (it.@missed as int) + (it.@covered as int)]]
        }
    }

    private static Map<String, Integer> addPackageCoverage(Map<String, Integer> pkg1, Map<String, Integer> pkg2) {
        return (pkg1.keySet() + pkg2.keySet()).toSet().collectEntries {
            [it, pkg1.get(it, 0) + pkg2.get(it, 0)]
        }
    }

    private static Map<String, String> mapPackagesToParents(Collection<String> packages) {
        Map<String, String> parents = new LinkedHashMap<>()
        LinkedList<String> packageList = new LinkedList<>(packages)
        while (!packageList.isEmpty()) {
            String pkg = packageList.removeFirst()
            if (pkg) {
                String parent = pkg - ~/\.?\w+$/
                packageList.add(parent)
                parents[pkg] = parent
            }
        }
        return parents
    }

    private static Map<String, Map<String, Integer>> interpolatePackages(Map<String, Map<String, Integer>> sparsePackages) {
        Map<String, Map<String, Integer>> res = [:].withDefault { [covered: 0, total: 0] }
        res.putAll(sparsePackages)
        def parents = mapPackagesToParents(res.keySet())
        parents.toSorted { -(it.key).count(".") }.each {
            pkg, parent ->
                res[parent] = addPackageCoverage(res[parent], res[pkg])
        }
        return res
    }
}
