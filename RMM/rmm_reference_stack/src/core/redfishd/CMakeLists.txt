SET(TARGET_RFEVT Redfishd)
SET(TARGET_RFTEST test_redfishd)
SET(TARGET_SSATEST test_snmp_subagentd)

SET(SRC_RFEVT main.c rf_memdb.c rf_log.c)
SET(SRC_RFTEST test_redfish.c)
SET(SRC_SSATEST test_snmp.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_EXECUTABLE(${TARGET_RFEVT} ${SRC_RFEVT})
TARGET_LINK_LIBRARIES(${TARGET_RFEVT} librmm_init.so librmm_utils.a libcurl.a libssl.a libcrypto.a librmm_jsonrpcapi.so libpthread.so librmm_wrap.so librmm_redfish.so librmm_log.so librmm_json.so librmm_cfg.so librmm_securec.so libdl.so libuuid.so)

ADD_EXECUTABLE(${TARGET_RFTEST} ${SRC_RFTEST})
TARGET_LINK_LIBRARIES(${TARGET_RFTEST} librmm_redfish.so librmm_utils.a libcurl.a libssl.a libcrypto.a librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so librmm_log.so librmm_cfg.so libdl.so)

ADD_EXECUTABLE(${TARGET_SSATEST} ${SRC_SSATEST})
TARGET_LINK_LIBRARIES(${TARGET_SSATEST} librmm_redfish.so librmm_utils.a libssl.a libcrypto.a librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so librmm_log.so librmm_cfg.so libcurl.a libdl.so)
