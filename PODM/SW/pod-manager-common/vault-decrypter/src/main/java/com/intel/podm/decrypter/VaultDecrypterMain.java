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
package com.intel.podm.decrypter;

import com.beust.jcommander.JCommander;
import com.intel.podm.decrypter.cli.CommandLineArgs;

@SuppressWarnings({"checkstyle:UncommentedMain"})
public final class VaultDecrypterMain {
    private final CommandLineArgs commandLineArgs;

    private VaultDecrypterMain(CommandLineArgs commandLineArgs) {
        this.commandLineArgs = commandLineArgs;
    }

    private String getPassword() {
        PodmVaultDecrypter decrypter = new PodmVaultDecrypter();
        return decrypter.getPasswordUnder(commandLineArgs.getPasswordType().getPassPath());
    }

    public static void main(String[] args) {
        CommandLineArgs commandLineArgs = new CommandLineArgs();
        JCommander jCommander = new JCommander(commandLineArgs, args);
        jCommander.setProgramName("VaultDecrypter");

        if (commandLineArgs.isHelp()) {
            jCommander.usage();
            return;
        }

        VaultDecrypterMain vaultDecrypterMain = new VaultDecrypterMain(commandLineArgs);
        System.out.println(vaultDecrypterMain.getPassword());
    }
}
