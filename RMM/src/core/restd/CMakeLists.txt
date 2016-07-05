SET(TARGET Restd)

SET(SRC_LIST main.c http.c rest.c websocket.c handler/event_handler.c handler/manager_handler.c handler/rack_handler.c handler/mzone_handler.c handler/dzone_handler.c handler/pzone_handler.c handler/tzone_handler.c handler/general_handler.c)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)

ADD_EXECUTABLE(${TARGET} ${SRC_LIST})
TARGET_LINK_LIBRARIES(${TARGET} librmm_init.so librmm_json.so librmm_jsonrpcapi.so libpthread.so librmm_utils.a libssl.a libcrypto.a librmm_wrap.so librmm_redfish.so librmm_log.so librmm_cfg.so libcurl.a librmm_securec.so libdl.so libuuid.a)

INSTALL(
  DIRECTORY web
  DESTINATION ${PROJECT_BINARY_DIR}/bin
  )
