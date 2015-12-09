/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <glib.h>

#include <privacy_checker_client.h>

#include <msg.h>
#include <msg_transport.h>
#include <msg_storage.h>

#include <messages.h>
#include <messages_types.h>
#include <messages_private.h>

/* Private Utility Functions */
int _messages_get_media_type_from_filepath(const char *filepath);
int _messages_save_mms_data(messages_message_s * msg);
int _messages_load_mms_data(messages_message_s * msg, msg_handle_t handle);
int _messages_save_textfile(const char *text, char **filepath);
int _messages_load_textfile(const char *filepath, char **text);
void _messages_sent_mediator_cb(msg_handle_t handle, msg_struct_t pStatus, void *user_param);
int _messages_check_feature(char *feature_name);

int messages_open_service(messages_service_h * svc)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	messages_service_s *_svc;

	CHECK_NULL(svc);

	_svc = (messages_service_s *) calloc(1, sizeof(messages_service_s));
	if (NULL == _svc) {
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '_svc'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	_svc->sent_cb_list = NULL;
	_svc->incoming_cb = NULL;
	_svc->incoming_cb_enabled = false;
	_svc->push_incoming_cb_list = NULL;

	ret = msg_open_msg_handle(&_svc->service_h);
	if (MSG_SUCCESS != ret) {
		free(_svc);
		return ERROR_CONVERT(ret);
	}

	ret = msg_reg_sent_status_callback(_svc->service_h, &_messages_sent_mediator_cb, (void *)_svc);
	if (MSG_SUCCESS != ret) {
		free(_svc);
		return ERROR_CONVERT(ret);
	}

	*svc = (messages_service_h) _svc;

	return MESSAGES_ERROR_NONE;
}

void _free_push_incoming_cb(messages_push_incoming_callback_s * cb)
{
	if (NULL != cb) {
		if (NULL != cb->app_id)
			free(cb->app_id);
		free(cb);
	}
}

int messages_close_service(messages_service_h svc)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;

	messages_service_s *_svc = (messages_service_s *) svc;
	CHECK_NULL(_svc);

	ret = msg_close_msg_handle(&_svc->service_h);

	if (_svc->sent_cb_list) {
		g_slist_foreach(_svc->sent_cb_list, (GFunc) g_free, NULL);
		g_slist_free(_svc->sent_cb_list);
		_svc->sent_cb_list = NULL;
	}

	if (_svc->push_incoming_cb_list) {
		g_slist_foreach(_svc->push_incoming_cb_list, (GFunc) _free_push_incoming_cb, NULL);
		g_slist_free(_svc->push_incoming_cb_list);
		_svc->push_incoming_cb_list = NULL;
	}

	free(svc);

	return ERROR_CONVERT(ret);
}

int messages_create_message(messages_message_type_e type, messages_message_h * msg)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	messages_message_s *_msg = NULL;

	CHECK_NULL(msg);

	_msg = (messages_message_s *) calloc(1, sizeof(messages_message_s));
	if (NULL == _msg) {
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	_msg->msg_h = msg_create_struct(MSG_STRUCT_MESSAGE_INFO);
	if (NULL == _msg->msg_h) {
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg->msg_h'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		free(_msg);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	_msg->text = NULL;
	_msg->attachment_list = NULL;

	if (MESSAGES_TYPE_SMS == type) {
		ret = ERROR_CONVERT(msg_set_int_value(_msg->msg_h, MSG_MESSAGE_TYPE_INT, MSG_TYPE_SMS));
		if (MESSAGES_ERROR_NONE != ret) {
			msg_release_struct(&_msg->msg_h);
			free(_msg);
			return ret;
		}
	} else if (MESSAGES_TYPE_MMS == type) {
		ret = _messages_check_feature(MESSAGES_TELEPHONY_MMS_FEATURE);
		if (MESSAGES_ERROR_NONE != ret) {
			msg_release_struct(&_msg->msg_h);
			free(_msg);
			return ret;
		}

		ret = ERROR_CONVERT(msg_set_int_value(_msg->msg_h, MSG_MESSAGE_TYPE_INT, MSG_TYPE_MMS));
		if (MESSAGES_ERROR_NONE != ret) {
			msg_release_struct(&_msg->msg_h);
			free(_msg);
			return ret;
		}
	} else {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type is unknown.", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		msg_release_struct(&_msg->msg_h);
		free(_msg);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	*msg = (messages_message_h) _msg;

	return MESSAGES_ERROR_NONE;
}

int messages_destroy_message(messages_message_h msg)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);

	messages_mms_remove_all_attachments(msg);
	if (_msg->text) {
		free(_msg->text);
		_msg->text = NULL;
	}

	ret = msg_release_struct(&_msg->msg_h);

	free(msg);

	return ERROR_CONVERT(ret);
}

int messages_get_message_type(messages_message_h msg, messages_message_type_e * type)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int msgType;
	int ret;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(type);

	ret = msg_get_int_value(_msg->msg_h, MSG_MESSAGE_TYPE_INT, &msgType);
	if (ret != MSG_SUCCESS)
		return ERROR_CONVERT(ret);

	switch (msgType) {
	case MSG_TYPE_SMS_CB:
	case MSG_TYPE_SMS_JAVACB:
		*type = MESSAGES_TYPE_SMS_CB;
		break;
	case MSG_TYPE_SMS_WAPPUSH:
		*type = MESSAGES_TYPE_SMS_PUSH;
		break;
	case MSG_TYPE_SMS:
	case MSG_TYPE_SMS_MWI:
	case MSG_TYPE_SMS_SYNCML:
	case MSG_TYPE_SMS_REJECT:
		*type = MESSAGES_TYPE_SMS;
		break;
	case MSG_TYPE_MMS:
	case MSG_TYPE_MMS_NOTI:
	case MSG_TYPE_MMS_JAVA:
		*type = MESSAGES_TYPE_MMS;
		break;
	default:
		*type = MESSAGES_TYPE_UNKNOWN;
		break;
	}

	return MESSAGES_ERROR_NONE;
}

int messages_add_address(messages_message_h msg, const char *address, messages_recipient_type_e type)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;

	messages_message_type_e msgType;
	msg_struct_t addr_info;

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(address);

	messages_get_message_type(msg, &msgType);

	ret = msg_list_add_item(_msg->msg_h, MSG_MESSAGE_ADDR_LIST_HND, &addr_info);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	if (IS_SMS(msgType)) {
		msg_set_int_value(addr_info, MSG_ADDRESS_INFO_RECIPIENT_TYPE_INT, MSG_RECIPIENTS_TYPE_TO);
	} else if (IS_MMS(msgType)) {
		if (strchr(address, '@') == NULL)
			msg_set_int_value(addr_info, MSG_ADDRESS_INFO_ADDRESS_TYPE_INT, MSG_ADDRESS_TYPE_PLMN);
		else
			msg_set_int_value(addr_info, MSG_ADDRESS_INFO_ADDRESS_TYPE_INT, MSG_ADDRESS_TYPE_EMAIL);

		msg_set_int_value(addr_info, MSG_ADDRESS_INFO_RECIPIENT_TYPE_INT, _messages_convert_recipient_to_fw(type));
	} else {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type is unknown.", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	ret = msg_set_str_value(addr_info, MSG_ADDRESS_INFO_ADDRESS_VALUE_STR, (char *)address, strlen(address));
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	return MESSAGES_ERROR_NONE;
}

