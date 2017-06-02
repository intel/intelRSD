/**
 * Copyright (c)  2015-2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <pthread.h>

#include "handler.h"
#include "libcfg/cfg.h"
#include "libredfish/msg_reg.h"
#include "libutils/time.h"
#include "libutils/ip.h"
#include "libutils/base64.h"

typedef struct rack_update_info {
	uint32 upgrade_seq;
	uint8 deb_name[MAX_DEB_NAME_LEN];
	uint8 deb_path[MAX_DEB_PATH_LEN];
} rack_update_info_t;

static struct rest_handler manager_coll_handler;
static struct rest_handler manager_rmc_handler;
static struct rest_handler manager_mbp_handler;
static struct rest_handler manager_network_srv_handler;
static struct rest_handler manager_eth_inf_handler;
static struct rest_handler manager_eth_inf_coll_handler;
static struct rest_handler manager_vlans_handler;
static struct rest_handler manager_vlan_handler;
static struct rest_handler manager_reset_handler;
static struct rest_handler rack_update_handler;
static struct rest_handler rack_default_handler;
static struct rest_handler rack_reset_handler;
static struct rest_handler mbpc_reset_handler;

static rack_update_info_t rack_update;

#define PUB_KEY_LEN     (2048)
#define PUB_KEY_PATH    "/etc/stunnel/key.pub"


void register_manager_handler(void)
{
	register_handler(MANAGER_COLL_URL, &manager_coll_handler);
	register_handler(MANAGER_RMC_URL, &manager_rmc_handler);
	register_handler(MANAGER_MBPC1_URL, &manager_mbp_handler);
	register_handler(MANAGER_MBPC2_URL, &manager_mbp_handler);
	register_handler(MANAGER_NETWORK_SERVICE_URL, &manager_network_srv_handler);
	register_handler(MANAGER_ETHERNET_INF_COLL_URL, &manager_eth_inf_coll_handler);
	register_handler(MANAGER_ETHERNET_INF_URL, &manager_eth_inf_handler);
	register_handler(MANAGER_VLANS_URL, &manager_vlans_handler);
	register_handler(MANAGER_VLAN_URL, &manager_vlan_handler);

	register_handler(MANAGER_ACTION_RESET_URL,  &rack_reset_handler);
	register_handler(MBPC1_ACTION_RESET_URL,    &mbpc_reset_handler);
	register_handler(MBPC2_ACTION_RESET_URL,    &mbpc_reset_handler);
	register_handler(RACK_ACTION_UPDATE_URL,    &rack_update_handler);
	register_handler(RACK_ACTION_DEFAULT_URL,   &rack_default_handler);
}

static json_t *manager_coll_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}

	rs = libwrap_pack_manager_coll_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack manager collection fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *manager_rmc_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}

	rs = libwrap_pack_rmc_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack manager fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}


static json_t *manager_mbp_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		HTTPD_ERR("request json object fail\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}

	rs = libwrap_pack_mbpc_to_json(result, param);
	if (rs != RESULT_OK) {
		HTTPD_ERR("pack manager fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static result_t reset_manager(struct rest_uri_param *param)
{
	pid_t pid = fork();

	if (0 == pid) {
		close(param->fd);
		sleep(2);
		HTTPD_INFO("RMM Reset\n");
		if (system("reboot &") < 0)
			exit(0);
	} else {
		sleep(1);
		return RESULT_OK;
	}

	return RESULT_OK;
}

static json_t *manager_reset_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	json_t *result = NULL;

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json_object fail\n");
		return NULL;
	}

	if ((param->json_data == NULL) || (0 == strnlen_s(param->json_data, RSIZE_MAX_STR))) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json_data is NULL or empty\n");
		goto err;
	}

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("parse json error\n");
		goto err;
	}

	if (reset_manager(param) != RESULT_OK)
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
	else
		update_response_info(param, HTTP_OK);

	return NULL;

err:
	if (req)
		json_free(req);
	if (result)
		json_free(result);

	return NULL;
}

static json_t *manager_network_srv_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	mngr_network_srv_t network_srv = {};

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}

	if (libwrap_manager_network_srv_init(&network_srv) != 0){
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("manager network interfaceinfo init fail\n");
		return NULL;
	}

	rs = libwrap_pack_manager_network_srv_json(result, &network_srv);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack manager fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *manager_eth_inf_coll_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	mngr_eth_inf_coll_t eth_coll_inf = {};

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}

	if (libwrap_manager_eth_inf_coll_init(&eth_coll_inf) != 0){
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("manager ethernet interface collection info init fail\n");
		return NULL;
	}

	rs = libwrap_pack_manager_eth_inf_coll_json(result, &eth_coll_inf);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("pack manager interface collection fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *manager_eth_inf_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	mngr_eth_inf_t eth_inf = {};
	int8 ifname[32];
	int32 inf_id = 0;

	inf_id = get_asset_idx(param, "inf_id", MC_TYPE_V1);
	if (inf_id == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get ethernet interface index fail\n");
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}

	bzero(ifname, sizeof(ifname));

	if (libwrap_manager_eth_inf_init(&eth_inf, inf_id) != 0){
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("manager ethernet interfaceinfo init fail\n");
		return NULL;
	}

	rs = libwrap_pack_manager_eth_inf_json(result, &eth_inf);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack ethernet interface fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *manager_vlans_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	mngr_vlans_t vlans = {};
	int8 ifname[32];
	int32 inf_id = 0;

	inf_id = get_asset_idx(param, "inf_id", MC_TYPE_V1);
	if (inf_id == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get ethernet interface index fail\n");
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}
	if (libwrap_manager_vlans_init(&vlans, inf_id) != 0){
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("manager vlans init fail\n");
		return NULL;
	}

	rs = libwrap_pack_manager_vlans_json(result, &vlans);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack vlans fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *manager_vlan_get(struct rest_uri_param *param)
{
	result_t rs = RESULT_OK;
	json_t *result = NULL;
	mngr_vlan_t vlan = {};
	int8 ifname[32];
	int32 inf_idx = 0;
	int32 vlan_idx = 0;
	int32 url_len = 0;

	inf_idx = get_asset_idx(param, "inf_id", MC_TYPE_V1);
	if (inf_idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get ethernet interface index fail\n");
		return NULL;
	}

	vlan_idx = get_asset_idx(param, "vlan_id", MC_TYPE_V1);
	if (vlan_idx == -1) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("get vlan index fail\n");
		return NULL;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("request json object fail\n");
		return NULL;
	}
	if (libwrap_manager_vlan_init(&vlan, inf_idx, vlan_idx) != 0){
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		HTTPD_ERR("manager vlans init fail\n");
		return NULL;
	}

	rs = libwrap_pack_manager_vlan_json(result, &vlan);
	if (rs != RESULT_OK) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("pack vlans fail, result is %d\n", rs);
		return NULL;
	}

	update_response_info(param, HTTP_OK);
	return result;
}

static json_t *json_object_result(struct json_result *result)
{
	json_t *obj;

	obj = json_object();
	if (obj == NULL) {
		HTTPD_ERR("request json object error\n");
		return NULL;
	}

	for (; result->name != NULL; result++)
		json_object_add(obj, result->name, result->json);

	return obj;
}

static int32 process_reset_rack(uint8 option)
{
	int32 fd = -1;
	int32 rc = -1;
	struct sockaddr_in addr;
	int32 port = rmm_cfg_get_port(UPGRADED_PORT);
	if (port == 0) {
		HTTPD_ERR("Failed to call rmm_cfg_get_upgrade_monitor_port\n");
		exit(-1);
	}

	int32 len = sizeof(struct sockaddr_in);
	struct _upgrade_monitor req = {0};
	int32 try_cnt = 0;

	if (SERVICE_RESET_PREPARE == option)
		req.sync = SERVICE_RESET_PREPARE;
	else if (FACTORY_DEFAULT_PREPARE == option)
		req.sync = FACTORY_DEFAULT_PREPARE;
	else {
		HTTPD_ERR("option not supported\n");
		return -1;
	}
	fd = udp_create();
	if (fd < 0) {
		HTTPD_ERR("upgrade thread create failed\n");
		return -1;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	while (try_cnt < 3) {
		try_cnt++;

		rc = sendto(fd, (int8 *)&req, sizeof(req), 0,
					(struct sockaddr *)&addr, len);
		if (rc != sizeof(req))
			continue;

		break;
	}

	close(fd);

	if (try_cnt >= 3) {
		HTTPD_ERR("try count over 3\n");
		return -1;
	}
	return 0;
}

static result_t reset_rack(int32 reset_mode, struct rest_uri_param *param)
{
	pid_t pid;

	HTTPD_DEBUG("reset mode:%s\n", (0 == reset_mode) ? RMM_JSON_RESET_MODE_SERVICE:RMM_JSON_RESET_MODE_RMM);

	pid = fork();
	if (0 == pid) {
		close(param->fd);
		sleep(2);
		if (0 == reset_mode)
			process_reset_rack(SERVICE_RESET_PREPARE);
		else if (1 == reset_mode) {
			HTTPD_INFO("RMM Reset\n");
			if (system("reboot &") < 0)
				exit(0);
		}
		exit(0);
	} else {
		sleep(1);
		return RESULT_OK;
	}
}

static void *process_upgrade_thread(void *arg)
{
	int32 fd = -1;
	int32 rc = -1;
	struct sockaddr_in addr;
	struct sockaddr_in remote_addr;
	int32 remote_len = sizeof(struct sockaddr_in);
	int32 port = rmm_cfg_get_port(UPGRADED_PORT);
	if (port == 0) {
		HTTPD_ERR("Failed to call rmm_cfg_get_upgrade_monitor_port\n");
		exit(-1);
	}

	int32 len = sizeof(struct sockaddr_in);
	struct _upgrade_monitor req = {0};
	struct _upgrade_monitor resp = {0};
	fd_set fds;

	fd = udp_create();
	if (fd < 0) {
		HTTPD_ERR("upgrade thread create failed\n");
		return NULL;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	while (1) {
		/* Step 1: notify upgrade monitor to prepare do upgradation */
		req.sync = UPGRADE_PREPARE;
		req.seq = rack_update.upgrade_seq++;
		memcpy_s(req.deb_name, sizeof(req.deb_name), rack_update.deb_name, MAX_DEB_NAME_LEN);
		memcpy_s(req.deb_path, sizeof(req.deb_path), rack_update.deb_path, MAX_DEB_PATH_LEN);

		rc = sendto(fd, (int8 *)&req, sizeof(req), 0,
					(struct sockaddr *)&addr, len);
		if (rc != sizeof(req))
			continue;

		/* Step 2: wait for the response from the upgrade monitor,
		 * to make sure the message just be send has been received by upgrade
		 * monitor. */
		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		rc = select(fd + 1, &fds, NULL, NULL, NULL);
		if (rc < 0)
			continue;

		rc = recvfrom(fd, (int8 *)&resp, sizeof(resp), 0,
					  (struct sockaddr *)&remote_addr,
					  (socklen_t *)&remote_len);
		if (rc != sizeof(resp))
			continue;

		if ((resp.sync == UPGRADE_PREPARE_ACK) && (resp.seq == rack_update.upgrade_seq))
			++rack_update.upgrade_seq;
		else
			continue;

		/* Step 3: send the confirmation to the upgrade monitor. */
		req.sync = UPGRADE_CONFIRM;
		req.seq = rack_update.upgrade_seq++;
		while (1) {
			rc = sendto(fd, (int8 *)&req, sizeof(req), 0,
						(struct sockaddr *)&addr, len);
			if (rc != sizeof(req))
				continue;

			close(fd);
			return NULL;
		}
	}

	close(fd);
	return NULL;
}

