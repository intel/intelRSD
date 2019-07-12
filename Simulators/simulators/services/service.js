/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const WirelessMock = require('./wireless_mock').WirelessMock;
const DiscoveryNotifier = require('./discovery_notifier').DiscoveryNotifier;
const ServiceRoot = require('../api/serviceRoot').ServiceRoot;

class Service {
    constructor(
        uuid,
        discoveryProtocols,
        port = 8888,
        serviceType = 'psme',
        endpoint = '/redfish/v1'
    ) {
        this._mock = new WirelessMock({port: port});
        this.serviceRoot = new ServiceRoot(this._mock, uuid);
        this.eventTrigger = this.serviceRoot.EventTrigger;
        this.serviceType = serviceType;
        this.endpoint = endpoint;
        this.discoveryProtocols = discoveryProtocols;
    }
    get mock() {
        return this._mock;
    }

    Start () {
        this.mock.startListening(() => {
            this.discoveryNotifier = new DiscoveryNotifier(
                this.serviceType,
                this.serviceRoot.Uuid,
                this.mock.getBaseUri() + this.endpoint,
                this.discoveryProtocols
            );
            this.discoveryNotifier.startResponding();
        });
    }
    Stop () {
        this.mock.stop();
        if (this.discoveryNotifier) {
            this.discoveryNotifier.stopResponding();
        }
    }

    FireResourceAddedEvent(originOfCondition) {
        return this.eventTrigger.FireResourceAddedEvent(originOfCondition);
    }

    FireResourceRemovedEvent(originOfCondition) {
        return this.eventTrigger.FireResourceRemovedEvent(originOfCondition);
    }

    FireResourceUpdatedEvent(originOfCondition) {
        return this.eventTrigger.FireResourceUpdatedEvent(originOfCondition);
    }

    FireStatusChangedEvent(originOfCondition) {
        return this.eventTrigger.FireStatusChangedEvent(originOfCondition);
    }

    get EventTrigger() {
        return this.eventTrigger;
    }

    get ServiceRoot() {
        return this.serviceRoot;
    }

    get getPort() {
        return this.port;
    }
}

exports.Service = Service;