int messages_get_address_count(messages_message_h msg, int *count)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	msg_list_handle_t addr_list = NULL;

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(count);

	ret = msg_get_list_handle(_msg->msg_h, MSG_MESSAGE_ADDR_LIST_HND, (void **)&addr_list);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	*count = msg_list_length(addr_list);

	return MESSAGES_ERROR_NONE;
}

int messages_get_address(messages_message_h msg, int index, char **address, messages_recipient_type_e * type)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	char _address[MAX_ADDRESS_VAL_LEN + 1] = { 0, };
	int _type;
	int count;

	msg_list_handle_t addr_list = NULL;
	msg_struct_t addr_info;

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(address);

	ret = msg_get_list_handle(_msg->msg_h, MSG_MESSAGE_ADDR_LIST_HND, (void **)&addr_list);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	count = msg_list_length(addr_list);
	if (index > count) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : index(%d) > addr_list->nCount(%d) ", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER, index, count);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	addr_info = (msg_struct_t) msg_list_nth_data(addr_list, index);

	ret = msg_get_str_value(addr_info, MSG_ADDRESS_INFO_ADDRESS_VALUE_STR, _address, MAX_ADDRESS_VAL_LEN);
	if (MSG_SUCCESS != ret) {
		*address = NULL;
	} else {
		*address = strdup(_address);
		if (NULL == *address) {
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*address'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}
	}

	if (NULL != type) {
		ret = msg_get_int_value(addr_info, MSG_ADDRESS_INFO_RECIPIENT_TYPE_INT, &_type);
		if (MSG_SUCCESS != ret) {
			*type = MESSAGES_RECIPIENT_UNKNOWN;
		} else {
			switch (_type) {
			case MSG_RECIPIENTS_TYPE_TO:
				*type = MESSAGES_RECIPIENT_TO;
				break;
			case MSG_RECIPIENTS_TYPE_CC:
				*type = MESSAGES_RECIPIENT_CC;
				break;
			case MSG_RECIPIENTS_TYPE_BCC:
				*type = MESSAGES_RECIPIENT_BCC;
				break;
			default:
				*type = MESSAGES_RECIPIENT_UNKNOWN;
				break;
			}
		}
	}

	return MESSAGES_ERROR_NONE;
}

int messages_remove_all_addresses(messages_message_h msg)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	ret = msg_list_clear(_msg->msg_h, MSG_MESSAGE_ADDR_LIST_HND);

	return ERROR_CONVERT(ret);
}

void _dump_message(messages_message_h msg)
{

}

int messages_send_message(messages_service_h svc, messages_message_h msg, bool save_to_sentbox, messages_sent_cb callback, void *user_data)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	/* Privacy check */
	int privacy_ret = privacy_checker_check_by_privilege(MESSAGES_PRIVILEGE_WRITE);
	if (privacy_ret == PRIV_MGR_ERROR_USER_NOT_CONSENTED)
		return MESSAGES_ERROR_PERMISSION_DENIED;

	int ret;
	int reqId;
	msg_struct_t req;
	msg_struct_t sendOpt;
	msg_struct_t option = NULL;
	messages_message_type_e msgType;

	messages_service_s *_svc = (messages_service_s *) svc;
	messages_message_s *_msg = (messages_message_s *) msg;

	messages_sent_callback_s *_cb;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	sendOpt = msg_create_struct(MSG_STRUCT_SENDOPT);
	msg_set_bool_value(sendOpt, MSG_SEND_OPT_SETTING_BOOL, true);
	msg_set_bool_value(sendOpt, MSG_SEND_OPT_DELIVER_REQ_BOOL, false);
	msg_set_bool_value(sendOpt, MSG_SEND_OPT_KEEPCOPY_BOOL, save_to_sentbox);

	messages_get_message_type(msg, &msgType);

	if (IS_SMS(msgType)) {
		req = msg_create_struct(MSG_STRUCT_REQUEST_INFO);
		msg_set_struct_handle(req, MSG_REQUEST_MESSAGE_HND, _msg->msg_h);
		msg_set_struct_handle(req, MSG_REQUEST_SENDOPT_HND, sendOpt);

		ret = msg_sms_send_message(_svc->service_h, req);

		msg_get_int_value(req, MSG_REQUEST_REQUESTID_INT, &reqId);
		msg_release_struct(&req);
	} else if (IS_MMS(msgType)) {
		ret = _messages_check_feature(MESSAGES_TELEPHONY_MMS_FEATURE);
		if (MESSAGES_ERROR_NONE != ret) {
			msg_release_struct(&sendOpt);
			return ret;
		}

		ret = _messages_save_mms_data(_msg);
		if (MESSAGES_ERROR_NONE == ret) {
			if (DBG_MODE)
				_dump_message(msg);

			req = msg_create_struct(MSG_STRUCT_REQUEST_INFO);
			msg_set_struct_handle(req, MSG_REQUEST_MESSAGE_HND, _msg->msg_h);
			msg_set_struct_handle(req, MSG_REQUEST_SENDOPT_HND, sendOpt);

			msg_get_struct_handle(sendOpt, MSG_SEND_OPT_MMS_OPT_HND, &option);
			msg_set_bool_value(option, MSG_MMS_SENDOPTION_READ_REQUEST_BOOL, false);
			msg_set_int_value(option, MSG_MMS_SENDOPTION_PRIORITY_INT, MSG_MESSAGE_PRIORITY_NORMAL);
			msg_set_int_value(option, MSG_MMS_SENDOPTION_EXPIRY_TIME_INT, MSG_EXPIRY_TIME_MAXIMUM);
			msg_set_int_value(option, MSG_MMS_SENDOPTION_DELIVERY_TIME_INT, MSG_DELIVERY_TIME_IMMEDIATLY);

			ret = msg_mms_send_message(_svc->service_h, req);

			msg_get_int_value(req, MSG_REQUEST_REQUESTID_INT, &reqId);
			msg_release_struct(&req);
		}
	} else {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : Invalid Message Type.", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		msg_release_struct(&sendOpt);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	msg_release_struct(&sendOpt);

	if (NULL != callback && MSG_SUCCESS == ret) {
		/* Add callback to mapping table */
		_cb = (messages_sent_callback_s *) malloc(sizeof(messages_sent_callback_s));
		if (NULL != _cb) {
			_cb->req_id = reqId;
			_cb->callback = (void *)callback;
			_cb->user_data = user_data;
			_svc->sent_cb_list = g_slist_append(_svc->sent_cb_list, _cb);
		}
	}

	return ERROR_CONVERT(ret);
}

