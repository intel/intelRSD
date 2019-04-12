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
 *
 *
 * @file br_score_message_header.hpp
 *
 * @brief LLDP Bridge score message header
 * */

#pragma once

#include "message_header.hpp"

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP Bridge score message header class
 * */
class BrScoreMessageHeader final : public MessageHeader {
public:
    /*!
     * @enum Score
     * @brief LLDP Bridge score
     *
     * @var Score::NEAREST_BRIDGE
     * LLDP nearest bridge
     *
     * @var Score::NEAREST_NONTPMR_BRIDGE
     * LLDP nearest nontpmr bridge
     *
     * @var Score::NEAREST_CUSTOMER_BRIDGE
     * LLDP nearest customer bridge
     *
     * */
    enum class Score : uint8_t {
        NEAREST_BRIDGE = 0,
        NEAREST_NONTPMR_BRIDGE,
        NEAREST_CUSTOMER_BRIDGE,
    };

    /*!
     * @brief Default constructor
     *
     * @param[in] score Bridge score
     *
     * */
    BrScoreMessageHeader(Score score = Score::NEAREST_BRIDGE);

    /*!
     * @brief Parse raw message
     *
     * @param[in] raw Raw message
     *
     * */
    void parse_raw(const std::string& raw) override;

    /*!
     * @brief Get raw message
     *
     * @return Raw message
     *
     * */
    std::string get_raw() const override;

    /*!
     * @brief Get raw message size
     *
     * @return Raw message size
     *
     * */
    size_t get_raw_size() const override;

    /*! Default destructor */
    ~BrScoreMessageHeader();

private:
    Score m_score;
};

}
}
}
}
