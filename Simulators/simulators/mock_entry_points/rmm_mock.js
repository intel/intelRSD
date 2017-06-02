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

const program = require('commander');
const Rmm = require('../services/rmm').Rmm;

program
    .option('-p, --port <n>', 'Port number')
    .option('-u, --uuid <uuid>', 'Mock uuid')
    .option('-r, --rackpuid <rackpuid>', 'RackPUID')
    .option('-l, --locationid <locationid>', 'Location.Id')
    .option('-L, --no-location', 'Hide Location property')
    .parse(process.argv);

const rmmMock = new Rmm(program.port, program.uuid);

rmmMock.Rack.RackPuid = parseInt(program.rackpuid || "0");
rmmMock.Rack.LocationId = program.locationid || "0";
rmmMock.Rack.LocationPropertyVisible = program.location;

rmmMock.Start();

exports.rmmMock = rmmMock;
