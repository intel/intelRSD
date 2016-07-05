SET(TARGET SNMPSubagentd)

AUX_SOURCE_DIRECTORY(. SRC_LIST)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
SET(SNMP_AGENT_NEED_LIBS librmm_init.so  librmm_cfg.so librmm_redfish.so librmm_wrap.so librmm_log.so librmm_utils.a  librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so libnetsnmpmibs.a libnetsnmpagent.a libnetsnmp.a libcrypto.a libssl.a libcurl.a librmm_jsonrpcapi.so librmm_securec.so)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_EXECUTABLE(${TARGET} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${TARGET} ${SNMP_AGENT_NEED_LIBS} libuuid.so libdl.so)
