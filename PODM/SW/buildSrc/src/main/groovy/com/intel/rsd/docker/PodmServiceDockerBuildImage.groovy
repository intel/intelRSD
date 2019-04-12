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

package com.intel.rsd.docker

import com.bmuschko.gradle.docker.tasks.image.DockerBuildImage

class PodmServiceDockerBuildImage extends DockerBuildImage {
    static class Signature {
        static final Signature UNSIGNED = new Signature()

        int computedHash
        boolean present

        Signature(int hash) {
            computedHash = hash
            present = true
        }

        Signature() {
            present = false
        }
    }

    enum SignatureCheck {
        MISSING(true), UNSIGNED, OUTDATED(true), UPTODATE
        boolean shouldGenerate = false

        SignatureCheck(boolean shouldGenerate = false) {
            this.shouldGenerate = shouldGenerate
        }
    }

    PodmServiceDockerBuildImage() {
        dockerFile = new File(project.buildDir, 'docker/Dockerfile')
        inputDir = project.rootDir
        remove = true
    }

    @Override
    void runRemoteCommand(Object dockerClient) {
        def podmServiceExtension = project.extensions.getByType(PodmServiceExtension)
        def version = project.version == "unspecified" ? "dev" : project.version
        tag = "${(podmServiceExtension.appName ?: project.name)}:$version"
        def dockerignoreFile = new File(inputDir, ".dockerignore")
        prepareDockerignore(dockerignoreFile, podmServiceExtension.ignorePatterns.toList())
        super.runRemoteCommand(dockerClient)
    }

    void prepareDockerignore(File targetFile, List<String> ignorePatterns) {
        def signatureCheck = runSignatureCheck(targetFile, ignorePatterns)
        switch (signatureCheck) {
            case SignatureCheck.MISSING:
                logger.quiet("generating .dockerignore from configured patterns: ${ignorePatterns}")
                break
            case SignatureCheck.UNSIGNED:
                logger.quiet("NOT generating .dockerignore - using provided file")
                break
            case SignatureCheck.UPTODATE:
                logger.quiet("previously generated .dockerignore is up to date")
                break
            case SignatureCheck.OUTDATED:
                logger.quiet("generated .dockerignore is outdated; generating from configured patterns: ${ignorePatterns}")
                break
        }
        if (signatureCheck.shouldGenerate) {
            writeDockerignorePatterns(targetFile, ignorePatterns)
        }
    }

    static void writeDockerignorePatterns(File targetFile, List<String> ignorePatterns) {
        targetFile.withWriter() { writer ->
            writer.println("##Auto-Generated; hash: ${ignorePatterns.toList().hashCode()}")
            ignorePatterns.each { line ->
                writer.println(line)
            }
        }
    }

    static Signature getDockerignoreSignature(File targetFile) {
        def lines = new ArrayList<String>()
        def signaturePtrn = /##Auto-Generated; hash: ([0-9-]+)\s*/
        def signature = Signature.UNSIGNED
        targetFile.readLines().each { line ->
            def match = (line =~ signaturePtrn)
            if (match) {
                signature = new Signature(match[0][1] as Integer)
            } else {
                lines.add(line)
            }
        }
        return signature
    }

    static SignatureCheck runSignatureCheck(File targetFile, List<String> patterns) {
        def targetSignature = (targetFile.exists()) ?getDockerignoreSignature(targetFile) :Signature.UNSIGNED
        SignatureCheck check = null
        if (!targetFile.exists()) {
            check = SignatureCheck.MISSING
        } else if (targetSignature == Signature.UNSIGNED) {
            check = SignatureCheck.UNSIGNED
        } else if (targetSignature.computedHash != patterns.hashCode()) {
            check = SignatureCheck.OUTDATED
        } else {
            check = SignatureCheck.UPTODATE
        }
    }
}
