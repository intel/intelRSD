#include "psme/rest/model/handlers/root_handler.hpp"

namespace psme {
namespace rest {
namespace model {
namespace handler {
RootHandler::~RootHandler() {}
}
}
}
}
namespace agent_framework {
namespace model {
FakeRoot::~FakeRoot() {}
}
namespace command {
FakeRootInfo::~FakeRootInfo() {}
}
namespace module {

using FakeRootManager = agent_framework::module::GenericManager<agent_framework::model::FakeRoot>;

/*!
 * @brief Specialization of get_manager template function
 *
 * Will not be executed!
 */
template<>
FakeRootManager& get_manager<agent_framework::model::FakeRoot>() {
    static FakeRootManager manager{};
    return manager;
}

}
}