static void sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH], unsigned char outputBuffer[65])
{
	int i = 0;

	for(i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf((char*)outputBuffer + (i * 2), "%02x", hash[i]);
	}

	outputBuffer[64] = 0;
}

static int calc_sha256 (FILE *file, unsigned char output[65])
{
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	const int bufSize = 32768;
	int bytesRead = 0;

	unsigned char* buffer = malloc(bufSize);
	if(!buffer) return -1;

	while((bytesRead = fread(buffer, 1, bufSize, file))) {
		SHA256_Update(&sha256, buffer, bytesRead);
	}
	SHA256_Final(hash, &sha256);

	sha256_hash_string(hash, output);
	free(buffer);
	return 0;
}

static int signature_verify(const char *signed_file)
{

	unsigned char signature[256];
	unsigned int slen = 256;
	unsigned int verified;
	unsigned char calc_hash[65] = {0};
	RSA *public_key;
	BIO *pub_bio;
	char pub_key[PUB_KEY_LEN + 1]; // create buffer for public key +1 for NULL delimiter.

	// Fills buffer with 0. To work with OpenSSL BIO_new_mem_buf array must be NULL terminated.
	memset(pub_key, 0, sizeof(pub_key));

	FILE *key_file = fopen(PUB_KEY_PATH, "r");
	if (key_file == NULL) {
		return 0;
	}

	// Read the whole key.
	fread(pub_key, PUB_KEY_LEN, 1, key_file);
	fclose(key_file);

	// -1 means that pub_key must be NULL terminated. We ensure that by filling buffer with 0s.
	pub_bio = BIO_new_mem_buf(pub_key, -1);
	if(pub_bio == NULL) {
		return 0;
	}

	public_key = PEM_read_bio_RSA_PUBKEY(pub_bio, NULL, NULL, NULL);

	// PEM_read_bio_RSA_PUBKEY allocates new memory. We can free pub_bio.
	BIO_free(pub_bio);

	if(public_key == NULL) {
		return 0;
	}

	FILE *file = fopen(signed_file, "r");
	if (file == NULL) {
		RSA_free(public_key);
		return 0;
	}

	fread(signature, 1, 256, file);
	calc_sha256(file, calc_hash);
	fclose(file);

	verified = RSA_verify(NID_sha256, (unsigned char*) calc_hash,
			strnlen_s((char*)calc_hash, sizeof(calc_hash)), signature, slen, public_key);

	RSA_free(public_key);

	return verified;
}

