SET(TARGET_IPMI_MODULE IPMIModule)

SET(TARGET_JRPC_APP_TEST jrpc_app_test)
SET(SRC_JRPC_APP_TEST jrpc_app_test.c)

SET(SRC_APP main.c event.c util.c subscribe.c app_intf.c rmcp_intf.c rmcp_handler.c rmcp_session.c ipmi20_crypto.c serial_intf.c serial_handler.c ipmi_jrpc.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
SET(IPMI_NEED_LIBS librmm_utils.a libssl.a libcrypto.a  librmm_log.so librmm_cfg.so  libcurl.a  librmm_json.so  librmm_jsonrpc.so librmm_securec.so librmm_securec.so libdl.so)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_EXECUTABLE(${TARGET_IPMI_MODULE} ${SRC_APP})
ADD_EXECUTABLE(${TARGET_JRPC_APP_TEST} ${SRC_JRPC_APP_TEST})

TARGET_LINK_LIBRARIES(${TARGET_IPMI_MODULE}  libpthread.so librt.so ${IPMI_NEED_LIBS})
TARGET_LINK_LIBRARIES(${TARGET_JRPC_APP_TEST} ${IPMI_NEED_LIBS})
