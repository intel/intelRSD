/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * */

#include "default_configuration.hpp"
#include "psme/ssdp/ssdp_config_loader.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "gtest/gtest.h"


namespace {

const std::string service_uuid = "a7f09664-2181-11e6-96be-0ba482e91a3c";

json::Json get_default_config() {
    json::Json default_config = json::Json();
    default_config = json::Json::parse(psme::app::DEFAULT_CONFIGURATION);
    return default_config;
}

}

namespace ssdp {

using namespace testing;

TEST(LoadSsdpConfigTest, LoadFromDefaultConfiguration) {
    const auto ssdp_config = load_ssdp_config(get_default_config(), service_uuid);
    ASSERT_TRUE(ssdp_config.is_ssdp_service_enabled());
    ASSERT_FALSE(ssdp_config.is_announcement_enabled());
    ASSERT_EQ(2, ssdp_config.get_socket_ttl());
    ASSERT_EQ("https://localhost:8443/redfish/v1", ssdp_config.get_service_url());
    ASSERT_EQ("urn:dmtf-org:service:redfish-rest:1", ssdp_config.get_service_urn());
    ASSERT_EQ(service_uuid, ssdp_config.get_service_uuid());
    ASSERT_EQ(std::chrono::seconds(0), ssdp_config.get_announce_interval());
    ASSERT_EQ(1, ssdp_config.get_nic_names().size());
    ASSERT_EQ("enp0s20f0.4094", *ssdp_config.get_nic_names().cbegin());
}

}
