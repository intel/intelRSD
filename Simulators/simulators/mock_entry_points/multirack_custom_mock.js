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

const program = require('commander');
const Multirack = require('../services/multirack').Multirack;

var multirackMock = new Multirack();

program
    .option('-r, --racks <n>', 'Racks count', parseInt)
    .option('-d, --drawers <n>', 'Drawers count in each rack', parseInt)
    .option('-s, --sleds <n>', 'Sleds count in each drawer', parseInt)
    .option('-b, --blades <n>', 'Blades count in each sled', parseInt)
    .parse(process.argv);

console.log('Running mock with additional custom settings:');
console.log('\tRacks: ' + (program.racks || 0));
console.log('\tDrawers: ' + (program.drawers || 0));
console.log('\tSleds: ' + (program.sleds || 0));
console.log('\tBlades: ' + (program.blades || 0));

var settings = {
    custom: true,
    drawers: program.drawers,
    sleds: program.sleds,
    blades: program.blades
};

for(var rackNum = 0; rackNum < program.racks; rackNum++) {
    var rackId = rackNum.toString();
    var rmm = multirackMock.StartNewRmm(rackId);
    var psme = multirackMock.StartNewPsme(settings);

    for(var drawer of psme.Drawers) {
        drawer.ParentId = rackId;
        var drawerUri = drawer.GetBaseUri();
        psme.EventTrigger.FireEvent(drawerUri, 'ResourceUpdated');
    }
}

