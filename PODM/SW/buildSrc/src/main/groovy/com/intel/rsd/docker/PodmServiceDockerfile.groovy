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

import com.bmuschko.gradle.docker.tasks.image.Dockerfile
import org.gradle.api.tasks.TaskAction

class PodmServiceDockerfile extends Dockerfile {

    def relativeToProjectRootDir(File path) {
        return project.rootDir.toURI().relativize(path.toURI()).getPath()
    }

    def relativeToProjectRootDir(String path) {
        return project.rootDir.toURI().relativize(path.toURI()).getPath()
    }

    @TaskAction
    void create() {
        def podmServiceExtension = project.extensions.getByType(PodmServiceExtension)

        def profiles = podmServiceExtension.profiles
        String[] springProfiles = profiles == null ? [] : ["--spring.profiles.active=${profiles.join(",")}"]
        String[] appArgs = podmServiceExtension.appArgs ?: []
        String[] remoteDebug = podmServiceExtension.remoteDebugEnabled ? ["-Xdebug", "-Xrunjdwp:server=y,transport=dt_socket,address=8000,suspend=n"] : []

        def uid = 1500

        from("openjdk:8u181-jre-alpine")
        def entrypointDstFile = new File(project.buildDir, "entrypoint.sh")
        copyFile(relativeToProjectRootDir(entrypointDstFile), "/usr/local/bin/entrypoint.sh")
        runCommand("mkdir -p /app/javaopts; chmod +x /usr/local/bin/entrypoint.sh")
        entryPoint("sh", "/usr/local/bin/entrypoint.sh")
        workingDir("/app")
        runCommand("addgroup -g $uid podm && adduser -D -G podm -S -u $uid podm && chgrp -R 0 /app && chmod -R g=u /app")
        user(uid.toString())
        def effectiveLabels = ["withCoverage": "enabled",
                               "podm-service": podmServiceExtension.appName ?: project.name]
        podmServiceExtension.labels.each {
            effectiveLabels[it.key] = it.value
        }
        label(effectiveLabels)

        environmentVariable("${project.name.toUpperCase()}_VERSION", project.version)
        defaultCommand(
                'java', "-XX:MaxHeapSize=${podmServiceExtension.maxHeapSize}", //todo: validate maxHeapSize
                *remoteDebug,
                '-jar', "${project.name}.jar",
                *springProfiles,
                *appArgs
        )
        podmServiceExtension.includeFiles.each { copyFile(relativeToProjectRootDir(it.key), it.value) }
        copyFile(relativeToProjectRootDir(project.tasks.bootJar.archivePath).toString(), "/app/${project.name}.jar")

        super.create()
    }
}
