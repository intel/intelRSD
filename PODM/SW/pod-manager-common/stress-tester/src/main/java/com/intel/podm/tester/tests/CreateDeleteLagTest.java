/*
 * Copyright (c) 2016-2017 Intel Corporation
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
package com.intel.podm.tester.tests;

import com.intel.podm.common.types.PortType;
import com.intel.podm.components.EthernetSwitch;
import com.intel.podm.components.EthernetSwitchPort;
import com.intel.podm.components.Members;
import com.intel.podm.providers.client.RestClient;
import com.intel.podm.tester.utilities.LagCreationJson;
import com.intel.podm.tester.utilities.dictionaries.ResourceUrlProvider;
import org.apache.http.HttpStatus;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.testng.Assert;
import org.testng.TestException;
import org.testng.annotations.AfterTest;
import org.testng.annotations.BeforeTest;
import org.testng.annotations.Parameters;
import org.testng.annotations.Test;

import javax.ws.rs.core.Response;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class CreateDeleteLagTest {
    private static final Logger LOGGER = LoggerFactory.getLogger(CreateDeleteLagTest.class);

    private List<String> failedActionMessages = new ArrayList<>();

    private EthernetSwitch ethernetSwitch;
    private EthernetSwitchPort ethernetSwitchPort;

    private RestClient restClient;
    private ResourceUrlProvider resourceUrlProvider;

    @Parameters({"podmBaseUrl", "encodedPodmUserPassword"})
    @BeforeTest
    public void before(String podmBaseUrl, String encodedPodmUserPassword) {
        this.restClient = new RestClient(encodedPodmUserPassword);
        this.resourceUrlProvider = new ResourceUrlProvider(podmBaseUrl);
        this.ethernetSwitch = getEthernetSwitch();
        this.ethernetSwitchPort = getEthernetSwitchPort();
    }

    @Parameters("createDeleteLagRepetitionCount")
    @Test
    public void createDeleteLagTest(Integer createDeleteLagRepetitionCount) {
        int repeatCounter = 0;
        LOGGER.info("Create delete test started");
        while (repeatCounter < createDeleteLagRepetitionCount) {
            performCreateDeleteLagActions(repeatCounter);
            repeatCounter++;
        }
        LOGGER.info("Create delete test finished");
        Assert.assertEquals(failedActionMessages.size(), 0, failedActionMessages.toString());
    }

    @AfterTest
    public void closeRestClient() {
        restClient.closeClient();
    }

    private void performCreateDeleteLagActions(int createDeleteLagRepetitionCount) {
        Response createdLagActionResponse = createLag();
        if (createdLagActionResponse.getStatus() != HttpStatus.SC_CREATED) {
            failedActionMessages.add("Create Lag failed -> iteration -> " + createDeleteLagRepetitionCount
                    + " due to error: " + getResponseAsString(createdLagActionResponse));
            createdLagActionResponse.close();
        } else {
            URI createdLagLocation = createdLagActionResponse.getLocation();
            createdLagActionResponse.close();
            Response deleteLagResponse = deleteLag(createdLagLocation);
            checkDeleteActionStatus(deleteLagResponse, createDeleteLagRepetitionCount);
        }
    }

    private EthernetSwitch getEthernetSwitch() {
        LOGGER.debug("Get ethernet switch");
        List<String> ethernetSwitchUrlCollection = getMembersCollection(resourceUrlProvider.getEthernetSwitchCollectionUrl());
        return ethernetSwitchUrlCollection.stream()
                .map(url -> restClient.get(resourceUrlProvider.getUrlWithPodmBaseUrlAdded(url)).readEntity(EthernetSwitch.class))
                .findFirst().orElseThrow(() -> new TestException("Ethernet switch not found, test can not be executed"));
    }

    private EthernetSwitchPort getEthernetSwitchPort() {
        List<String> portUrls = getMembersCollection(resourceUrlProvider.getEthernetSwitchPortsUrl(ethernetSwitch.getOdataId()));
        return portUrls.stream().map(this::getEthernetSwitchPortFromResponse)
                .filter(Objects::nonNull).filter(ethernetSwitchPort -> PortType.UPSTREAM.getValue().equals(ethernetSwitchPort.getPortType()))
                .findFirst().orElseThrow(() -> new TestException("Ethernet switch port of type 'Upstream' not found. "
                        + "LAGs can be created only on upstream physical ports that have matching speeds."));
    }

    private List<String> getMembersCollection(URI collectionUri) {
        Response response = restClient.get(collectionUri);
        Members members = response.readEntity(Members.class);
        response.close();
        return members.getUris();
    }

    private EthernetSwitchPort getEthernetSwitchPortFromResponse(String switchPortUrl) {
        Response response = restClient.get(resourceUrlProvider.getUrlWithPodmBaseUrlAdded(switchPortUrl));
        EthernetSwitchPort ethernetSwitchPort = response.readEntity(EthernetSwitchPort.class);
        response.close();
        return ethernetSwitchPort;
    }

    private Response createLag() {
        LOGGER.info("Create lag");
        URI lagCreateUri = resourceUrlProvider.getEthernetSwitchPortsUrl(ethernetSwitch.getOdataId());
        LagCreationJson lagCreationJson = new LagCreationJson(ethernetSwitchPort.getOdataId());
        return restClient.post(lagCreateUri, lagCreationJson);
    }

    private Response deleteLag(URI lagToBeDeletedUri) {
        LOGGER.info("Delete lag " + lagToBeDeletedUri);
        return restClient.delete(lagToBeDeletedUri);
    }

    private void checkDeleteActionStatus(Response deleteActionResponse, int repeatCounter) {
        if (deleteActionResponse.getStatus() != HttpStatus.SC_NO_CONTENT) {
            failedActionMessages.add("Delete Lag action failed: iteration -> " + repeatCounter
                    + " due to error: " + getResponseAsString(deleteActionResponse));
        }
    }

    private String getResponseAsString(Response response) {
        String entity = response.readEntity(String.class);
        response.close();
        return entity;
    }
}
