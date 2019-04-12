/*!
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
 */

#include "tgt/target_parser.hpp"
#include "tgt/tokenizer.hpp"

#include <regex>



using namespace tgt;

namespace {

std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}


std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}


std::string& trim(std::string& s) {
    return ltrim(rtrim(s));
}


std::string& remove_excess(std::string& s) {
    s.erase(std::find_if(s.begin(), s.end(), std::ptr_fun<int, int>(std::isspace)), s.end());
    return s;
}
}

class TargetParser::Impl {
public:
    enum class Section {
        NONE,
        SYSTEM,
        NEXUS,
        LUN,
        ACCOUNT,
        ACL
    };


    TargetDataCollection parse(const std::string& text) {
        std::vector<std::shared_ptr<TargetData>> targets_data{};
        Tokenizer tokens{text, "\n"};
        while (tokens.next_token()) {
            const auto& token = tokens.get_token();
            if (token.empty()) {
                continue;
            }

            auto target = check_target(token);
            if (target) {
                m_section = Section::NONE;
                targets_data.emplace_back(target);
            }

            check_section(token);

            if (Section::LUN == m_section) {
                check_lun(token);
                check_lun_device_path(token);
            }

            if (Section::ACL == m_section) {
                acl_counter++;
                check_acl_initiator(token);
            }

            if (Section::ACCOUNT == m_section) {
                check_chap_data(token);
            }

        }

        TargetDataCollection targets{};
        targets.reserve(targets_data.size());
        auto dereference_transformation = [](std::shared_ptr<TargetData> target) {
            return *target;
        };

        std::transform(targets_data.begin(), targets_data.end(),
                       std::back_inserter(targets), dereference_transformation);
        return targets;
    }


    std::shared_ptr<TargetData> check_target(const std::string& text) {
        std::smatch match;
        if (std::regex_match(text, match, m_target_re)) {
            acl_counter = 0;
            TargetData target{};
            target.set_target_id(static_cast<TargetData::Id>(std::stoi(match[1].str())));
            target.set_target_iqn(match[2].str());
            m_target_data = std::make_shared<TargetData>(target);
            return m_target_data;
        }
        return nullptr;
    }


    void check_lun(const std::string& text) {
        std::smatch match{};
        if (std::regex_match(text, match, m_lun_re)) {
            auto lun_no = static_cast<LunData::Id>(std::stoi(match[1].str()));
            if (0 == lun_no) { // lun 0 controller
                m_lun_data = nullptr;
                return;
            }
            auto lun_obj = std::make_shared<LunData>();
            lun_obj->set_lun(lun_no);
            m_lun_data = lun_obj;

            if (m_target_data) {
                m_target_data->add_lun_data(lun_obj);
            }
        }

    }


    void check_lun_device_path(const std::string& text) {
        std::smatch match;
        if (std::regex_match(text, match, m_lun_device_re)) {
            if (m_lun_data) {
                m_lun_data->set_device_path(match[1].str());
            }
        }
    }


    void check_section(const std::string& text) {
        if (std::regex_match(text, m_system_section_re)) {
            m_section = Section::SYSTEM;
        }
        else if (std::regex_match(text, m_lun_section_re)) {
            m_section = Section::LUN;
        }
        else if (std::regex_match(text, m_account_section_re)) {
            m_section = Section::ACCOUNT;
        }
        else if (std::regex_match(text, m_acl_section_re)) {
            m_section = Section::ACL;
        }
    }


    void check_acl_initiator(const std::string& text) {
        if (m_target_data &&
            !std::regex_match(text, m_acl_address_re) &&
            !std::regex_match(text, m_acl_section_re) &&
            acl_counter > acl_initiator_address_section) {
            auto initiator = text;
            m_target_data->set_target_initiator(trim(initiator));
        }
    }


    void check_chap_data(const std::string& text) {
        if (m_target_data && !std::regex_match(text, m_account_section_re)) {
            if (std::regex_match(text, m_mutual_username_re)) {
                auto username = text;
                m_target_data->set_mutual_chap_username(remove_excess(trim(username)));
                m_target_data->set_authentication_method(TargetParser::MUTUAL);
            }
            else {
                auto username = text;
                m_target_data->set_chap_username(trim(username));
                m_target_data->set_authentication_method(TargetParser::ONE_WAY);
            }
        }
    }


private:
    // Storage Agent creates targets with 2 ACLs: initiator-address and optional initiator-iqn.
    // Tgtd does not  distinguish these 2 ACL types, but always shows initiator-address first.
    // Thus, not to show initiator-address as initiator-iqn, the section with initiator-address is omitted.
    short acl_counter = 0;
    const short acl_initiator_address_section = 2;
    Section m_section{Section::NONE};
    std::shared_ptr<TargetData> m_target_data{nullptr};
    std::shared_ptr<LunData> m_lun_data{nullptr};

    std::regex m_target_re{R"(.*Target\s*([\d]+):\s*(.+).*)"};
    std::regex m_lun_re{R"(.*LUN:\s*([\d]+).*)"};
    std::regex m_lun_device_re{R"(.*Backing store path:\s*(.+).*)"};
    // Regex for check IPv4 and IPv6 addresses
    std::regex m_acl_address_re{R"(.*(((([1]?\d)?\d|2[0-4]\d|25[0-5])\.){3})"
                                    R"((([1]?\d)?\d|2[0-4]\d|25[0-5]))|([\da-fA-F]{1,4})"
                                    R"("(\:[\da-fA-F]{1,4}){7})|(([\da-fA-F]{1,4}:){0,5})"
                                    R"(::([\da-fA-F]{1,4}:){0,5}[\da-fA-F]{1,4}).*)"};
    std::regex m_mutual_username_re{R"(.*\(outgoing\).*)"};
    std::regex m_system_section_re{R"(.*System information:.*)"};
    std::regex m_lun_section_re{R"(.*LUN information:.*)"};
    std::regex m_account_section_re{R"(.*Account information:.*)"};
    std::regex m_acl_section_re{R"(.*ACL information:.*)"};
};


TargetParser::TargetParser() : m_impl{new TargetParser::Impl{}} {}


TargetParser::~TargetParser() {}


const constexpr char TargetParser::ONE_WAY[];
const constexpr char TargetParser::MUTUAL[];


TargetDataCollection TargetParser::parse(const std::string& text) {
    return m_impl->parse(text);
}
