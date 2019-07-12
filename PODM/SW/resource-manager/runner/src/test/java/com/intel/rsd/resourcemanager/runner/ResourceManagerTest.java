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

package com.intel.rsd.resourcemanager.runner;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.ContextConfiguration;
import org.springframework.test.context.TestPropertySource;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import java.util.Objects;

import static java.util.Collections.singletonList;
import static java.util.UUID.randomUUID;
import static org.assertj.core.api.Assertions.assertThat;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.MediaType.APPLICATION_JSON;

@ActiveProfiles("test")
@SpringBootTest(
    webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT,
    properties = "config=configs/resource-manager.yml"
)
@ContextConfiguration(initializers = RunnerApplication.ExternalModuleClassPathInitializer.class)
@AutoConfigureTestDatabase
@TestPropertySource("classpath:tagger-test.properties")
@EnableDiscoveryClient(autoRegister = false)
public class ResourceManagerTest extends AbstractTestNGSpringContextTests {
    @Autowired
    private TestRestTemplate restTemplate;

    @Test
    public void resourceManagerIsResponding() {
        val response = restTemplate.getForEntity("/redfish/v1", String.class);
        assertThat(response.getStatusCode()).isEqualTo(OK);
    }

    @Test
    public void resourceManagerExposesPodChassis() {
        val response = restTemplate.getForEntity("/redfish/v1/Chassis/pod", String.class);
        assertThat(response.getStatusCode()).isEqualTo(OK);
    }

    @Test
    public void resourceManagerExposesPodManager() {
        val response = restTemplate.getForEntity("/redfish/v1/Managers/PodManager", String.class);
        assertThat(response.getStatusCode()).isEqualTo(OK);
    }

    @Test(suiteName = "tagger", testName = "PATCH tagger-managed AssetTag", dependsOnGroups = "idempotent")
    public void assetTagOfPodChassisCanBePatched() {
        String podUrl = "/redfish/v1/Chassis/pod";
        String assetTagBeforePatch = getAssetTagAtUrl(podUrl);

        val assetTagToSet = "tag-" + randomUUID();
        patchAssetTagAtUrl(podUrl, assetTagToSet);
        String assetTagAfterPatch = getAssetTagAtUrl(podUrl);

        assertThat(assetTagAfterPatch).isNotEqualTo(assetTagBeforePatch);
        assertThat(assetTagAfterPatch).isEqualTo(assetTagToSet);
    }

    @Test(suiteName = "tagger", groups = "idempotent", testName = "PATCH tagger-managed AssetTag property with null before it was assigned any value")
    public void patchingNotPreviouslySetAssetTagWithNullShouldNotChangeResponse() {
        String podUrl = "/redfish/v1/Chassis/pod";
        String assetTagBeforePatch = getAssetTagAtUrl(podUrl);

        patchAssetTagAtUrl(podUrl, null);
        String assetTagAfterPatch = getAssetTagAtUrl(podUrl);

        assertThat(assetTagAfterPatch).isEqualTo(assetTagBeforePatch);
    }

    @Test(suiteName = "tagger", groups = "idempotent", testName = "PATCH tagger-managed AssetTag already set property with null should unset the AssetTag")
    public void patchingAssetTagWithNullAfterSuccessfulPatchShouldUnsetAssetTag() {
        String podUrl = "/redfish/v1/Chassis/pod";
        String assetTagBeforePatch = getAssetTagAtUrl(podUrl);

        val assetTagToSet = "tag-" + randomUUID();
        patchAssetTagAtUrl(podUrl, assetTagToSet);
        String assetTagAfterPatch = getAssetTagAtUrl(podUrl);

        assertThat(assetTagAfterPatch).isNotEqualTo(assetTagBeforePatch);
        assertThat(assetTagAfterPatch).isEqualTo(assetTagToSet);

        patchAssetTagAtUrl(podUrl, null);
        String assetTagAfterSecondPatch = getAssetTagAtUrl(podUrl);

        assertThat(assetTagAfterSecondPatch).isEqualTo(assetTagBeforePatch);
    }

    private String getAssetTagAtUrl(String url) {
        val response = restTemplate.getForEntity(url, JsonNode.class);
        assertThat(response.getStatusCode()).isEqualTo(OK);
        Objects.requireNonNull(response.getBody());
        return response.getBody().at("/AssetTag").textValue();
    }

    private void patchAssetTagAtUrl(String url, String assetTagToSet) {
        val nullAwareAssetTag = assetTagToSet != null ? ("\"" + assetTagToSet + "\"") : "null";
        val body = "{\"AssetTag\":" + nullAwareAssetTag + "}";
        val headers = new HttpHeaders();
        headers.setAccept(singletonList(APPLICATION_JSON));
        restTemplate.exchange(url, PATCH, new HttpEntity<>(body, headers), JsonNode.class);
    }
}