static int save_signed_image(const char *signed_file, int8 *buff, int32 length)
{
	int rc = 0;
	int8 *w_buf = NULL;
	int32 fd = -1;
	int32 offset = 0;
    int32 len = 0;
    int32 decode_size = 0;

	fd = open(signed_file, O_RDWR | O_CREAT, S_IRWXU);
	if (fd < 0) {
		HTTPD_ERR("fail to create signed image file\n");
		return -1;
	}

	while (offset < length) {
		/* Decode the file first. */
		decode_size = ((offset + MAX_READ_BASE64_SIZE)
				< length ? MAX_READ_BASE64_SIZE
				: (length - offset));

		w_buf = base64_decode(buff + offset, decode_size, &len);
		if (NULL == w_buf) {
			HTTPD_ERR("w_buff is null\n");
			rc = -1;
			goto out;
		}

		offset += decode_size;

		rc = write(fd, w_buf, len);
		if (rc < 0) {
			HTTPD_ERR("write error\n");
			rc = -1;
			goto out;
		}
		free(w_buf);
		w_buf = NULL;
	}

out:
	if (w_buf)
		free(w_buf);
	if (fd >= 0)
		close(fd);

	return rc;
}

static int save_unsigned_image(const char *signed_file, const char *unsigned_file)
{
	int rc = 0;
	int r_fd = -1;
	int w_fd = -1;
	unsigned char buff[256];
    int len;

	r_fd = open(signed_file, O_RDONLY);
	if (r_fd < 0) {
		rc = -1; goto out;
	}

	w_fd = open(unsigned_file, O_RDWR | O_CREAT, S_IRWXU);
	if (w_fd < 0) {
		rc = -1; goto out;
	}
	if (lseek(r_fd, 256, SEEK_SET) < 0) {
		rc = -1; goto out;
	}

	while((len = read(r_fd, buff, sizeof(buff)))>0)
		write(w_fd, buff, len);

out:
	if (r_fd >= 0)
		close(r_fd);
	if (w_fd >= 0)
		close(w_fd);
	return rc;
}

