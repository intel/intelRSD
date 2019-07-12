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

import com.bmuschko.gradle.docker.DockerExtension
import com.bmuschko.gradle.docker.DockerRemoteApiPlugin
import com.bmuschko.gradle.docker.tasks.image.DockerPushImage
import com.bmuschko.gradle.docker.tasks.image.DockerTagImage
import org.gradle.api.GradleException
import org.gradle.api.Plugin
import org.gradle.api.Project

class PodmServiceDockerPlugin implements Plugin<Project> {
    private final String DOCKER_REGISTRY_HOST_VAR_NAME = "DOCKER_REGISTRY_HOST"

    @Override
    void apply(Project target) {
        def dockerRemoteApiPlugin = new DockerRemoteApiPlugin()
        dockerRemoteApiPlugin.apply(target)

        DockerExtension dockerExtension = target.extensions.getByType(DockerExtension)
        dockerExtension.url = System.getenv('DOCKER_HOST') ?: 'unix:///var/run/docker.sock'
        dockerExtension.certPath = System.getenv('DOCKER_CERT_PATH') ? new File(System.getenv('DOCKER_CERT_PATH')) : null
        def dockerRegistryHost = System.getenv(DOCKER_REGISTRY_HOST_VAR_NAME) ?: ''
        def dockerRegistryHostUri = URI.create(dockerRegistryHost)

        if (!dockerRegistryHost.isAllWhitespace() && !dockerRegistryHostUri.isAbsolute()) {
            throw new GradleException("URI provided for $DOCKER_REGISTRY_HOST_VAR_NAME env variable should be absolute")
        }

        target.extensions.create("podmService", PodmServiceExtension, target)

        target.task("createDockerfile", type: PodmServiceDockerfile, dependsOn: 'bootJar')
        def version = target.version == "unspecified" ? "dev" : target.version
        target.task('buildJavaEntrypoint') {
            def entrypointDstFile = new File(target.buildDir, "entrypoint.sh")
            outputs.file(entrypointDstFile)
            doLast {
                getClass().getResourceAsStream('/entrypoint.sh').withReader { reader ->
                    entrypointDstFile.write reader.text
                }
            }
        }

        def buildDockerImageTask = target.task("buildDockerImage", type: PodmServiceDockerBuildImage, dependsOn: ['createDockerfile', 'buildJavaEntrypoint'])

        target.task("tagDockerImage", type: DockerTagImage, dependsOn: 'buildDockerImage') {
            repository = "$dockerRegistryHost/${(project.extensions.getByType(PodmServiceExtension).appName ?: target.name)}"
            //todo: if scheme is not provided then host is set to null...
            tag = version
            targetImageId { buildDockerImageTask.getImageId() }
            doFirst {
                if (dockerRegistryHost.isAllWhitespace()) {
                    throw new GradleException("$DOCKER_REGISTRY_HOST_VAR_NAME env variable is not set")
                }
                println("Using docker registry host: $dockerRegistryHost")
                logger.quiet("tagDockerImage; repository='${repository ?: "(null)"}'; tag='${tag ?: "(null)"}'; imageId='${imageId ?: "(null)"}'")
            }
        }

        target.task("pushDockerImage", type: DockerPushImage, dependsOn: 'tagDockerImage') {
            imageName = "$dockerRegistryHost/${(project.extensions.getByType(PodmServiceExtension).appName ?: target.name)}"
            tag = version
            registryCredentials = dockerExtension.registryCredentials
            doFirst {
                logger.quiet("""pushDockerImage; imageName='${imageName ?: "(null)"}'; tag='${tag ?: "(null)"}'""")
            }
        }
    }
}

