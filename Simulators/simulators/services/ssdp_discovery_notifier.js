/*
 * Copyright (c) 2015-2017 Intel Corporation
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
const SsdpResource = require('node-ssdp').Server;

class SsdpDiscoveryNotifier {
    constructor(uuid, location) {
        this._serviceUuid = uuid;
        this._serviceLocation = location;

        this._ssdpNotifier = undefined;
    }

    start() {
        console.log("Running SSDP discovery for " + this._serviceLocation);
        this._ssdpNotifier = new SsdpResource({
            location: this._serviceLocation,
            udn: 'uuid:' + this._serviceUuid,
            ssdpTtl: 200,
            adInterval: 90*1000

        });
        this._ssdpNotifier.addUSN('urn:dmtf-org:service:redfish-rest:1');
        this._ssdpNotifier.start();
    }

    stop () {
        this._ssdpNotifier.stop();
    }
}

exports.SsdpDiscoveryNotifier = SsdpDiscoveryNotifier;