static result_t update_rack(int8 *comp_type, int8 *buff,
							 int64 length, int8 *version, uint32 version_len)
{
	int32 rc = RESULT_OTHER_ERR;
	int32 status = -1;
	int8 cmd[128] = {0};
	int32 len = 0;

	FILE *pp = NULL;
	int8 *sys_path = "export PATH=/sbin/:/bin/:/usr/sbin:/usr/bin";
	int8 output[256] = {0};
	int8 out_name[256] = {0};
	int8 out_ver[256] = {0};
	int32 ver_digital_cur[4] = {0};
	int32 ver_digital_new[4] = {0};
	int32 i = 0;

	struct stat sb;
	int8 *upgrade_path = "/tmp/tmp_rsa_rmm_upgrade_path";

	/* Sanity check: input parameters are ok */
	if ((length <= 0) || (comp_type == NULL) || (buff == NULL)) {
		HTTPD_ERR("input param error\n");
		return rc;
	}

	/* Sanity check: has path write permission */
	if (stat(upgrade_path, &sb) == -1) {
		if (errno == ENOENT)
			mkdir(upgrade_path, 0777);
		else {
			HTTPD_ERR("upgrade path error\n");
			return rc;
		}
	}

	if (save_signed_image(UPGRADE_FILE_SIGNED, buff, length) == -1) {
		HTTPD_ERR("Fail to save signed image file\n");
		goto err;
	}

	if (!signature_verify(UPGRADE_FILE_SIGNED)) {
		rc = RESULT_SIGNATURE_ERR;
		goto err;
	}

	if (save_unsigned_image(UPGRADE_FILE_SIGNED, UPGRADE_FILE_TARBALL) == -1) {
		HTTPD_ERR("Fail to save unsigned image file\n");
		goto err;
	}

	snprintf_s_s(cmd, sizeof(cmd), "rm -rf %s/*", upgrade_path);
	if (system(cmd) < 0) {
		HTTPD_ERR("remove upgrade path error\n");
		rc = RESULT_OTHER_ERR;
		goto err;
	}

	memset(cmd, 0, sizeof(cmd));
	snprintf_s_ss(cmd, sizeof(cmd), "tar -zxf %s -C %s/", UPGRADE_FILE_TARBALL, upgrade_path);
	status = system(cmd);
	if (status < 0) {
		HTTPD_ERR("decompress error\n");
		rc = RESULT_OTHER_ERR;
		goto err;
	}

	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != 0) {
			HTTPD_ERR("status error\n");
			rc = RESULT_OTHER_ERR;
			goto err;
		}
	} else {
		HTTPD_ERR("unknown status\n");
		rc = RESULT_OTHER_ERR;
		goto err;
	}

	snprintf_s_sss(cmd, sizeof(cmd), "%s && dpkg-deb -W %s/%s.deb\n", sys_path, upgrade_path, comp_type);
	printf("cmd %s\n", cmd);
	pp = popen(cmd, "r");
	if (pp == NULL) {
		HTTPD_ERR("open package error\n");
		rc = RESULT_OTHER_ERR;
		goto err;
	}
	while (fgets(output, 256, pp) != NULL) {
		rc = sscanf(output, "%s%s", out_name, out_ver);
		if (rc == 2) {
			if (strcmp(out_name, comp_type) == 0) {
				/* Compare the version */
				rc = sscanf(out_ver, "%d.%d.%d.%d", &ver_digital_new[0],
							&ver_digital_new[1], &ver_digital_new[2],
							&ver_digital_new[3]);
				if (rc < 3) {
					HTTPD_ERR("version mismatch\n");
					rc = RESULT_UNMATCHED_VERSION;
					goto err;
				}
				rc = sscanf(RSA_RMM_FIRMWARE_VERSION, "%d.%d.%d.%d",
							&ver_digital_cur[0],
							&ver_digital_cur[1],
							&ver_digital_cur[2],
							&ver_digital_cur[3]);
				if (rc < 3) {
					HTTPD_ERR("RackSale FW Version get error\n");
					rc = RESULT_OTHER_ERR;
					goto err;
				}
				HTTPD_DEBUG("%d.%d.%d.%d::%d.%d.%d.%d\n", ver_digital_cur[0],
						   ver_digital_cur[1], ver_digital_cur[2],
						   ver_digital_cur[3],
						   ver_digital_new[0], ver_digital_new[1],
						   ver_digital_new[2], ver_digital_new[3]);

				for (i = 0; i < 4; i++) {
					if ((ver_digital_cur[i] < ver_digital_new[i])
						|| (ver_digital_cur[i] > ver_digital_new[i]))
						break;

					if (i >= 3) {
						rc = RESULT_SAME_VERSION;
						goto err;
					}
				}

				snprintf_s_s(version, version_len, "rev %s", out_ver);
				break;
			}
		}
	}

	pclose(pp);

	memcpy_s(rack_update.deb_name, sizeof(rack_update.deb_name), comp_type, strnlen_s(comp_type, RSIZE_MAX_STR));
	len = (MAX_DEB_PATH_LEN > strnlen_s(upgrade_path, RSIZE_MAX_STR) ? strnlen_s(upgrade_path, RSIZE_MAX_STR) : MAX_DEB_PATH_LEN);
	memcpy_s(rack_update.deb_path, sizeof(rack_update.deb_path), upgrade_path, len);

	unlink(UPGRADE_FILE_SIGNED);
	unlink(UPGRADE_FILE_TARBALL);

	printf("libwrap_update_rack image[%s] len[%lld]\n", comp_type, length);
	return RESULT_OK;

