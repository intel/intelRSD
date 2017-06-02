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

const Service = require('./service').Service;
const ServiceRoot = require('../api/rmm/serviceRoot').RmmServiceRoot;
const Rmc = require('../api/rmm/managers/rmc').Rmc;
const UUID = require('uuid');

class Rmm extends Service {
    constructor(port = '7812', uuid = UUID.v1(), discoveryProtocols = ["dhcp"]) {
        super(uuid, discoveryProtocols, port, 'rmm', '/redfish/v1');

        this.serviceRoot = new ServiceRoot(this.mock, this.serviceRoot.uuid);

        this.eventTrigger = this.serviceRoot.EventTrigger;
        this.rack = this.serviceRoot.AddRack();
        this.drawer = this.AddDrawer();
        this.drawer.BindToRack(this.rack);
        this.thermalZone = this.rack.AddThermalZone();
        this.thermalZone.AddFan();

        this.powerZone = this.rack.AddPowerZone();
        this.powerZone.AddPsu();

        this.thermal = this.rack.Thermal;
        var thermalFan = this.thermal.AddThermalFan();
        var redundancy = this.thermal.AddRedundancy();
        var thermalTemp = this.thermal.AddThermalTemperature();

        redundancy.AddToRedundancySet(thermalFan);
        thermalTemp.AddToRelatedItems(this.rack);
        thermalFan.AddToRelatedItems(this.rack);
        thermalFan.AddToRedundancySet(redundancy);

        var rmc = this.serviceRoot.AddRmc();
        rmc.AddManagerForChassis(this.rack);
        rmc.AddEthernetInterface().AddVlan();

        var mbpc = this.serviceRoot.AddMbpc();

        var mbp = this.rack.AddMbp();
        mbpc.AddMbpToManager(mbp);


    }

    get EventTrigger() {
        return this.serviceRoot.EventTrigger;
    }

    get Rack() {
        return this.rack;
    }

    get Drawers() {
        return this.serviceRoot.Drawers;
    }

    get ThermalZone() {
        return this.thermalZone;
    }

    get PowerZone() {
        return this.powerZone;
    }

    AddDrawer() {
        return this.serviceRoot.AddDrawer();
    }

    RemoveDrawerWithId(id) {
        return this.serviceRoot.RemoveDrawerWithId(id);
    }

    get ServiceRoot() {
        return this.serviceRoot;
    }

    get Managers() {
        return this.serviceRoot.Managers;
    }
}

exports.Rmm = Rmm;
