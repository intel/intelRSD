/*
 * Copyright (c) 2015 Intel Corporation
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
package com.intel.podm.ipxesupplier;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.net.MacAddress;
import org.apache.commons.lang.ObjectUtils;

import javax.inject.Inject;
import javax.ws.rs.GET;
import javax.ws.rs.NotFoundException;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.QueryParam;
import java.text.MessageFormat;

import static javax.ws.rs.core.MediaType.TEXT_PLAIN;

@Path("ipxe")
public class IpxeService {
    @Inject
    private IpxeDirectory directory;
    @Inject
    private Logger logger;

    @GET
    @Produces(TEXT_PLAIN)
    public Object getRemoteTarget(@QueryParam("mac") String macAddressString,
                                  @QueryParam("uuid") String uuid,
                                  @QueryParam("ip") String ip) {
        try {
            logger.d("Got request for IPXE script " + getHostDetailsString(macAddressString, uuid, ip));

            MacAddress macAddress;
            try {
                macAddress = new MacAddress(macAddressString);
            } catch (IllegalArgumentException iae) {
                throw new AssetNotFoundException("MAC Address is not valid", iae);
            }

            String ipxeScript = directory.getIpxeScript(macAddress).toIpxeScript();
            logger.d("IPXE script response: [\n" + ipxeScript + "\n]");
            return ipxeScript;
        } catch (AssetNotFoundException e) {
            logger.e("Error while handling request for IPXE script " + getHostDetailsString(macAddressString, uuid, ip) + ": " + e.getMessage(), e);
            throw new NotFoundException();
        }
    }

    private String getHostDetailsString(String macAddressString, String uuid, String ip) {
        return MessageFormat.format(
                "(from MAC: [{0}], UUID: [{1}], IP: [{2}])",
                ObjectUtils.toString(macAddressString),
                ObjectUtils.toString(uuid),
                ObjectUtils.toString(ip)
        );
    }
}
