SET(TARGET coolingctrl)
SET(TARGET_TEST test_coolingctrl)

SET(SRC_COOL main.c cooling_ctrl.c)
SET(SRC_TEST test.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)

ADD_EXECUTABLE(${TARGET} ${SRC_COOL})
TARGET_LINK_LIBRARIES(${TARGET} librmm_init.so libssl.a libcrypto.a librmm_redfish.so librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so librmm_utils.a librmm_log.so librmm_cfg.so libcurl.a librmm_securec.so)

ADD_EXECUTABLE(${TARGET_TEST} ${SRC_TEST})
TARGET_LINK_LIBRARIES(${TARGET_TEST} libssl.a libcrypto.a librmm_redfish.so librmm_jsonrpcapi.so librmm_jsonrpc.so librmm_json.so librmm_utils.a librmm_log.so libcurl.a librmm_securec.so)

