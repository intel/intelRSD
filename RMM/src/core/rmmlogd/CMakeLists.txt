SET(TARGET_RMM_LOGD RMMLogd)
SET(TARGET_TEST test_rmmlogd)

SET(SRC_EVT_LOG main.c log_manager.c)
SET(SRC_TEST test.c log_manager.c )

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)

ADD_EXECUTABLE(${TARGET_RMM_LOGD} ${SRC_EVT_LOG})
TARGET_LINK_LIBRARIES(${TARGET_RMM_LOGD} librmm_utils.a libpthread.so librmm_json.so librmm_cfg.so librmm_securec.so libcurl.a libssl.a libcrypto.a libdl.so)

ADD_EXECUTABLE(${TARGET_TEST} ${SRC_TEST})
TARGET_LINK_LIBRARIES(${TARGET_TEST} librmm_utils.a libpthread.so librmm_json.so librmm_cfg.so librmm_securec.so libcurl.a libssl.a libcrypto.a libdl.so)
