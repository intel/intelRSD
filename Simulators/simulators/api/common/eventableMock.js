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

const readline = require('readline');

class EventableMock {
    constructor(prompt, menuOptions) {
        this.CreateInterface();
        this.Prompt = prompt;
        this.ShowEventsMenu(menuOptions);
    }

    CreateInterface() {
        this.rl = readline.createInterface({
            input: process.stdin,
            output: process.stdout
        });
    }

    ShowEventsMenu(options) {
        console.log('Options:');
        for(let option of options) {
            console.log(option);
        }
    }

    set Prompt(prompt) {
        this.rl.setPrompt(`${prompt} Mock> `);
    }

    set MockBehavior(func) {
        this.rl.on('line', func.bind(this));
    }

    Start() {
        this.rl.prompt();
    }
}

exports.EventableMock = EventableMock;