/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "psme/rest-ref/eventing/config/subscription_config.hpp"
#include "psme/rest-ref/eventing/event_service.hpp"
#include <json/json.hpp>
#include <fstream>

namespace psme {
namespace rest {
namespace eventing {
namespace config {

constexpr const char SubscriptionConfig::DEFAULT_SUBSCRIPTION_FILE_PATH[];

json::Value SubscriptionConfig::get_subscriptions_json(const SubscriptionMap& subscriptions) const {
    json::Value json(json::Value::Type::ARRAY);
    for (const auto& entry : subscriptions) {
        const auto& subscription = entry.second;
        json.push_back(subscription.to_json());
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
        const auto& subscriptions_json = get_subscriptions_json(content);
        if (subscriptions_json.is_array()) {
            SubscriptionManager::get_instance();
            for (const auto& subscription_json : subscriptions_json.as_array()) {
                const auto subscription = Subscription::from_json(subscription_json);
                SubscriptionManager::get_instance()->add(subscription);
            }
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
