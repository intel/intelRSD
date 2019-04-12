/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * */

#include "psme/rest/endpoints/chassis/chassis.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/constants/routes.hpp"
#include "psme/rest/server/request.hpp"
#include "psme/rest/server/response.hpp"
#include "psme/rest/server/multiplexer.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include "agent-framework/module/common_components.hpp"

#include "gtest/gtest.h"
#include "gmock/gmock.h"



using namespace psme::rest;
using namespace psme::rest::endpoint;
using namespace agent_framework::module;

class ChassisLocation : public ::testing::Test {
public:
    Chassis m_endpoint{constants::Routes::CHASSIS_PATH};
    std::string m_manager_uuid{};


    std::string add_chassis(uint64_t id, agent_framework::model::enums::ChassisType type, uint16_t location_offset,
                            const OptionalField<std::string>& location_id, const std::string& parent_id) {
        agent_framework::model::Chassis chassis{m_manager_uuid};
        chassis.set_id(id);
        chassis.set_location_id(location_id);
        chassis.set_parent_id(parent_id);
        chassis.set_location_offset(location_offset);
        chassis.set_type(type);
        get_manager<agent_framework::model::Chassis>().add_entry(chassis);
        return chassis.get_uuid();
    }


    std::string add_manager(uint64_t id) {
        agent_framework::model::Manager manager{};
        manager.set_id(id);
        get_manager<agent_framework::model::Manager>().add_entry(manager);
        return manager.get_uuid();
    }


    json::Json get_chassis_endpoint(uint64_t id) {
        server::Request req;
        req.set_destination(std::string{"/redfish/v1/Chassis/"} + std::to_string(id));
        req.params[constants::PathParam::CHASSIS_ID] = std::to_string(id);
        server::Response res;
        m_endpoint.get(req, res);

        return json::Json::parse(res.get_body());;
    }


    std::string get_parent_id(const json::Json& r) {
        return r.value(constants::Common::OEM, json::Json::object())
            .value(constants::Common::RACKSCALE, json::Json::object())
            .value(constants::Common::LOCATION, json::Json::object())
            .value(constants::Chassis::PARENT_ID, std::string{});
    }


    std::string get_location_id(const json::Json& r) {
        return r.value(constants::Common::OEM, json::Json::object())
            .value(constants::Common::RACKSCALE, json::Json::object())
            .value(constants::Common::LOCATION, json::Json::object())
            .value(constants::Common::ID, std::string{});
    }


    bool has_contained_by_link(const json::Json& r) {
        const json::Json contained_by_json = r.value(constants::Common::LINKS, json::Json::object())
            .value(constants::Common::CONTAINED_BY, json::Json());
        if (contained_by_json.is_object()) {
            if (contained_by_json
                .value(constants::Common::ODATA_ID, json::Json())
                .is_string()) {
                return true;
            }
        }
        return false;
    }


    uint64_t get_contained_by(const json::Json& r) {
        auto params = server::Multiplexer::get_instance()->get_params(
            r.value(constants::Common::LINKS, json::Json::object())
                .value(constants::Common::CONTAINED_BY, json::Json::object())
                .value(constants::Common::ODATA_ID, std::string{}
                ),
            constants::Routes::CHASSIS_PATH);
        try {
            return endpoint::utils::id_to_uint64(params[constants::PathParam::CHASSIS_ID]);
        }
        catch (const std::exception& e) {
            log_error("test", "Exception while processing ContainedBy field: "
                << r.value(constants::Common::LINKS, json::Json::object()).value(constants::Common::CONTAINED_BY, json::Json::object())
                << ": " << e.what());
            throw;
        }
    }


    std::vector<uint64_t> get_contains(const json::Json& r) {
        std::vector<uint64_t> ids;
        if (r.value(constants::Common::LINKS, json::Json::object()).count(constants::Chassis::CONTAINS)) {
            auto links = r[constants::Common::LINKS][constants::Chassis::CONTAINS];
            for (const auto& link : links) {
                auto params = server::Multiplexer::get_instance()->get_params(
                    link.value(constants::Common::ODATA_ID, std::string{}),
                    constants::Routes::CHASSIS_PATH);
                try {
                    ids.emplace_back(endpoint::utils::id_to_uint64(params[constants::PathParam::CHASSIS_ID]));
                }
                catch (const std::exception& e) {
                    log_error("test", "Exception while processing Contains field: " << link << ": " << e.what());
                    throw;
                }
            }
        }
        std::sort(ids.begin(), ids.end());

        return ids;
    }


    void test_chassis_endpoint(uint64_t id, const std::string& expected_loc_id, const std::string& expected_parent_id,
                               bool is_contained, uint64_t parent_chassis, const std::vector<uint64_t>& child_chassis) {

        log_info("test", "Testing chassis endpoint with id: " << id);
        json::Json chassis = json::Json();
        try {
            chassis = get_chassis_endpoint(id);
        }
        catch (const std::exception& e) {
            log_error("test", "Exception on endpoint GET for id" << id << ": " << e.what());
            ASSERT_TRUE(false);
        }
        EXPECT_EQ(expected_loc_id, get_location_id(chassis));
        EXPECT_EQ(expected_parent_id, get_parent_id(chassis));
        EXPECT_EQ(is_contained, has_contained_by_link(chassis));
        if (is_contained) {
            EXPECT_EQ(parent_chassis, get_contained_by(chassis));
        }
        EXPECT_EQ(child_chassis, get_contains(chassis));
    }


