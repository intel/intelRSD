/*!
 * @brief Location class implementation
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file location.cpp
 * */




#include "psme/rest/registries/model/attributes/location.hpp"

#include "json-wrapper/json-wrapper.hpp"



using namespace psme::rest::registries;


void Location::set_language(const OptionalField<std::string>& language) {
    m_language = language;
}


const OptionalField<std::string>& Location::get_language() const {
    return m_language;
}


void Location::set_uri(const OptionalField<std::string>& uri) {
    m_uri = uri;
}


const OptionalField<std::string>& Location::get_uri() const {
    return m_uri;
}


void Location::set_archive_uri(const OptionalField<std::string>& archive_uri) {
    m_archive_uri = archive_uri;
}


const OptionalField<std::string>& Location::get_archive_uri() const {
    return m_archive_uri;
}


void Location::set_publication_uri(const OptionalField<std::string>& publication_uri) {
    m_publication_uri = publication_uri;
}


const OptionalField<std::string>& Location::get_publication_uri() const {
    return m_publication_uri;
}


void Location::set_archive_file(const OptionalField<std::string>& archive_file) {
    m_archive_file = archive_file;
}


const OptionalField<std::string>& Location::get_archive_file() const {
    return m_archive_file;
}


Location Location::from_json(const json::Json& json) {
    Location location{};

    location.set_language(json["Language"]);
    location.set_uri(json["Uri"]);
    location.set_archive_uri(json["ArchiveUri"]);
    location.set_publication_uri(json["PublicationUri"]);
    location.set_archive_file(json["ArchiveFile"]);

    return location;
}
