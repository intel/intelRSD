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

package com.intel.podm.decrypter.cli;

import com.beust.jcommander.Parameter;

public class CommandLineArgs {
    @Parameter(names = "--password-type", description = "Type of password ('keystore' or 'database').",
            converter = PasswordTypeConverter.class, required = true)
    private PasswordType passwordType;

    @Parameter(names = "--help", description = "Print this help.", help = true)
    private boolean help;

    public PasswordType getPasswordType() {
        return passwordType;
    }

    public boolean isHelp() {
        return help;
    }
}
