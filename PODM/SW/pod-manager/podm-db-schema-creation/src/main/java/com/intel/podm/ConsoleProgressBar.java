/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm;

import static com.google.common.base.Preconditions.checkArgument;

public class ConsoleProgressBar implements AutoCloseable {
    private final int percentageMultiplier = 100;
    private int maxElementsToProceed;
    private int counter;

    public ConsoleProgressBar(int maxElementsToProceed) {
        checkArgument(maxElementsToProceed > 0, "maxElementsToProceed must be a positive integer value.");
        this.counter = 0;
        this.maxElementsToProceed = maxElementsToProceed;
    }

    @Override
    public void close() {
        System.out.print("\n");
    }

    public void addProgress() {
        update(++counter * percentageMultiplier / maxElementsToProceed);
    }

    private void update(int percent) {
        StringBuilder bar = new StringBuilder("[");
        final int barLength = 50;
        final int coefficient = 100 / barLength;

        for (int i = 0; i < barLength; i++) {
            if (i < (percent / coefficient)) {
                bar.append("=");
            } else if (i == (percent / coefficient)) {
                bar.append(">");
            } else {
                bar.append(" ");
            }
        }

        bar.append("]   " + percent + "%     ");
        System.out.print("\r" + bar.toString());
    }
}
