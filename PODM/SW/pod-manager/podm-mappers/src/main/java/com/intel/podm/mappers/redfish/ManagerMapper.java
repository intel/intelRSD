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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.properties.Console;
import com.intel.podm.business.entities.redfish.properties.GraphicalConsole;
import com.intel.podm.client.api.resources.redfish.ConsoleObject;
import com.intel.podm.client.api.resources.redfish.GraphicalConsoleObject;
import com.intel.podm.client.api.resources.redfish.ManagerResource;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class ManagerMapper extends DomainObjectMapper<ManagerResource, Manager> {

    @Inject
    private GenericDao genericDao;

    public ManagerMapper() {
        super(ManagerResource.class, Manager.class);
        registerProvider(Console.class, this::provideConsole);
        registerProvider(GraphicalConsole.class, this::provideGraphicalConsole);
    }

    private Console provideConsole(ConsoleObject consoleObject) {
        Console console;
        if (consoleObject.equals(source.getCommandShell())) {
            console = target.getCommandShell();
        } else if (consoleObject.equals(source.getSerialConsole())) {
            console = target.getSerialConsole();
        } else {
            throw new RuntimeException("Unknown console: " + consoleObject);
        }

        if (console == null) {
            console = genericDao.create(Console.class);
        }

        return console;
    }

    private GraphicalConsole provideGraphicalConsole(GraphicalConsoleObject graphicalConsoleObject) {
        GraphicalConsole graphicalConsole;
        if (graphicalConsoleObject.equals(source.getGraphicalConsole())) {
            graphicalConsole = target.getGraphicalConsole();
        } else {
            throw new RuntimeException("Unknown graphical console: " + graphicalConsoleObject);
        }

        if (graphicalConsole == null) {
            graphicalConsole = genericDao.create(GraphicalConsole.class);
        }

        return graphicalConsole;
    }

    @Override
    protected void performNotAutomatedMapping(ManagerResource source, Manager target) {
        super.performNotAutomatedMapping(source, target);
    }
}