int messages_get_message_count(messages_service_h service, messages_message_box_e mbox, messages_message_type_e type, int *count)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	msg_folder_id_t folderId;
	msg_struct_t countInfo = NULL;
	int nSms, nMms;

	messages_service_s *_svc = (messages_service_s *) service;

	CHECK_NULL(_svc);
	CHECK_NULL(count);

	nSms = 0;
	nMms = 0;

	if (MESSAGES_MBOX_ALL == mbox) {
		if (IS_SMS(type) || MESSAGES_TYPE_UNKNOWN == type) {
			ret = ERROR_CONVERT(msg_count_msg_by_type(_svc->service_h, MSG_TYPE_SMS, &nSms));
			if (MESSAGES_ERROR_NONE != ret)
				return ret;
		}

		if (IS_MMS(type) || MESSAGES_TYPE_UNKNOWN == type) {
			ret = ERROR_CONVERT(msg_count_msg_by_type(_svc->service_h, MSG_TYPE_MMS, &nMms));
			if (MESSAGES_ERROR_NONE != ret)
				return ret;
		}

		*count = nSms + nMms;
	} else {
		countInfo = msg_create_struct(MSG_STRUCT_COUNT_INFO);
		if (NULL == countInfo) {
			LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create 'countInfo'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}

		folderId = _messages_convert_mbox_to_fw(mbox);
		ret = ERROR_CONVERT(msg_count_message(_svc->service_h, folderId, countInfo));
		if (MESSAGES_ERROR_NONE != ret) {
			msg_release_struct(&countInfo);
			return ret;
		}

		msg_get_int_value(countInfo, MSG_COUNT_INFO_SMS_INT, &nSms);
		msg_get_int_value(countInfo, MSG_COUNT_INFO_MMS_INT, &nMms);

		msg_release_struct(&countInfo);

		switch (type) {
		case MESSAGES_TYPE_SMS:
			*count = nSms;
			break;
		case MESSAGES_TYPE_MMS:
			*count = nMms;
			break;
		case MESSAGES_TYPE_UNKNOWN:
			*count = nSms + nMms;
			break;
		default:
			*count = 0;
			break;
		}
	}

	return MESSAGES_ERROR_NONE;
}

int messages_search_message(messages_service_h service, messages_message_box_e mbox, messages_message_type_e type, const char *keyword, const char *address, int offset, int limit, messages_message_h ** message_array, int *length, int *total)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int i;
	int ret;

	msg_struct_list_s msg_list;
	msg_struct_t listCond;
	messages_message_type_e _msgType;

	messages_service_s *_svc = (messages_service_s *) service;
	messages_message_s *_msg = NULL;
	messages_message_h *_array;

	CHECK_NULL(_svc);
	CHECK_NULL(message_array);

	/* Set Condition */
	listCond = msg_create_struct(MSG_STRUCT_MSG_LIST_CONDITION);
	if (NULL == listCond) {
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create 'listCond'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	msg_set_int_value(listCond, MSG_LIST_CONDITION_FOLDER_ID_INT, _messages_convert_mbox_to_fw(mbox));
	msg_set_int_value(listCond, MSG_LIST_CONDITION_MSGTYPE_INT, _messages_convert_msgtype_to_fw(type));

	if (NULL != keyword)
		msg_set_str_value(listCond, MSG_LIST_CONDITION_TEXT_VALUE_STR, strdup(keyword), strlen(keyword));

	if (NULL != address)
		msg_set_str_value(listCond, MSG_LIST_CONDITION_ADDRESS_VALUE_STR, strdup(address), strlen(address));

	/* Search */
	msg_set_bool_value(listCond, MSG_LIST_CONDITION_AND_OPERATER_BOOL, true);
	msg_set_int_value(listCond, MSG_LIST_CONDITION_OFFSET_INT, offset);
	msg_set_int_value(listCond, MSG_LIST_CONDITION_LIMIT_INT, limit);
	ret = msg_get_message_list2(_svc->service_h, listCond, &msg_list);
	msg_release_struct(&listCond);
	if (MSG_SUCCESS != ret) {
		msg_release_list_struct(&msg_list);
		return ERROR_CONVERT(ret);
	}
	/* Result */
	_array = (messages_message_h *) calloc(msg_list.nCount + 1, sizeof(messages_message_h));
	if (NULL == _array) {
		msg_release_list_struct(&msg_list);
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create '_array'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	for (i = 0; i < msg_list.nCount; i++) {
		_msg = (messages_message_s *) calloc(1, sizeof(messages_message_s));
		if (NULL == _msg) {
			LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create '_msg'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
			for (; i > 0; i--)
				free(_array[i - 1]);

			free(_array);
			msg_release_list_struct(&msg_list);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}

		_msg->text = NULL;
		_msg->attachment_list = NULL;
		_msg->msg_h = msg_list.msg_struct_info[i];

		messages_get_message_type((messages_message_h) _msg, &_msgType);

		if (IS_MMS(_msgType))
			_messages_load_mms_data(_msg, _svc->service_h);

		_array[i] = (messages_message_h) _msg;
	}

	*message_array = (messages_message_h *) _array;

	if (NULL != length)
		*length = msg_list.nCount;

	if (NULL != total)
		*total = -1;			/* TODO: total count is not supported yet. */

	/* TODO: where should I free msg_list? */

	return MESSAGES_ERROR_NONE;
}

int messages_free_message_array(messages_message_h * message_array)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int i = 0;

	messages_message_h *_array = (messages_message_h *) message_array;
	CHECK_NULL(_array);

	while (_array[i] != NULL) {
		ret = messages_destroy_message(_array[i]);
		if (MESSAGES_ERROR_NONE != ret)
			LOGW("[%s:%d] messages_destroy_message() is fail. ret = %d", __FUNCTION__, __LINE__, ret);

		i++;
	}

	free(message_array);

	return MESSAGES_ERROR_NONE;
}

int messages_foreach_message(messages_service_h svc, messages_message_box_e mbox, messages_message_type_e type, const char *keyword, const char *address, int offset, int limit, messages_search_cb callback, void *user_data)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	/* Privacy check */
	int privacy_ret = privacy_checker_check_by_privilege(MESSAGES_PRIVILEGE_READ);
	if (privacy_ret == PRIV_MGR_ERROR_USER_NOT_CONSENTED)
		return MESSAGES_ERROR_PERMISSION_DENIED;

	int i;
	int ret;
	bool ret_cb;

	messages_message_h *msg_array;
	int length;
	int total;

	ret = messages_search_message(svc, mbox, type, keyword, address, offset, limit, &msg_array, &length, &total);
	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	for (i = 0; i < length; i++) {
		ret_cb = callback((messages_message_h) msg_array[i], i, length, total, user_data);
		if (!ret_cb)
			break;
	}

	ret = messages_free_message_array(msg_array);
	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	return MESSAGES_ERROR_NONE;
}

