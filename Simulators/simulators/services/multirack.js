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

const Rmm = require('./rmm').Rmm;
const Psme = require('./psme').Psme;
const UUID = require('uuid');

class Multirack {
    constructor() {
        this.rmmCollection = [];
        this.nextRmmPort = 7813;
        this.nextRackLocation = 1;

        this.psmeCollection = [];
        this.nextPsmePort = 9444; // 9443 is default port reserved by mock on test_runner startup

        this.psmeNumber = 0;
    }

    StartNewPsme(settings) {
        let newPsmePort = this.NextPsmePort();
        let newPsme = new Psme(newPsmePort, UUID.v1(), ['ssdp', 'dhcp'], settings);
        let psmeNumber = this.psmeNumber++;

        this.psmeCollection[psmeNumber] = newPsme;
        newPsme.mockId = psmeNumber;
        newPsme.Start();

        return newPsme;
    }

    StartNewRmm(rackLocationId) {
        let newRmmPort = this.NextRmmPort();
        let newRackLocationId = this.NextRackLocation(rackLocationId);
        let newRmm = new Rmm(newRmmPort);

        newRmm.Rack.LocationId = newRackLocationId;
        this.rmmCollection[newRackLocationId] = newRmm;
        newRmm.Start();

        return newRmm;
    }

    NextPsmePort() {
        return this.nextPsmePort++;
    }

    NextRmmPort() {
        return this.nextRmmPort++;
    }

    NextRackLocation(rackLocationId) {
        var newRackLocationId = rackLocationId || this.nextRackLocation;
        this.nextRackLocation = Math.max(newRackLocationId, this.nextRackLocation) + 1;
        return newRackLocationId;
    }

    get Rmms() {
        return this.rmmCollection;
    }

    GetRmmWithLocationId(rackLocationId) {
        return this.rmmCollection[rackLocationId];
    }

    StopRmm(rackLocationId) {
        this.rmmCollection[rackLocationId].Stop();
        this.rmmCollection.splice(rackLocationId, 1);
    }

    StopPsme(mockId) {
        this.psmeCollection[mockId].Stop();
        this.psmeCollection.splice(mockId, 1);
    }

    StopAllPsmes() {
        this.psmeCollection.forEach(psme => psme.Stop());
        this.psmeCollection = [];
    }

    StopAllRmms() {
        this.rmmCollection.forEach(rmm => rmm.Stop());
        this.rmmCollection = [];
    }
}

exports.Multirack = Multirack;