err:
	if (pp) {
		pclose(pp);
	}
	unlink(UPGRADE_FILE_SIGNED);
	unlink(UPGRADE_FILE_TARBALL);

	return rc;
}

static result_t set_factory_rack(void)
{
	int32 rc = -1;
#if 0
	
	int8 cmd[128] = {0};

	if (system("/bin/rm /var/Memdb/memdb_upgrade.snapshot"))
		return rc;
	if (system("/bin/rm /var/Memdb/memdb_rmm_update.log"))
		return rc;
	snprintf(cmd, sizeof(cmd), "touch %s", FACTORY_DEFAULT_FLAG);
	if (system(cmd))
		return rc;

	rc = process_reset_rack(FACTORY_DEFAULT_PREPARE);
#endif
	libwrap_rmm_factory_reset(&rc);

	return rc;
}


static json_t *rack_process_reset(json_t *result, json_t *req, struct rest_uri_param *param)
{
	if (req)
		json_free(req);

	if (RESULT_OK == reset_rack(0, param)) {	/* only support service reset */
		HTTPD_INFO("rack reset success\n");
		update_response_info(param, HTTP_NO_CONTENT);
		rf_snmp_evt(INFO, MSGRackStatusChange, RMM_JSON_RESETED_BIG);

		return result;

	} else {
		HTTPD_ERR("rack reset fail\n");
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
}

static json_t *rack_handle_reset(json_t *req, json_t *result, struct rest_uri_param *param)
{
	#if 0
	json_t *rs_json = NULL;
	int8 *cap = NULL;

	rs_json = json_object_get(req, RMM_JSON_RESET_TYPE);
	if (rs_json == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json object request error.\n");
		json_free(req);
		json_free(result);
		return NULL;
	}
	cap = json_string_value(rs_json);
	if (cap == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("failed to json string.\n");
		return NULL;
	}
	#endif
	return rack_process_reset(result, req, param);
}

static json_t *rack_handle_update(pthread_t *tid, json_t *req, json_t *result, struct rest_uri_param *param)
{
	json_t *rs_json = NULL;
	int8 *cap1 = NULL;
	int8 *cap2 = NULL;
	int32 rc = -1;
	int64 len = 0;
	int8 image_ver[256] = {0};

	struct json_result response[] = {
		{RMM_JSON_RESULT, NULL},
		{"new image rev", NULL},
		{NULL, NULL}
	};

	rs_json = json_object_get(req, RMM_JSON_COMPONENT_TYPE);
	if (rs_json == NULL) {
		HTTPD_ERR("no ComponentType field!\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	} else
		cap1 = json_string_value(rs_json);

	if (cap1 == NULL || strncmp("rmm-base", cap1, strnlen_s("rmm-base", RSIZE_MAX_STR)) != 0) {
		HTTPD_ERR("failed to get json string <componentType>!\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	rs_json = json_object_get(req, RMM_JSON_IMAGE);
	if (rs_json == NULL) {
		HTTPD_ERR("get image faild!\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	cap2 = json_string_value(rs_json);
	if (cap2 == NULL) {
		HTTPD_ERR("failed to get json string <image>!\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	len = strnlen_s(cap2, RSIZE_MAX_STR);
	/* start to do the update, need to have a solution to receive the image */
	rc = update_rack(cap1, cap2, len, image_ver, 256);
	if (rc == RESULT_OK) {
		json_free(req);
		json_free(result);

		/* Create a new thread to notify upgrade_monitor to do
		* the upgradation. */
		rc = pthread_create(tid, NULL, process_upgrade_thread, NULL);
		if (rc < 0) {
			HTTPD_ERR("upgrade thread create failed\n");
			return NULL;
		}
	}

	update_response_info(param, HTTP_ACCEPTED);
	response[0].json = json_string(RMM_JSON_UPDATING);
	response[1].json = json_string(image_ver);

	rf_snmp_evt(INFO, MSGRackStatusChange, "Update firmware");

	libwrap_set_rmm_update_status(rc);
	rc = libwrap_update_rack_mtime();
	if (rc != RESULT_OK) {
		rmm_log(ERROR, "memdb set rack update date fail:%d\n", rc);
	}
#ifdef REST_HTTPS
	send_json_reply_redirect(param->fd, param->https_url);
#else
	send_json_reply_redirect(param->fd, param->url);
#endif
	return json_object_result(response);

err:
	json_free(req);
	json_free(result);
	return NULL;

}

static json_t *rack_handle_default(json_t *req, json_t *result, struct rest_uri_param *param)
{
	if (req)
		json_free(req);

	if (set_factory_rack() == RESULT_OK) {
		HTTPD_INFO("factory default success\n");
		update_response_info(param, HTTP_OK);
		rf_snmp_evt(INFO, MSGRackStatusChange, "factory default");

		json_object_add(result, RMM_JSON_RESULT, json_string("Success"));
		return result;
	} else {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("processing factory default error\n");
		return NULL;
	}
}

static input_attr_t rack_reset_attrs[] = {
};

static json_t *rack_reset_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	json_t *result = NULL;
	pthread_t tid;
	uint32 ary_size = sizeof(rack_reset_attrs)/sizeof(input_attr_t);

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json_object fail\n");
		return NULL;
	}

	if ((param->json_data == NULL) || (0 == strnlen_s(param->json_data, RSIZE_MAX_STR))) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json_data is NULL or empty\n");
		goto err;
	}

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("parse json error\n");
		goto err;
	}

	if (libwrap_check_input_attrs(rack_reset_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	return rack_handle_reset(req, result, param);

err:
	if (req)
		json_free(req);
	if (result)
		json_free(result);

	return NULL;
}

static json_t *rack_update_get(struct rest_uri_param *param)
{
	json_t *result = NULL;
	int32 rc = libwrap_get_rmm_update_status();

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("result json object request fail\n");
		return NULL;
	}

	if (rc == RESULT_OK)
		json_object_add(result, RMM_JSON_STATUS, json_string(RMM_JSON_UPDATED));
	else if (rc == RESULT_UNMATCHED_VERSION)
		json_object_add(result, RMM_JSON_STATUS, json_string("unmatched version"));
	else if (rc == RESULT_SAME_VERSION)
		json_object_add(result, RMM_JSON_STATUS, json_string("same version"));
	else if (rc == RESULT_SIGNATURE_ERR)
		json_object_add(result, RMM_JSON_STATUS, json_string("wrong signature"));
	else {
		json_object_add(result, RMM_JSON_STATUS, json_string("unknown error"));
	}

	update_response_info(param, HTTP_OK);

	return result;
}

static input_attr_t rack_update_attrs[] = {
    {"ComponentType", NULL},
    {"Image", NULL}
};

static json_t *rack_update_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	json_t *result = NULL;
	pthread_t tid;
	uint32 ary_size = sizeof(rack_update_attrs)/sizeof(input_attr_t);

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json_object fail\n");
		return NULL;
	}

	if ((param->json_data == NULL) || (0 == strnlen_s(param->json_data, RSIZE_MAX_STR))) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("json_data is NULL or empty\n");
		goto err;
	}

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("parse json error\n");
		goto err;
	}

	if (libwrap_check_input_attrs(rack_update_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	return rack_handle_update(&tid, req, result, param);

err:
	if (req)
		json_free(req);
	if (result)
		json_free(result);

	return NULL;
}

static input_attr_t factory_default_attrs[] = {

};

static json_t *rack_default_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	json_t *result = NULL;
	pthread_t tid;
	uint32 ary_size = sizeof(factory_default_attrs)/sizeof(input_attr_t);

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		update_response_info(param, HTTP_BAD_REQUEST);
		HTTPD_ERR("parse json error\n");
		goto err;
	}

	if (libwrap_check_input_attrs(factory_default_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		HTTPD_ERR("json_object fail\n");
		return NULL;
	}

	return rack_handle_default(req, result, param);

err:
	if (req)
		json_free(req);
	if (result)
		json_free(result);

	return NULL;
}


static json_t *mbpc_process_reset(int32 idx, struct rest_uri_param *param)
{
	int64 reset_type = 0;
	result_t rs = RESULT_OK;
	int32 reset_mode = 0;
	int32 reset_sys_result;
	json_t *result = NULL;

	rs = libwrap_mbp_hard_reset(idx, &reset_sys_result);

	if (rs != RESULT_OK) {
		HTTPD_ERR("reset mbp fail, result is %d\n", rs);
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
	update_response_info(param, HTTP_OK);

	int8 buff[128] = {};
	snprintf_s_i(buff, sizeof(buff), "%d", idx);
	rf_snmp_evt(INFO, MSGMBPStatusChange, buff, RMM_JSON_RESETED_BIG);

	result = json_object();
	if (result == NULL) {
		update_response_info(param, HTTP_INTERNAL_SERVER_ERROR);
		return NULL;
	}
	json_object_add(result, RMM_JSON_RESULT, json_string("Success"));
	return result;
}


static input_attr_t mbpc_reset_attrs[] = {
};

static json_t *mbpc_reset_post(struct rest_uri_param *param)
{
	json_t *req = NULL;
	int32 cm_lid = 0;
	int32 url_len = 0;
	uint32 ary_size = sizeof(mbpc_reset_attrs)/sizeof(input_attr_t);
	int8 cm_dev[64] = {0};

	url_len = strnlen_s(param->url, sizeof(param->url));
	if (url_len != 0) {
		sscanf(param->url, "/redfish/v1/Managers/MBPC%d/Actions/Manager.Reset", &cm_lid);
	}

	req = json_parse_with_len(param->json_data, (int32)(param->content_length));
	if (req == NULL) {
		HTTPD_ERR("json parse error\n");
		update_response_info(param, HTTP_BAD_REQUEST);
		return NULL;
	}

	if (libwrap_check_input_attrs(mbpc_reset_attrs, ary_size, req, NULL) != RESULT_OK) {
		update_response_info(param, HTTP_BAD_REQUEST);
		goto err;
	}

	snprintf_s_i(cm_dev, sizeof(cm_dev), "/dev/ttyCm%dIPMI", cm_lid);
	if (cm_lid <1 || cm_lid > 2 || (access(cm_dev, R_OK) != 0)) {
		HTTPD_ERR("cm not exist\n");
		update_response_info(param, HTTP_RESOURCE_NOT_FOUND);
		json_free(req);
		libwrap_set_firmware_update_status(1);
		return NULL;
	}

	if (req)
		json_free(req);

	HTTPD_INFO("processing mbp reset\n");
	return mbpc_process_reset(cm_lid, param);

err:
	if (req)
		json_free(req);

	return NULL;
}



static struct rest_handler manager_coll_handler = {
	.get    = manager_coll_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};

static struct rest_handler manager_rmc_handler = {
	.get    = manager_rmc_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};

static struct rest_handler manager_mbp_handler = {
	.get    = manager_mbp_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};


static struct rest_handler manager_reset_handler = {
	.get    = manager_rmc_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = manager_reset_post,
	.del	= NULL,
};

static struct rest_handler manager_network_srv_handler = {
	.get    = manager_network_srv_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};

static struct rest_handler manager_eth_inf_handler = {
	.get    = manager_eth_inf_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};

static struct rest_handler manager_eth_inf_coll_handler = {
	.get    = manager_eth_inf_coll_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};

static struct rest_handler manager_vlans_handler = {
	.get    = manager_vlans_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};

static struct rest_handler manager_vlan_handler = {
	.get    = manager_vlan_get,
	.put    = NULL,
	.patch	= NULL,
	.post   = NULL,
	.del	= NULL,
};


static struct rest_handler rack_reset_handler = {
	.get	= NULL,
	.put	= NULL,
	.patch	= NULL,
	.post	= rack_reset_post,
	.del	= NULL,
};

static struct rest_handler rack_update_handler = {
	.get	= rack_update_get,
	.put	= NULL,
	.patch	= NULL,
	.post	= rack_update_post,
	.del	= NULL,
};

static struct rest_handler rack_default_handler = {
	.get	= NULL,
	.put	= NULL,
	.patch	= NULL,
	.post	= rack_default_post,
	.del	= NULL,
};


static struct rest_handler mbpc_reset_handler = {
	.get    = NULL,
	.put    = NULL,
	.patch  = NULL,
	.post   = mbpc_reset_post,
	.del    = NULL,
};