void _messages_sent_mediator_cb(msg_handle_t handle, msg_struct_t pStatus, void *user_param)
{
	messages_sending_result_e ret;
	messages_service_s *_svc = (messages_service_s *) user_param;

	int i;
	int status = MSG_NETWORK_SEND_FAIL;
	int reqId = 0;
	messages_sent_callback_s *_cb;

	msg_get_int_value(pStatus, MSG_SENT_STATUS_REQUESTID_INT, &reqId);
	msg_get_int_value(pStatus, MSG_SENT_STATUS_NETWORK_STATUS_INT, &status);

	if (NULL == _svc)
		return;

	for (i = 0; i < g_slist_length(_svc->sent_cb_list); i++) {
		_cb = (messages_sent_callback_s *) g_slist_nth_data(_svc->sent_cb_list, i);
		if (NULL != _cb && _cb->req_id == reqId) {
			ret = (status == MSG_NETWORK_SEND_SUCCESS) ? MESSAGES_SENDING_SUCCEEDED : MESSAGES_SENDING_FAILED;

			((messages_sent_cb) _cb->callback) (ret, _cb->user_data);
			_svc->sent_cb_list = g_slist_remove(_svc->sent_cb_list, _cb);
			free(_cb);
			break;
		}
	}
}

void _messages_incoming_mediator_cb(msg_handle_t handle, msg_struct_t msg, void *user_param)
{
	messages_message_type_e msgType;
	messages_message_s *_msg;
	messages_service_s *_svc = (messages_service_s *) user_param;

	if (NULL == _svc)
		return;

	if (_svc->incoming_cb_enabled && _svc->incoming_cb != NULL) {
		_msg = (messages_message_s *) calloc(1, sizeof(messages_message_s));
		if (NULL == _msg) {
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return;
		}

		_msg->msg_h = msg;

		messages_get_message_type((messages_message_h) _msg, &msgType);

		if (IS_MMS(msgType)) {
			if (MESSAGES_ERROR_NONE != _messages_check_feature(MESSAGES_TELEPHONY_MMS_FEATURE)) {
				free(_msg);
				return;
			}

			_messages_load_mms_data(_msg, handle);
		}

		((messages_incoming_cb) _svc->incoming_cb) ((messages_message_h) _msg, _svc->incoming_cb_user_data);

		messages_mms_remove_all_attachments((messages_message_h) _msg);
		free(_msg);
	}
}

int messages_set_message_incoming_cb(messages_service_h svc, messages_incoming_cb callback, void *user_data)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;

	messages_service_s *_svc = (messages_service_s *) svc;

	CHECK_NULL(_svc);
	CHECK_NULL(callback);

	if (NULL == _svc->incoming_cb) {
		ret = ERROR_CONVERT(msg_reg_sms_message_callback(_svc->service_h, &_messages_incoming_mediator_cb, 0, (void *)_svc)
			);
		if (MESSAGES_ERROR_NONE != ret)
			return ret;

		if (MESSAGES_ERROR_NONE == _messages_check_feature(MESSAGES_TELEPHONY_MMS_FEATURE)) {
			ret = ERROR_CONVERT(msg_reg_mms_conf_message_callback(_svc->service_h, &_messages_incoming_mediator_cb, NULL, (void *)_svc)
				);
			if (MESSAGES_ERROR_NONE != ret)
				return ret;
		}
	}

	_svc->incoming_cb = (void *)callback;
	_svc->incoming_cb_user_data = (void *)user_data;
	_svc->incoming_cb_enabled = true;

	return MESSAGES_ERROR_NONE;
}

int messages_add_sms_listening_port(messages_service_h service, int port)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	messages_service_s *_svc = (messages_service_s *) service;
	CHECK_NULL(_svc);

	if (port <= 0)
		return MESSAGES_ERROR_INVALID_PARAMETER;

	ret = ERROR_CONVERT(msg_reg_sms_message_callback(_svc->service_h, &_messages_incoming_mediator_cb, port, (void *)_svc)
		);

	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	return MESSAGES_ERROR_NONE;
}

int messages_unset_message_incoming_cb(messages_service_h svc)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	messages_service_s *_svc = (messages_service_s *) svc;

	CHECK_NULL(_svc);

	_svc->incoming_cb_enabled = false;
	_svc->incoming_cb = NULL;
	_svc->incoming_cb_user_data = NULL;

	return MESSAGES_ERROR_NONE;
}

int messages_get_message_port(messages_message_h msg, int *port)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int _port;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(port);

	ret = msg_get_int_value(_msg->msg_h, MSG_MESSAGE_DEST_PORT_INT, &_port);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	*port = _port;

	return MESSAGES_ERROR_NONE;
}

int messages_set_text(messages_message_h msg, const char *text)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int len;
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(text);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	if (IS_SMS(type)) {
		len = strlen(text);
		if (len > MAX_MSG_TEXT_LEN) {
			LOGE("[%s] INVALID_PARAMETER(0x%08x) : the length of body exceeded the max, 1530 .", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
			return MESSAGES_ERROR_INVALID_PARAMETER;
		}
		ret = ERROR_CONVERT(msg_set_str_value(_msg->msg_h, MSG_MESSAGE_SMS_DATA_STR, (char *)text, len));
	} else if (IS_MMS(type)) {
		CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

		if (NULL != _msg->text)
			free(_msg->text);

		_msg->text = strdup(text);
		if (NULL == _msg->text) {
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '_msg->text'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}

		ret = MESSAGES_ERROR_NONE;
	} else {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : unknown message type", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	return ret;
}

int messages_get_text(messages_message_h msg, char **text)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	char _text[MAX_MSG_TEXT_LEN + 1] = { 0, };
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(text);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	if (IS_SMS(type)) {
		ret = msg_get_str_value(_msg->msg_h, MSG_MESSAGE_SMS_DATA_STR, _text, MAX_MSG_TEXT_LEN);
		if (MSG_SUCCESS != ret) {
			*text = NULL;
		} else {
			*text = strdup(_text);
			if (NULL == *text) {
				LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*text'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
				return MESSAGES_ERROR_OUT_OF_MEMORY;
			}
		}
	} else if (IS_MMS(type)) {
		if (NULL == _msg->text) {
			*text = NULL;
		} else {
			*text = strdup(_msg->text);
			if (NULL == *text) {
				LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*text'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
				return MESSAGES_ERROR_OUT_OF_MEMORY;
			}
		}
	} else {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : unknown message type", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	return MESSAGES_ERROR_NONE;
}

int messages_get_time(messages_message_h msg, time_t * time)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int _time;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(time);

	ret = msg_get_int_value(_msg->msg_h, MSG_MESSAGE_DISPLAY_TIME_INT, &_time);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	*time = (time_t) _time;

	return MESSAGES_ERROR_NONE;
}

