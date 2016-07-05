set (UTILS_SRC_DIR  ${PROJECT_SOURCE_DIR}/utils)
set (UTILS_BIN_DIR  ${PROJECT_BINARY_DIR}/utils)

execute_process(
	COMMAND mkdir -p ${UTILS_BIN_DIR}
	COMMAND mkdir -p ${UTILS_BIN_DIR}/redfish
	COMMAND mkdir -p ${UTILS_BIN_DIR}/cfg
	COMMAND mkdir -p ${UTILS_BIN_DIR}/encrypt_text
	COMMAND mkdir -p ${UTILS_BIN_DIR}/scripts
	COMMAND mkdir -p ${UTILS_BIN_DIR}/tools
	COMMAND cp ${UTILS_SRC_DIR}/redfish/MR.json             ${UTILS_BIN_DIR}/redfish/
	COMMAND	cp ${UTILS_SRC_DIR}/cfg/rmm.cfg                 ${UTILS_BIN_DIR}/cfg/
	COMMAND cp ${UTILS_SRC_DIR}/cfg/pwm_rpm.cfg             ${UTILS_BIN_DIR}/cfg/
	COMMAND cp ${UTILS_SRC_DIR}/cfg/80-persistent-tty.rules ${UTILS_BIN_DIR}/cfg/
	COMMAND cp ${UTILS_SRC_DIR}/encrypt_text/keyfile        ${UTILS_BIN_DIR}/encrypt_text/
	COMMAND cp ${UTILS_SRC_DIR}/scripts/cm_dbclean.sh       ${UTILS_BIN_DIR}/scripts/
	COMMAND cp ${UTILS_SRC_DIR}/scripts/cm_restart.sh       ${UTILS_BIN_DIR}/scripts/
	COMMAND cp ${UTILS_SRC_DIR}/scripts/cm_stop.sh          ${UTILS_BIN_DIR}/scripts/
	COMMAND cp ${UTILS_SRC_DIR}/scripts/cm_update.sh        ${UTILS_BIN_DIR}/scripts/
	COMMAND cp ${UTILS_SRC_DIR}/tools/set_puid.py           ${UTILS_BIN_DIR}/tools/
)

if (ENCRYPT_USERNAME_PASSWD)
	execute_process(COMMAND ${UTILS_SRC_DIR}/cfg/encrypt.sh ${UTILS_BIN_DIR}/cfg/rmm.cfg)
endif()