    static void SetUpTestCase() {
        server::Multiplexer::get_instance()->
            register_handler(Chassis::UPtr(new Chassis(constants::Routes::CHASSIS_PATH)));
    }


    void SetUp() {
        m_manager_uuid = add_manager(1);
    }


    void TearDown() {
        get_manager<agent_framework::model::Chassis>().clear_entries();
        get_manager<agent_framework::model::Manager>().clear_entries();
    }


    ~ChassisLocation();
};


ChassisLocation::~ChassisLocation() {}


TEST_F(ChassisLocation, RmmHierarchy) {
    add_chassis(1, agent_framework::model::enums::ChassisType::Rack, 0, "Rack", "");
    add_chassis(2, agent_framework::model::enums::ChassisType::Zone, 0, "Zone-1", "Rack");
    add_chassis(3, agent_framework::model::enums::ChassisType::Drawer, 0, "Drawer-1", "Rack");
    add_chassis(4, agent_framework::model::enums::ChassisType::Drawer, 1, "Drawer-2", "Rack");
    add_chassis(5, agent_framework::model::enums::ChassisType::Zone, 1, "Zone-2", "Rack");
    add_chassis(6, agent_framework::model::enums::ChassisType::Drawer, 2, "Drawer-3", "Rack");
    add_chassis(7, agent_framework::model::enums::ChassisType::Drawer, 3, "Drawer-4", "Rack");

    test_chassis_endpoint(1, "Rack", "", false, 0, {2, 3, 4, 5, 6, 7});
    test_chassis_endpoint(2, "Zone-1", "Rack", true, 1, {});
    test_chassis_endpoint(3, "Drawer-1", "Rack", true, 1, {});
    test_chassis_endpoint(4, "Drawer-2", "Rack", true, 1, {});
    test_chassis_endpoint(5, "Zone-2", "Rack", true, 1, {});
    test_chassis_endpoint(6, "Drawer-3", "Rack", true, 1, {});
    test_chassis_endpoint(7, "Drawer-4", "Rack", true, 1, {});
}


TEST_F(ChassisLocation, PncHierarchy) {
    add_chassis(1, agent_framework::model::enums::ChassisType::Enclosure, 0, {}, "");

    test_chassis_endpoint(1, "0", "", false, 0, {});
}


TEST_F(ChassisLocation, NetworkHierarchy) {
    add_chassis(1, agent_framework::model::enums::ChassisType::Module, 0, {}, "");

    test_chassis_endpoint(1, "0", "", false, 0, {});
}


TEST_F(ChassisLocation, ComputeHierarchy) {
    add_chassis(1, agent_framework::model::enums::ChassisType::Module, 0, {}, "");
    add_chassis(2, agent_framework::model::enums::ChassisType::Module, 1, {}, "");
    add_chassis(3, agent_framework::model::enums::ChassisType::Module, 2, {}, "");
    add_chassis(4, agent_framework::model::enums::ChassisType::Module, 3, {}, "");

    test_chassis_endpoint(1, "0", "", false, 0, {});
    test_chassis_endpoint(2, "1", "", false, 0, {});
    test_chassis_endpoint(3, "2", "", false, 0, {});
    test_chassis_endpoint(4, "3", "", false, 0, {});
}


TEST_F(ChassisLocation, ChassisHierarchy) {
    add_chassis(1, agent_framework::model::enums::ChassisType::Drawer, 0, {}, "");

    test_chassis_endpoint(1, "0", "", false, 0, {});
}


TEST_F(ChassisLocation, FullPsmeHierarchy) {
    // chassis
    add_chassis(1, agent_framework::model::enums::ChassisType::Drawer, 123, {}, "");
    // network
    add_chassis(2, agent_framework::model::enums::ChassisType::Module, 5, {}, "");
    // compute
    add_chassis(3, agent_framework::model::enums::ChassisType::Module, 0, {}, "");
    add_chassis(4, agent_framework::model::enums::ChassisType::Module, 1, {}, "");
    add_chassis(5, agent_framework::model::enums::ChassisType::Module, 2, {}, "");
    add_chassis(6, agent_framework::model::enums::ChassisType::Module, 3, {}, "");

    test_chassis_endpoint(1, "123", "", false, 0, {2, 3, 4, 5, 6});
    test_chassis_endpoint(2, "5", "123", true, 1, {});
    test_chassis_endpoint(3, "0", "123", true, 1, {});
    test_chassis_endpoint(4, "1", "123", true, 1, {});
    test_chassis_endpoint(5, "2", "123", true, 1, {});
    test_chassis_endpoint(6, "3", "123", true, 1, {});
}