int messages_get_message_id(messages_message_h msg, int *msg_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int _id;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(msg_id);

	ret = msg_get_int_value(_msg->msg_h, MSG_MESSAGE_ID_INT, &_id);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	*msg_id = _id;

	return MESSAGES_ERROR_NONE;
}

int messages_set_sim_id(messages_message_h msg, int sim_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	ret = msg_set_int_value(_msg->msg_h, MSG_MESSAGE_SIM_INDEX_INT, sim_id);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	return MESSAGES_ERROR_NONE;
}

int messages_get_sim_id(messages_message_h msg, int *sim_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int _id;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(sim_id);

	ret = msg_get_int_value(_msg->msg_h, MSG_MESSAGE_SIM_INDEX_INT, &_id);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	*sim_id = _id;

	return MESSAGES_ERROR_NONE;
}

int messages_search_message_by_id(messages_service_h service, int msg_id, messages_message_h * msg)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	msg_struct_t new_msg_h;
	messages_message_type_e _msgType;
	msg_struct_t sendOpt;

	messages_service_s *_svc = (messages_service_s *) service;
	messages_message_s *_msg = NULL;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(msg);

	new_msg_h = msg_create_struct(MSG_STRUCT_MESSAGE_INFO);
	if (NULL == new_msg_h) {
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create 'new_msg_h'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	sendOpt = msg_create_struct(MSG_STRUCT_SENDOPT);
	if (NULL == sendOpt) {
		msg_release_struct(&new_msg_h);
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create 'sendOpt'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	ret = msg_get_message(_svc->service_h, msg_id, new_msg_h, sendOpt);
	if (MSG_SUCCESS != ret) {
		msg_release_struct(&sendOpt);
		msg_release_struct(&new_msg_h);
		return ERROR_CONVERT(ret);
	}

	msg_release_struct(&sendOpt);

	_msg = (messages_message_s *) calloc(1, sizeof(messages_message_s));
	if (NULL == _msg) {
		msg_release_struct(&new_msg_h);
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create '_msg'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	_msg->text = NULL;
	_msg->attachment_list = NULL;
	_msg->msg_h = new_msg_h;

	messages_get_message_type((messages_message_h) _msg, &_msgType);
	if (IS_MMS(_msgType)) {
		ret = _messages_check_feature(MESSAGES_TELEPHONY_MMS_FEATURE);
		if (MESSAGES_ERROR_NONE != ret) {
			free(_msg);
			msg_release_struct(&new_msg_h);
			return ret;
		}

		ret = _messages_load_mms_data(_msg, _svc->service_h);
		if (MESSAGES_ERROR_NONE != ret) {
			free(_msg);
			msg_release_struct(&new_msg_h);
			return ret;
		}
	}

	*msg = (messages_message_h) _msg;

	return MESSAGES_ERROR_NONE;
}

int messages_get_mbox_type(messages_message_h msg, messages_message_box_e * mbox)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	int folder_id;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(mbox);

	ret = msg_get_int_value(_msg->msg_h, MSG_MESSAGE_FOLDER_ID_INT, &folder_id);
	if (MSG_SUCCESS != ret)
		return ERROR_CONVERT(ret);

	switch (folder_id) {
	case MSG_INBOX_ID:
		*mbox = MESSAGES_MBOX_INBOX;
		break;
	case MSG_OUTBOX_ID:
		*mbox = MESSAGES_MBOX_OUTBOX;
		break;
	case MSG_SENTBOX_ID:
		*mbox = MESSAGES_MBOX_SENTBOX;
		break;
	case MSG_DRAFT_ID:
		*mbox = MESSAGES_MBOX_DRAFT;
		break;
	default:
		*mbox = MESSAGES_MBOX_ALL;
		break;
	}

	return MESSAGES_ERROR_NONE;
}

/* MMS */
int messages_mms_set_subject(messages_message_h msg, const char *subject)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

	int ret;
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(subject);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	if (!IS_MMS(type)) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	ret = msg_set_str_value(_msg->msg_h, MSG_MESSAGE_SUBJECT_STR, (char *)subject, strlen(subject));

	return ERROR_CONVERT(ret);
}

int messages_mms_get_subject(messages_message_h msg, char **subject)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

	int ret;
	char _subject[MAX_SUBJECT_LEN + 1] = { 0, };
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s *) msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(subject);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret)
		return ret;

	if (!IS_MMS(type)) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	ret = msg_get_str_value(_msg->msg_h, MSG_MESSAGE_SUBJECT_STR, _subject, MAX_SUBJECT_LEN);
	if (MSG_SUCCESS != ret) {
		*subject = NULL;
	} else {
		*subject = strdup(_subject);
		if (NULL == *subject) {
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*subject'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}
	}

	return MESSAGES_ERROR_NONE;
}

int messages_mms_add_attachment(messages_message_h msg, messages_media_type_e type, const char *path)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

	messages_message_type_e msg_type;

	messages_message_s *_msg = (messages_message_s *) msg;
	messages_attachment_s *attach;
	int input_file_name_length;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(path);

	/* Check Message Type */
	messages_get_message_type(msg, &msg_type);
	if (!IS_MMS(msg_type)) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	input_file_name_length = strlen(path);
	if (input_file_name_length > MSG_FILEPATH_LEN_MAX) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : maximum length of file_name_path can be %d", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER, MSG_FILEPATH_LEN_MAX);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}
	/* New Attach */
	attach = (messages_attachment_s *) calloc(1, sizeof(messages_attachment_s));
	if (NULL == attach) {
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'attach'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	attach->media_type = type;

	strncpy(attach->filepath, path, input_file_name_length);

	/* Append */
	_msg->attachment_list = g_slist_append(_msg->attachment_list, attach);

	return MESSAGES_ERROR_NONE;
}

int messages_mms_get_attachment_count(messages_message_h msg, int *count)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(count);

	/* Check Message Type */
	messages_get_message_type(msg, &type);
	if (!IS_MMS(type)) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}
	/* Count */
	*count = g_slist_length(_msg->attachment_list);

	return MESSAGES_ERROR_NONE;
}

int messages_mms_get_attachment(messages_message_h msg, int index, messages_media_type_e * type, char **path)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

	messages_attachment_s *_attach;
	messages_message_type_e msg_type;

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(path);

	/* Check Message Type */
	messages_get_message_type(msg, &msg_type);
	if (!IS_MMS(msg_type)) {
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS", __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	_attach = (messages_attachment_s *) g_slist_nth_data(_msg->attachment_list, index);
	if (NULL == _attach) {
		*type = MESSAGES_MEDIA_UNKNOWN;
		*path = NULL;
	} else {
		*type = _attach->media_type;
		*path = strdup(_attach->filepath);
		if (NULL == *path) {
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*path'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}
	}

	return MESSAGES_ERROR_NONE;
}

int messages_mms_remove_all_attachments(messages_message_h msg)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_MMS_FEATURE);

	messages_message_s *_msg = (messages_message_s *) msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	if (_msg->attachment_list) {
		g_slist_foreach(_msg->attachment_list, (GFunc) g_free, NULL);
		g_slist_free(_msg->attachment_list);
		_msg->attachment_list = NULL;
	}

	return MESSAGES_ERROR_NONE;
}

