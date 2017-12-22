/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "psme/rest/eventing/config/subscription_config.hpp"
#include "psme/rest/eventing/event_service.hpp"
#include "psme/rest/endpoints/utils.hpp"
#include <json/json.hpp>
#include <fstream>
#include <sstream>
#include <iterator>

namespace psme {
namespace rest {
namespace eventing {
namespace config {

constexpr const char SubscriptionConfig::DEFAULT_SUBSCRIPTION_FILE_PATH[];

/*!
 * @attention Subscriptions are stored in the subscription file specified in configuration in the following form:
 * [{"id" : subscription_as_object}, ...]
 * for example:
 * [{"1" : {"Destination" : "10.3.0.2:3434", "EventTypes" : ["ResourceRemoved"] ...}},
 *  {"3" : {"Destination" : "10.3.0.3:4343", ...}}]
 */

json::Value SubscriptionConfig::get_subscriptions_json(const SubscriptionMap& subscriptions) const {
    json::Value json(json::Value::Type::ARRAY);
    for (const auto& entry : subscriptions) {
        const auto& subscription = entry.second;
        json::Value value(json::Value::Type::OBJECT);
        value[std::to_string(entry.first)] = subscription.to_json();
        json.push_back(std::move(value));
    }
    return json;
}

json::Value SubscriptionConfig::get_subscriptions_json(const std::string& json_content) const {
    json::Deserializer deserializer(json_content);
    if (deserializer.is_invalid()) {
        throw std::runtime_error("Malformed JSON: " + json_content);
    }
    json::Value json;
    deserializer >> json;
    return json;
}

void SubscriptionConfig::save() {
    log_debug(GET_LOGGER("app"), "Saving subscription config file: " << m_config_file_path);
    std::lock_guard<std::mutex> lock{m_mutex};
    std::ofstream out(m_config_file_path, std::ios::binary);
    if (out.fail()) {
        log_warning(GET_LOGGER("app"), "Unable to open config file: " << m_config_file_path);
        return;
    }
    const auto& subscriptions = SubscriptionManager::get_instance()->get();
    const auto& subs_json = get_subscriptions_json(subscriptions);
    json::Serializer serializer(subs_json);
    std::string json_content;
    json_content << serializer;
    out << json_content;
    out.close();
}

void SubscriptionConfig::load() {
    log_debug(GET_LOGGER("app"), "Reading subscription config file: " << m_config_file_path);
    std::ifstream in(m_config_file_path);
    if (in.fail()) {
        log_warning(GET_LOGGER("app"), "Unable to open config file: " << m_config_file_path);
        return;
    }
    std::stringstream buffer;
    buffer << in.rdbuf();
    in.close();
    const auto& content = buffer.str();

    if (content.empty()) {
        log_warning(GET_LOGGER("app"), "Empty subscription configuration file: " << m_config_file_path);
        return;
    }
    try {
        std::ostringstream info_msg{};
        info_msg << "Loaded subscriptions:\n";
        const auto& subscriptions_json = get_subscriptions_json(content);
        if (subscriptions_json.is_array()) {
            SubscriptionMap subscriptions;
            for (const auto& subscription_json : subscriptions_json.as_array()) {
                auto number_of_keys = std::distance(subscription_json.begin(), subscription_json.end());
                if (subscription_json.begin().is_array() ||
                    number_of_keys == 0 ||
                    number_of_keys > 1) {

                    log_warning(GET_LOGGER("app"), "Invalid subscription object in subscriptions config file.");
                    continue;
                }
                std::string key_as_string = subscription_json.begin().key();
                uint64_t key = psme::rest::endpoint::utils::id_to_uint64(key_as_string);
                info_msg << " \"" << key_as_string << "\" : " << subscription_json[key_as_string] << "\n";
                Subscription subscription = Subscription::from_json(subscription_json[key_as_string], false);
                subscription.set_id(key);
                subscriptions.emplace(key, subscription);
            }
            SubscriptionManager::get_instance()->set_subscriptions(subscriptions);
            log_info("app", info_msg.str());
        }
    }
    catch (const std::exception& ex) {
        log_warning(GET_LOGGER("app"), "Unable to add subscription: " << ex.what());
    }
}

void SubscriptionConfig::set_config_file(const std::string& config_file_path) {
    m_config_file_path = config_file_path;
}

SubscriptionConfig::~SubscriptionConfig() {
}

}
}
}
}
