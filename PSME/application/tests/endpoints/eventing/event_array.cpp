/*!
 * @brief Subscriptions Event Array tests
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file event_array.cpp
 */

#include "psme/rest/eventing/event_array.hpp"
#include "gtest/gtest.h"

using namespace psme::rest::eventing;

TEST(EventArrayTests, ContainsVariousOriginOfResource) {
    EventArray array{
        EventVec{
            Event{EventType::ResourceAdded, "path/to/resource/a"},
            Event{EventType::ResourceAdded, "path/to/resource/b"},
            Event{EventType::ResourceAdded, "path/to/resource/c"},
        }
    };

    array.remove_duplicates();
    auto events = array.get_events();
    ASSERT_EQ(3, events.size());
}

TEST(EventArrayTests, ContainsVariousTypesOfEvent) {
    EventArray array{
        EventVec{
            Event{EventType::ResourceAdded, "path/to/resource/a"},
            Event{EventType::Alert, "path/to/resource/a"},
            Event{EventType::ResourceUpdated, "path/to/resource/a"},
        }
    };

    array.remove_duplicates();
    auto events = array.get_events();
    ASSERT_EQ(3, events.size());
}

TEST(EventArrayTests, RemovesDuplicatedTypesOfEvent) {
    EventArray array{
        EventVec{
            Event{EventType::ResourceAdded, "path/to/resource/a"},
            Event{EventType::ResourceAdded, "path/to/resource/a"},
            Event{EventType::ResourceUpdated, "path/to/resource/b"},
            Event{EventType::ResourceUpdated, "path/to/resource/b"},
            Event{EventType::ResourceAdded, "path/to/resource/c"},
            Event{EventType::ResourceAdded, "path/to/resource/c"},
            Event{EventType::Alert, "path/to/resource/a"},
            Event{EventType::Alert, "path/to/resource/a"},
        }
    };

    array.remove_duplicates();
    auto events = array.get_events();
    ASSERT_EQ(4, events.size());
}