int _messages_save_mms_data(messages_message_s * msg)
{
	int i;
	int ret;

	msg_struct_t mms_data;
	msg_struct_t region;

	msg_struct_t page;
	msg_struct_t media;
	msg_struct_t smil_text;
	msg_struct_t mms_attach;

	messages_attachment_s *attach;
	messages_attachment_s *image;
	messages_attachment_s *audio;

	char *filepath = NULL;

	CHECK_NULL(msg);

	mms_data = msg_create_struct(MSG_STRUCT_MMS);
	if (NULL == mms_data) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_create_message failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}
	/* Check Attachments */
	image = NULL;
	audio = NULL;
	for (i = 0; i < g_slist_length(msg->attachment_list); i++) {
		attach = g_slist_nth_data(msg->attachment_list, i);
		if (MESSAGES_MEDIA_IMAGE == attach->media_type) {
			if (NULL == image)
				image = attach;
		} else if (MESSAGES_MEDIA_VIDEO == attach->media_type) {
			if (NULL == image)
				image = attach;
		} else if (MESSAGES_MEDIA_AUDIO == attach->media_type) {
			if (NULL == audio)
				audio = attach;
		}
	}

	/* Layout Setting */
	msg_set_int_value(mms_data, MSG_MMS_ROOTLAYOUT_WIDTH_INT, 100);
	msg_set_int_value(mms_data, MSG_MMS_ROOTLAYOUT_HEIGHT_INT, 100);
	msg_set_int_value(mms_data, MSG_MMS_ROOTLAYOUT_BGCOLOR_INT, 0xffffff);
	msg_set_bool_value(mms_data, MSG_MMS_ROOTLAYOUT_WIDTH_PERCENT_BOOL, true);
	msg_set_bool_value(mms_data, MSG_MMS_ROOTLAYOUT_HEIGHT_PERCENT_BOOL, true);

	if (NULL == image) {
		msg_list_add_item(mms_data, MSG_STRUCT_MMS_REGION, &region);
		msg_set_str_value(region, MSG_MMS_REGION_ID_STR, (char *)"Text", 4);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_LEFT_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_TOP_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_WIDTH_INT, 100);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_HEIGHT_INT, 100);
		msg_set_int_value(region, MSG_MMS_REGION_BGCOLOR_INT, 0xffffff);
	} else if (NULL == msg->text) {
		msg_list_add_item(mms_data, MSG_STRUCT_MMS_REGION, &region);
		msg_set_str_value(region, MSG_MMS_REGION_ID_STR, (char *)"Image", 5);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_LEFT_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_TOP_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_WIDTH_INT, 100);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_HEIGHT_INT, 100);
		msg_set_int_value(region, MSG_MMS_REGION_BGCOLOR_INT, 0xffffff);
	} else {
		msg_list_add_item(mms_data, MSG_STRUCT_MMS_REGION, &region);
		msg_set_str_value(region, MSG_MMS_REGION_ID_STR, (char *)"Image", 5);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_LEFT_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_TOP_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_WIDTH_INT, 100);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_HEIGHT_INT, 50);
		msg_set_int_value(region, MSG_MMS_REGION_BGCOLOR_INT, 0xffffff);

		msg_list_add_item(mms_data, MSG_STRUCT_MMS_REGION, &region);
		msg_set_str_value(region, MSG_MMS_REGION_ID_STR, (char *)"Text", 4);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_LEFT_INT, 0);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_TOP_INT, 50);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_WIDTH_INT, 100);
		msg_set_int_value(region, MSG_MMS_REGION_LENGTH_HEIGHT_INT, 50);
		msg_set_int_value(region, MSG_MMS_REGION_BGCOLOR_INT, 0xffffff);
	}

	/* Add Media */
	msg_list_add_item(mms_data, MSG_STRUCT_MMS_PAGE, &page);
	msg_set_int_value(page, MSG_MMS_PAGE_PAGE_DURATION_INT, 5440);

	if (NULL != image) {
		if (MESSAGES_MEDIA_IMAGE == image->media_type) {
			msg_list_add_item(page, MSG_STRUCT_MMS_MEDIA, &media);
			msg_set_int_value(media, MSG_MMS_MEDIA_TYPE_INT, MMS_SMIL_MEDIA_IMG);
			msg_set_str_value(media, MSG_MMS_MEDIA_REGION_ID_STR, (char *)"Image", 5);
			msg_set_str_value(media, MSG_MMS_MEDIA_FILEPATH_STR, (char *)image->filepath, MSG_FILEPATH_LEN_MAX);
		} else if (MESSAGES_MEDIA_VIDEO == image->media_type) {
			msg_list_add_item(page, MSG_STRUCT_MMS_MEDIA, &media);
			msg_set_int_value(media, MSG_MMS_MEDIA_TYPE_INT, MMS_SMIL_MEDIA_VIDEO);
			msg_set_str_value(media, MSG_MMS_MEDIA_REGION_ID_STR, (char *)"Image", 5);
			msg_set_str_value(media, MSG_MMS_MEDIA_FILEPATH_STR, (char *)image->filepath, MSG_FILEPATH_LEN_MAX);
		}
	}

	if (NULL != audio) {
		msg_list_add_item(page, MSG_STRUCT_MMS_MEDIA, &media);
		msg_set_int_value(media, MSG_MMS_MEDIA_TYPE_INT, MMS_SMIL_MEDIA_AUDIO);
		msg_set_str_value(media, MSG_MMS_MEDIA_REGION_ID_STR, (char *)"Audio", 5);
		msg_set_str_value(media, MSG_MMS_MEDIA_FILEPATH_STR, (char *)audio->filepath, MSG_FILEPATH_LEN_MAX);
	}

	if (NULL != msg->text) {
		ret = _messages_save_textfile(msg->text, &filepath);
		if (MESSAGES_ERROR_NONE == ret) {
			msg_list_add_item(page, MSG_STRUCT_MMS_MEDIA, &media);
			msg_set_int_value(media, MSG_MMS_MEDIA_TYPE_INT, MMS_SMIL_MEDIA_TEXT);
			msg_set_str_value(media, MSG_MMS_MEDIA_REGION_ID_STR, (char *)"Text", 4);
			msg_set_str_value(media, MSG_MMS_MEDIA_FILEPATH_STR, (char *)filepath, MSG_FILEPATH_LEN_MAX);

			msg_get_struct_handle(media, MSG_MMS_MEDIA_SMIL_TEXT_HND, &smil_text);
			msg_set_int_value(smil_text, MSG_MMS_SMIL_TEXT_COLOR_INT, 0x000000);
			msg_set_int_value(smil_text, MSG_MMS_SMIL_TEXT_SIZE_INT, MMS_SMIL_FONT_SIZE_NORMAL);
			msg_set_bool_value(smil_text, MSG_MMS_SMIL_TEXT_BOLD_BOOL, false);
		}

		if (NULL != filepath)
			free(filepath);
	}
	/* Add Attachment */
	for (i = 0; i < g_slist_length(msg->attachment_list); i++) {
		attach = g_slist_nth_data(msg->attachment_list, i);
		if (image != attach && audio != attach) {
			msg_list_add_item(mms_data, MSG_STRUCT_MMS_ATTACH, &mms_attach);
			msg_set_str_value(mms_attach, MSG_MMS_ATTACH_FILEPATH_STR, (char *)attach->filepath, MSG_FILEPATH_LEN_MAX);
		}
	}

	ret = msg_set_mms_struct(msg->msg_h, mms_data);
	if (MSG_SUCCESS != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_set_mms_struct failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		msg_release_struct(&mms_data);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	msg_release_struct(&mms_data);

	return MESSAGES_ERROR_NONE;
}

int _messages_load_mms_data(messages_message_s * msg, msg_handle_t handle)
{
	int i, j;
	int ret;
	int msg_id;
	int media_type;
	char filepath[MSG_FILEPATH_LEN_MAX + 1];

	msg_struct_t new_msg_h;
	msg_struct_t sendOpt;

	msg_struct_t mms_data;

	msg_list_handle_t mms_page_list;
	msg_list_handle_t mms_media_list;
	msg_list_handle_t mms_attach_list;

	msg_struct_t mms_page;
	msg_struct_t mms_media;
	msg_struct_t mms_attach;

	messages_attachment_s *attach = NULL;

	CHECK_NULL(msg);

	/* Get MessageId */
	ret = msg_get_int_value(msg->msg_h, MSG_MESSAGE_ID_INT, &msg_id);
	if (MSG_SUCCESS != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_get_int_value(MSG_MESSAGE_ID_INT) failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}
	/* Load MMS_MESSAGE_DATA_S */
	new_msg_h = msg_create_struct(MSG_STRUCT_MESSAGE_INFO);
	if (new_msg_h == NULL)
		return MESSAGES_ERROR_OPERATION_FAILED;

	sendOpt = msg_create_struct(MSG_STRUCT_SENDOPT);
	if (sendOpt == NULL) {
		msg_release_struct(&new_msg_h);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	ret = msg_get_message(handle, msg_id, new_msg_h, sendOpt);
	if (MSG_SUCCESS != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_get_message failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		msg_release_struct(&sendOpt);
		msg_release_struct(&new_msg_h);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}
	msg_release_struct(&sendOpt);

	mms_data = msg_create_struct(MSG_STRUCT_MMS);
	if (NULL == mms_data) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_create_message failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		msg_release_struct(&new_msg_h);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	ret = msg_get_mms_struct(new_msg_h, mms_data);
	if (MSG_SUCCESS != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_get_message_body failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		msg_release_struct(&mms_data);
		msg_release_struct(&new_msg_h);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}
	/* Load Media, Text */
	msg_get_list_handle(mms_data, MSG_MMS_PAGE_LIST_HND, (void **)&mms_page_list);
	for (i = 0; i < msg_list_length(mms_page_list); i++) {
		mms_page = (msg_struct_t) msg_list_nth_data(mms_page_list, i);
		if (NULL == mms_page)
			continue;

		msg_get_list_handle(mms_page, MSG_MMS_PAGE_MEDIA_LIST_HND, (void **)&mms_media_list);
		for (j = 0; j < msg_list_length(mms_media_list); j++) {
			mms_media = (msg_struct_t) msg_list_nth_data(mms_media_list, j);
			if (NULL == mms_media)
				continue;

			msg_get_int_value(mms_media, MSG_MMS_MEDIA_TYPE_INT, &media_type);
			msg_get_str_value(mms_media, MSG_MMS_MEDIA_FILEPATH_STR, filepath, MSG_FILEPATH_LEN_MAX);

			if (MMS_SMIL_MEDIA_TEXT == media_type) {
				_messages_load_textfile(filepath, &msg->text);
			} else {
				attach = (messages_attachment_s *) calloc(1, sizeof(messages_attachment_s));
				if (NULL == attach) {
					LOGW("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'attach'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
					break;
				}

				strncpy(attach->filepath, filepath, MSG_FILEPATH_LEN_MAX);
				switch (media_type) {
				case MMS_SMIL_MEDIA_IMG:
					attach->media_type = MESSAGES_MEDIA_IMAGE;
					break;
				case MMS_SMIL_MEDIA_VIDEO:
					attach->media_type = MESSAGES_MEDIA_VIDEO;
					break;
				case MMS_SMIL_MEDIA_AUDIO:
					attach->media_type = MESSAGES_MEDIA_AUDIO;
					break;
				default:
					attach->media_type = MESSAGES_MEDIA_UNKNOWN;
				}

				msg->attachment_list = g_slist_append(msg->attachment_list, attach);
			}
		}
	}

	/* Load Attachments */
	msg_get_list_handle(mms_data, MSG_MMS_ATTACH_LIST_HND, (void **)&mms_attach_list);
	for (i = 0; i < msg_list_length(mms_attach_list); i++) {
		mms_attach = (msg_struct_t) msg_list_nth_data(mms_attach_list, i);
		if (NULL == mms_attach)
			continue;

		attach = (messages_attachment_s *) calloc(1, sizeof(messages_attachment_s));
		if (NULL == attach) {
			LOGW("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'attach'.", __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			break;
		}
		msg_get_str_value(mms_attach, MSG_MMS_ATTACH_FILEPATH_STR, filepath, MSG_FILEPATH_LEN_MAX);
		strncpy(attach->filepath, filepath, MSG_FILEPATH_LEN_MAX);
		attach->media_type = _messages_get_media_type_from_filepath(attach->filepath);

		msg->attachment_list = g_slist_append(msg->attachment_list, attach);
	}

	msg_release_struct(&mms_data);
	msg_release_struct(&new_msg_h);

	return MESSAGES_ERROR_NONE;
}

int _messages_save_textfile(const char *text, char **filepath)
{
	FILE *file = NULL;

	CHECK_NULL(text);

	*filepath = (char *)malloc(sizeof(char) * MSG_FILEPATH_LEN_MAX + 1);
	if (NULL == *filepath) {
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create a '*filepath'.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	snprintf(*filepath, MSG_FILEPATH_LEN_MAX + 1, "/tmp/.capi_messages_text_%d.txt", getpid());

	file = fopen(*filepath, "w");
	if (file != NULL) {
		fputs(text, file);
		fclose(file);
	} else {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : opening file for text of message failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	return MESSAGES_ERROR_NONE;
}

int _messages_load_textfile(const char *filepath, char **text)
{
	FILE *file = NULL;
	char buf[1024];
	struct stat st;
	size_t nread, len;
	char *pos;

	CHECK_NULL(filepath);

	file = fopen(filepath, "r");

	if (NULL == file) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : opening file for text of message failed.", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	if (fstat(fileno(file), &st) < 0) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : opening file for text of message failed. (fstat)", __FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		fclose(file);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	if (NULL == *text) {
		*text = (char *)calloc(1, st.st_size + 1);
		if (*text == NULL) {
			fclose(file);
			return MESSAGES_ERROR_OPERATION_FAILED;
		}
		pos = *text;
	} else {
		len = strlen(*text);
		*text = (char *)realloc(*text, len + st.st_size + 2);
		if (*text == NULL) {
			fclose(file);
			return MESSAGES_ERROR_OPERATION_FAILED;
		}
		(*text)[len] = '\n';
		pos = *text + len + 1;
	}

	while (0 < (nread = fread(buf, 1, 1024, file))) {
		memcpy(pos, buf, nread);
		pos += nread;
	}
	pos[0] = '\0';

	fclose(file);

	return 0;
}

int _messages_get_media_type_from_filepath(const char *filepath)
{
	int len;
	int ret;
	char *file_ext;

	if (NULL == filepath)
		return MESSAGES_MEDIA_UNKNOWN;

	/* check the length of filepath */
	len = strlen(filepath);
	if (len < 5)
		return MESSAGES_MEDIA_UNKNOWN;

	/* check extension of file */
	file_ext = (char *)&filepath[len - 4];

	if (strncmp(file_ext, ".jpg", 4) != 0 && strncmp(file_ext, ".gif", 4) != 0 && strncmp(file_ext, ".bmp", 4) != 0 && strncmp(file_ext, ".png", 4) != 0)
		ret = MESSAGES_MEDIA_IMAGE;
	else if (strncmp(file_ext, ".mp4", 4) != 0 && strncmp(file_ext, ".3gp", 4) != 0)
		ret = MESSAGES_MEDIA_VIDEO;
	else if (strncmp(file_ext, ".mid", 4) != 0 && strncmp(file_ext, ".aac", 4) != 0 && strncmp(file_ext, ".amr", 4) != 0)
		ret = MESSAGES_MEDIA_AUDIO;
	else
		ret = MESSAGES_MEDIA_UNKNOWN;

	return ret;
}

int _messages_convert_mbox_to_fw(messages_message_box_e mbox)
{
	int folderId;
	switch (mbox) {
	case MESSAGES_MBOX_INBOX:
		folderId = MSG_INBOX_ID;
		break;
	case MESSAGES_MBOX_OUTBOX:
		folderId = MSG_OUTBOX_ID;
		break;
	case MESSAGES_MBOX_SENTBOX:
		folderId = MSG_SENTBOX_ID;
		break;
	case MESSAGES_MBOX_DRAFT:
		folderId = MSG_DRAFT_ID;
		break;
	default:
		folderId = MSG_ALLBOX_ID;
		break;
	}
	return folderId;
}

int _messages_convert_msgtype_to_fw(messages_message_type_e type)
{
	int msgType;
	switch (type) {
	case MESSAGES_TYPE_SMS:
		msgType = MSG_TYPE_SMS;
		break;
	case MESSAGES_TYPE_MMS:
		msgType = MSG_TYPE_MMS;
		break;
	default:
		msgType = MSG_TYPE_INVALID;
		break;
	}
	return msgType;
}

int _messages_convert_recipient_to_fw(messages_recipient_type_e type)
{
	int ret;
	switch (type) {
	case MESSAGES_RECIPIENT_TO:
		ret = MSG_RECIPIENTS_TYPE_TO;
		break;
	case MESSAGES_RECIPIENT_CC:
		ret = MSG_RECIPIENTS_TYPE_CC;
		break;
	case MESSAGES_RECIPIENT_BCC:
		ret = MSG_RECIPIENTS_TYPE_BCC;
		break;
	default:
		ret = MSG_RECIPIENTS_TYPE_UNKNOWN;
		break;
	}
	return ret;
}

int _messages_error_converter(int err, const char *func, int line)
{
	switch (err) {
	case MSG_ERR_NULL_POINTER:
		LOGE("[%s:%d] NULL_POINTER(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_INVALID_PARAMETER, err);
		return MESSAGES_ERROR_INVALID_PARAMETER;

	case MSG_ERR_INVALID_PARAMETER:
		LOGE("[%s:%d] INVALID_PARAMETER(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_INVALID_PARAMETER, err);
		return MESSAGES_ERROR_INVALID_PARAMETER;

	case -EINVAL:
		LOGE("[%s:%d] EINVAL(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_INVALID_PARAMETER, err);
		return MESSAGES_ERROR_INVALID_PARAMETER;

	case MSG_ERR_SERVER_NOT_READY:
		LOGE("[%s:%d] SERVER_NOT_READY(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_SERVER_NOT_READY, err);
		return MESSAGES_ERROR_SERVER_NOT_READY;

	case MSG_ERR_COMMUNICATION_ERROR:
		LOGE("[%s:%d] COMMUNICATION_ERROR(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED, err);
		return MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED;

	case MSG_ERR_TRANSPORT_ERROR:
		LOGE("[%s:%d] TRANSPORT_ERROR(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_SENDING_FAILED, err);
		return MESSAGES_ERROR_SENDING_FAILED;

	case MSG_ERR_NO_SIM:
		LOGE("[%s:%d] NO_SIM_CARD(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_NO_SIM_CARD, err);
		return MESSAGES_ERROR_NO_SIM_CARD;

	case MSG_ERR_DB_STEP:
		LOGE("[%s:%d] NO_DATA(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_NO_DATA, err);
		return MESSAGES_ERROR_NO_DATA;

	case MSG_ERR_PERMISSION_DENIED:
		return MESSAGES_ERROR_PERMISSION_DENIED;

	case MSG_SUCCESS:
		return MESSAGES_ERROR_NONE;

	default:
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : Error from internal Messaging F/W ret: %d.", func, line, MESSAGES_ERROR_OPERATION_FAILED, err);
		return MESSAGES_ERROR_OPERATION_FAILED;

	}
}

int _messages_check_feature(char *feature_name)
{
	bool is_supported = false;
	if (!system_info_get_platform_bool(feature_name, &is_supported)) {
		if (!is_supported) {
			LOGE("[%s] feature is disabled", feature_name);
			return MESSAGES_ERROR_NOT_SUPPORTED;
		}
	} else {
		LOGE("Error - Feature getting from System Info");
		return MESSAGES_ERROR_OPERATION_FAILED;
	}
	return MESSAGES_ERROR_NONE;
}
