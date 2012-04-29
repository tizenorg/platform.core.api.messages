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

#include <memory.h>

#include <sys/stat.h>
#include <unistd.h>

#include <glib.h>

#include <dlog.h>
#include <MapiMessage.h>
#include <MapiTransport.h>
#include <MapiStorage.h>
#include <MapiControl.h>
#include <MsgTypes.h>

#include <messages.h>
#include <messages_types.h>
#include <messages_private.h>


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_MESSAGING"
#define DBG_MODE (1)

#define MAX_MESSAGES_TEXT_LEN		1530

/* Private Utility Functions */
int _messages_error_converter(int err, const char *func, int line);
int _messages_get_media_type_from_filepath(const char *filepath);
int _messages_save_mms_data(messages_message_s *msg);
int _messages_load_mms_data(messages_message_s *msg, MSG_HANDLE_T handle);
int _messages_save_textfile(const char *text, char **filepath);
int _messages_load_textfile(const char *filepath, char **text);
MSG_FOLDER_ID_T _messages_convert_mbox_to_fw(messages_message_box_e mbox);
MSG_MESSAGE_TYPE_T _messages_convert_msgtype_to_fw(messages_message_type_e type);


#define ERROR_CONVERT(err) _messages_error_converter(err, __FUNCTION__, __LINE__);
#define CHECK_NULL(p) \
	if (NULL == p) { \
		LOGE("[%s] INVALID_PARAMETER(0x%08x) %s is null.", \
			__FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER, #p); \
		return MESSAGES_ERROR_INVALID_PARAMETER; \
	}	





int messages_open_service(messages_service_h *svc)
{
	int ret;
	messages_service_s *_svc;

	CHECK_NULL(svc);

	_svc = (messages_service_s*)calloc(1, sizeof(messages_service_s));
	if (NULL == svc)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'svc'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	_svc->sent_cb = NULL;
	_svc->sent_cb_user_data = NULL;
	_svc->sent_cb_enabled = false;

	ret = msg_open_msg_handle(&_svc->service_h);

	*svc = (messages_service_h)_svc;

	return ERROR_CONVERT(ret);
}

int messages_close_service(messages_service_h svc)
{
	int ret;

	ret = msg_close_msg_handle(&((messages_service_s*)svc)->service_h);

	free(svc);

	return ERROR_CONVERT(ret);
}

int messages_create_message(messages_message_type_e type, messages_message_h *msg)
{
	int ret;
	messages_message_s *_msg = NULL;

	CHECK_NULL(msg);

	_msg = (messages_message_s*)calloc(1, sizeof(messages_message_s));
	if (NULL == _msg)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}
	
	_msg->msg_h = msg_new_message();
	if (NULL == _msg->msg_h)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg->msg_h'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		free(_msg);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	_msg->text = NULL;
	_msg->attachment_list = NULL;

	if (MESSAGES_TYPE_SMS == type)
	{
		ret = ERROR_CONVERT(msg_set_message_type(_msg->msg_h, MSG_TYPE_SMS));
		if (MESSAGES_ERROR_NONE != ret)
		{
			msg_release_message(&_msg->msg_h);
			free(_msg);
			return ret;
		}
	}
	else if (MESSAGES_TYPE_MMS == type)
	{
		ret = ERROR_CONVERT(msg_set_message_type(_msg->msg_h, MSG_TYPE_MMS));
		if (MESSAGES_ERROR_NONE != ret)
		{
			msg_release_message(&_msg->msg_h);
			free(_msg);
			return ret;
		}				
	}
	else
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type is unknown."
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		msg_release_message(&_msg->msg_h);
		free(_msg);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	*msg = (messages_message_h)_msg;

	return MESSAGES_ERROR_NONE;
}

int messages_destroy_message(messages_message_h msg)
{
	int ret;

	messages_message_s *_msg = (messages_message_s*)msg;
	
	CHECK_NULL(_msg);

	messages_mms_remove_all_attachments(msg);
	if (_msg->text)
	{
		free(_msg->text);
		_msg->text = NULL;
	}

	ret = msg_release_message(&_msg->msg_h);

	free(msg);

	return ERROR_CONVERT(ret);
}

int messages_get_message_type(messages_message_h msg, messages_message_type_e *type)
{
	int msgType;

	messages_message_s *_msg = (messages_message_s*)msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(type);

	msgType = msg_get_message_type(_msg->msg_h);

	switch (msgType)
	{
		case MSG_TYPE_SMS:
		case MSG_TYPE_SMS_CB:
		case MSG_TYPE_SMS_JAVACB:
		case MSG_TYPE_SMS_WAPPUSH:
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

int messages_add_address(messages_message_h msg, const char *address)
{
	int ret;

	messages_message_s *_msg = (messages_message_s*)msg;
	
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(address);

	ret = msg_add_address(_msg->msg_h, address, MSG_RECIPIENTS_TYPE_TO);

	return ERROR_CONVERT(ret);
}

int messages_get_address_count(messages_message_h msg, int *count)
{
	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(count);

	*count = msg_get_address_count(_msg->msg_h);

	return MESSAGES_ERROR_NONE;
}

int messages_get_address(messages_message_h msg, int index, char **address)
{
	const char *_address;

	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(address);

	_address = msg_get_ith_address(_msg->msg_h, index);
	if (NULL == _address)
	{
		*address = NULL;
	}
	else
	{
		*address = strdup(_address);
		if (NULL == *address)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*address'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}
	}

	return MESSAGES_ERROR_NONE;
}

int messages_remove_all_addresses(messages_message_h msg)
{
	int ret;

	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	ret = msg_reset_address(_msg->msg_h);

	return ERROR_CONVERT(ret);
}

void _dump_message(messages_message_h msg)
{
	int ret;
	//messages_message_s *_msg = (messages_message_s*)msg;
	
	messages_message_type_e msgType;
	char *buf;
	int i, count;


	
	LOGD("=======================================================");
	// type
	ret = messages_get_message_type(msg, &msgType);
	if (MESSAGES_ERROR_NONE == ret) {
		switch(msgType) {
			case MESSAGES_TYPE_SMS: LOGD("Type: SMS"); break;
			case MESSAGES_TYPE_MMS: LOGD("Type: MMS"); break;
			default: LOGD("Type: Unknown (%d)", msgType); break;
		}
	} else {
		LOGD("Type: Err (%d)", ret);
	}

	// text
	ret = messages_get_text(msg, &buf);	
	if (MESSAGES_ERROR_NONE == ret) {
		LOGD("Text: %s", buf);
		free(buf);
	} else {
		LOGD("Text: Err (%d)", ret);
	}

	// address
	ret = messages_get_address_count(msg, &count);
	if (MESSAGES_ERROR_NONE == ret) {
		LOGD("Address_Count: %d", count);
		for (i=0; i < count; i++) {
			ret = messages_get_address(msg, i, &buf);
			if (MESSAGES_ERROR_NONE == ret) {
				LOGD("Address[%d]: %s", i, buf);
				free(buf);
			} else {
				LOGD("Address[%d]: Err (%d)", ret);
			}
		}		
	} else {
		LOGD("Address Count: Err (%d)", ret);
	}

}

int messages_send_message(messages_service_h svc, messages_message_h msg)
{
	int ret;
	MSG_REQUEST_S req = {0};
	messages_message_type_e msgType;

	messages_service_s *_svc = (messages_service_s*)svc;
	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	req.msg = (msg_message_t)_msg->msg_h;

	messages_get_message_type(msg, &msgType);

	if (MESSAGES_TYPE_SMS == msgType)
	{
		ret = msg_sms_send_message(_svc->service_h, &req);
	}
	else if (MESSAGES_TYPE_MMS == msgType)
	{
		ret = _messages_save_mms_data(_msg);
		if (MESSAGES_ERROR_NONE == ret)
		{
			if (DBG_MODE)
			{
				_dump_message(msg);
			}
			ret = msg_mms_send_message(_svc->service_h, &req);
		}
	}
	else
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : Invalid Message Type.", 
				__FUNCTION__, TIZEN_ERROR_INVALID_PARAMETER);
		return TIZEN_ERROR_INVALID_PARAMETER;
	}

	return ERROR_CONVERT(ret);
}

int messages_foreach_message_from_db(messages_service_h svc,
							 messages_message_box_e mbox,
							 messages_message_type_e type,
							 const char *keyword, const char *address,
							 int offset, int limit,
							 messages_search_cb callback, void *user_data)
{
	int i;
	int ret;
	bool ret_cb;

	MSG_LIST_S msg_list;
	MSG_SEARCH_CONDITION_S searchCon;
	messages_message_type_e _msgType;
	
	messages_service_s *_svc = (messages_service_s*)svc;
	messages_message_s *_msg = NULL;

	CHECK_NULL(_svc);
	CHECK_NULL(callback);

	// Set Condition
	searchCon.folderId = _messages_convert_mbox_to_fw(mbox);
	searchCon.msgType = _messages_convert_msgtype_to_fw(type);	
	searchCon.pSearchVal = NULL == keyword ? NULL : strdup(keyword);
	searchCon.pAddressVal = NULL == address ? NULL : strdup(address);

	ret = msg_search_message(_svc->service_h, &searchCon, offset, limit, &msg_list);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	for (i=0; i < msg_list.nCount; i++)
	{
		_msg = (messages_message_s*)calloc(1, sizeof(messages_message_s));
		_msg->text = NULL;
		_msg->attachment_list = NULL;
		if (NULL == _msg)
		{
			LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create '_msg'."
				, __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}

		_msg->msg_h = msg_list.msgInfo[i];

		messages_get_message_type((messages_message_h)_msg, &_msgType);

		if (MESSAGES_TYPE_MMS == _msgType)
		{
			// TODO: Well... performance issue.
			// Shoud I load mms data at here?
			_messages_load_mms_data(_msg, _svc->service_h);
		}

		// do callback
		// TODO: total count is not supported yet.
		ret_cb = callback((messages_message_h)_msg, i, msg_list.nCount, -1, user_data);

		messages_mms_remove_all_attachments((messages_message_h)_msg);
		msg_release_message(&_msg->msg_h);
		free(_msg);

		if (false == ret_cb)
		{
			break;
		}
	}

	return MESSAGES_ERROR_NONE;
}

void _messages_sent_mediator_cb(MSG_HANDLE_T handle, MSG_SENT_STATUS_S *pStatus, void *user_param)
{
	messages_sending_result_e ret;
	messages_service_s *_svc = (messages_service_s*)user_param;

	if (NULL == _svc)
	{
		return;
	}

	if (_svc->sent_cb_enabled && _svc->sent_cb != NULL)
	{
		ret = (pStatus->status == MSG_NETWORK_SEND_SUCCESS) ? 
					MESSAGES_SENDING_SUCCEEDED : MESSAGES_SENDING_FAILED;
		((messages_sent_cb)_svc->sent_cb)(ret, _svc->sent_cb_user_data);	
	}
}

int messages_set_message_sent_cb(messages_service_h svc, messages_sent_cb callback, void *user_data)
{
	int ret;

	messages_service_s *_svc = (messages_service_s*)svc;

	CHECK_NULL(_svc);
	CHECK_NULL(callback);

	if (NULL == _svc->sent_cb)
	{
		ret = ERROR_CONVERT(
				msg_reg_sent_status_callback(_svc->service_h, &_messages_sent_mediator_cb, (void*)_svc)
			);
		if (MESSAGES_ERROR_NONE != ret)
		{
			return ret;
		}
	}

	_svc->sent_cb = (void*)callback;
	_svc->sent_cb_user_data = (void*)user_data;
	_svc->sent_cb_enabled = true;
	
	return MESSAGES_ERROR_NONE;
}

int messages_unset_message_sent_cb(messages_service_h svc)
{
	messages_service_s *_svc = (messages_service_s*)svc;

	CHECK_NULL(_svc);

	_svc->sent_cb_enabled = false;

	return MESSAGES_ERROR_NONE;
}

void _messages_incoming_mediator_cb(MSG_HANDLE_T handle, msg_message_t msg, void *user_param)
{
	messages_message_type_e msgType;
	messages_message_s *_msg;
	messages_service_s *_svc = (messages_service_s*)user_param;

	if (NULL == _svc)
	{
		return;
	}

	if (_svc->incoming_cb_enabled && _svc->incoming_cb != NULL)
	{
		_msg = (messages_message_s*)calloc(1, sizeof(messages_message_s));
		if (NULL == _msg)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return;
		}
		
		_msg->msg_h = msg;

		messages_get_message_type((messages_message_h)_msg, &msgType);

		if (MESSAGES_TYPE_MMS == msgType)
		{
			_messages_load_mms_data(_msg, handle);
		}

		((messages_incoming_cb)_svc->incoming_cb)((messages_message_h)_msg, _svc->incoming_cb_user_data);

		free(_msg);
	}
}

int messages_set_message_incoming_cb(messages_service_h svc, messages_incoming_cb callback, void *user_data)
{
	int ret;

	messages_service_s *_svc = (messages_service_s*)svc;

	CHECK_NULL(_svc);
	CHECK_NULL(callback);

	if (NULL == _svc->incoming_cb)
	{
		ret = ERROR_CONVERT(
				msg_reg_sms_message_callback(_svc->service_h, &_messages_incoming_mediator_cb, 0, (void*)_svc)
			);
		if (MESSAGES_ERROR_NONE != ret)
		{
			return ret;
		}

		ret = ERROR_CONVERT(
				msg_reg_mms_conf_message_callback(_svc->service_h, &_messages_incoming_mediator_cb, NULL, (void*)_svc)
			);
		if (MESSAGES_ERROR_NONE != ret)
		{
			return ret;
		}
	}

	_svc->incoming_cb = (void*)callback;
	_svc->incoming_cb_user_data = (void*)user_data;
	_svc->incoming_cb_enabled = true;

	return MESSAGES_ERROR_NONE;
}

int messages_unset_message_incoming_cb(messages_service_h svc)
{
	messages_service_s *_svc = (messages_service_s*)svc;

	CHECK_NULL(_svc);

	_svc->incoming_cb_enabled = false;

	return MESSAGES_ERROR_NONE;
}

int messages_set_text(messages_message_h msg, const char *text)
{
	int ret;
	int len;
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s*)msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(text);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret) {
		return ret;
	}

	if (MESSAGES_TYPE_SMS == type)
	{
		len = strlen(text);
		if (len > MAX_MESSAGES_TEXT_LEN) 
		{
			LOGE("[%s] INVALID_PARAMETER(0x%08x) : the length of body exceeded the max, 1530 ."
				, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
			return MESSAGES_ERROR_INVALID_PARAMETER;
		}
		ret = ERROR_CONVERT(
				msg_sms_set_message_body(_msg->msg_h, text,  len)
				);
	}
	else if (MESSAGES_TYPE_MMS == type)
	{
		if (NULL != _msg->text)
		{
			free(_msg->text);
		}

		_msg->text = strdup(text);
		if (NULL == _msg->text)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '_msg->text'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}

		ret = MESSAGES_ERROR_NONE;
	}
	else
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : unknown message type"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;		
	}

	return ret;
}

int messages_get_text(messages_message_h msg, char **text)
{
	int ret;
	const char *_text;
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s*)msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret) {
		return ret;
	}

	if (MESSAGES_TYPE_SMS == type)
	{
		_text = msg_sms_get_message_body(_msg->msg_h);
		if (NULL == _text)
		{
			*text = NULL;
		}
		else
		{
			*text = strdup(_text);
			if (NULL == *text)
			{
				LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*text'."
					, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
				return MESSAGES_ERROR_OUT_OF_MEMORY;
			}
		}
	}
	else if (MESSAGES_TYPE_MMS == type)
	{
		if (NULL == _msg->text)
		{
			*text = NULL;
		}
		else
		{
			*text = strdup(_msg->text);
			if (NULL == *text)
			{
				LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*text'."
					, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
				return MESSAGES_ERROR_OUT_OF_MEMORY;
			}
		}
	}
	else 
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : unknown message type"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;
	}

	return MESSAGES_ERROR_NONE;
}



// MMS /////////////////////////////////////////////////////////////////////

int messages_mms_set_subject(messages_message_h msg, const char *subject)
{
	int ret;
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s*)msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(subject);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret) {
		return ret;
	}

	if (MESSAGES_TYPE_MMS != type)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;		
	}

	ret = msg_set_subject(_msg->msg_h, subject);

	return ERROR_CONVERT(ret);
}

int messages_mms_get_subject(messages_message_h msg, char **subject)
{
	int ret;
	const char *_subject;
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s*)msg;
	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(subject);

	ret = messages_get_message_type(msg, &type);
	if (MESSAGES_ERROR_NONE != ret) {
		return ret;
	}

	if (MESSAGES_TYPE_MMS != type)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;		
	}

	_subject = msg_get_subject(_msg->msg_h);
	if (NULL == _subject) 
	{
		*subject = NULL;
	}
	else
	{
		*subject = strdup(_subject);
		if (NULL == *subject)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*subject'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}
	}

	return MESSAGES_ERROR_NONE;
}


int messages_mms_add_attachment(messages_message_h msg, messages_media_type_e type, const char *path)
{
	messages_message_type_e msg_type;	

	messages_message_s *_msg = (messages_message_s*)msg;
	messages_attachment_s *attach;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(path);

	// Check Message Type
	messages_get_message_type(msg, &msg_type);
	if (MESSAGES_TYPE_MMS != msg_type)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;		
	}

	// New Attach
	attach = (messages_attachment_s *)calloc(1, sizeof(messages_attachment_s));
	if (NULL == attach)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'attach'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;		
	}

	attach->media_type = type;	
	strncpy(attach->filepath, path, strlen(path));

	// Append
	_msg->attachment_list = g_slist_append(_msg->attachment_list, attach);

	return MESSAGES_ERROR_NONE;
}

int messages_mms_get_attachment_count(messages_message_h msg, int *count)
{
	messages_message_type_e type;

	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(count);

	// Check Message Type
	messages_get_message_type(msg, &type);
	if (MESSAGES_TYPE_MMS != type)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;		
	}

	// Count
	*count = g_slist_length(_msg->attachment_list);

	return MESSAGES_ERROR_NONE;
}

int messages_mms_get_attachment(messages_message_h msg, int index, messages_media_type_e *type, char **path)
{
	messages_attachment_s *_attach;
	messages_message_type_e msg_type;

	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);
	CHECK_NULL(path);

	// Check Message Type
	messages_get_message_type(msg, &msg_type);
	if (MESSAGES_TYPE_MMS != msg_type)
	{
		LOGE("[%s] INVALID_PARAMETER(0x%08x) : the message type should be MESSAGES_TYPE_MMS"
			, __FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER);
		return MESSAGES_ERROR_INVALID_PARAMETER;		
	}

	_attach = (messages_attachment_s *)g_slist_nth_data(_msg->attachment_list, index);
	if (NULL == _attach)
	{
		*type = MESSAGES_MEDIA_UNKNOWN;
		*path = NULL;
	}
	else
	{
		*type = _attach->media_type;
		*path = strdup(_attach->filepath);
		if (NULL == *path)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '*path'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}
	}

	return MESSAGES_ERROR_NONE;
}

int messages_mms_remove_all_attachments(messages_message_h msg)
{
	messages_message_s *_msg = (messages_message_s*)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->msg_h);

	if (_msg->attachment_list)
	{
		g_slist_foreach(_msg->attachment_list, (GFunc)g_free, NULL);
		g_slist_free(_msg->attachment_list);
		_msg->attachment_list = NULL;
	}

	return MESSAGES_ERROR_NONE;
}

int _messages_save_mms_data(messages_message_s *msg)
{
	int i;
	int ret;

	MMS_MESSAGE_DATA_S *mms_data;
	MMS_SMIL_ROOTLAYOUT *layout;
	MMS_PAGE_S *page;
	MMS_MEDIA_S *media;

	messages_attachment_s *attach;
	messages_attachment_s *image;
	messages_attachment_s *audio;

	char *filepath = NULL;

	CHECK_NULL(msg);

	mms_data = msg_mms_create_message();
	if (NULL == mms_data)
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_create_message failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	// Check Attachments
	image = NULL;
	audio = NULL;
	for (i=0; i < g_slist_length(msg->attachment_list); i++)
	{
		attach = g_slist_nth_data(msg->attachment_list, i);
		if (MESSAGES_MEDIA_IMAGE == attach->media_type)
		{
			if (NULL == image)
			{
				image = attach;
			}
		}
		else if (MESSAGES_MEDIA_VIDEO == attach->media_type)
		{
			if (NULL == image)
			{
				image = attach;
			}
		}
		else if (MESSAGES_MEDIA_AUDIO == attach->media_type)
		{
			if (NULL == audio)
			{
				audio = attach;
			}
		}
	}

	// Layout Setting
	layout = msg_mms_set_rootlayout(mms_data, 100, 100, 0xffffff);
	if (NULL == layout)
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_set_rootlayout failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
	   	msg_mms_destroy_message(mms_data);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	if (NULL == image)
	{
		msg_mms_add_region(mms_data, "Text", 0, 0, 100, 100, 0xffffff);
	}
	else if (NULL == msg->text)
	{
		msg_mms_add_region(mms_data, "Image", 0, 0, 100, 100, 0xffffff);
	}
	else
	{
		msg_mms_add_region(mms_data, "Image", 0, 0, 100, 50, 0xffffff);
		msg_mms_add_region(mms_data, "Text", 0, 50, 100, 50, 0xffffff);
	}

	// Add Media
	page = msg_mms_add_page(mms_data, 5440);
	if (NULL == page)
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_add_page failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
	   	msg_mms_destroy_message(mms_data);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	if (NULL != image)
	{
		if (MESSAGES_MEDIA_IMAGE == image->media_type)
		{
			media = msg_mms_add_media(page, MMS_SMIL_MEDIA_IMG, "Image", (char *)image->filepath);
			if (NULL == media)
			{
				LOGW("msg_mms_add_media failed, filepath=%s", image->filepath);
			}
		}
		else if (MESSAGES_MEDIA_VIDEO == image->media_type)
		{
			media = msg_mms_add_media(page, MMS_SMIL_MEDIA_VIDEO, "Image", (char *)image->filepath);
			if (NULL == media)
			{
				LOGW("msg_mms_add_media failed, filepath=%s", image->filepath);
			}
		}
	}

	if (NULL != audio)
	{
		media = msg_mms_add_media(page, MMS_SMIL_MEDIA_AUDIO, NULL, (char *)audio->filepath);
		if (NULL == media)
		{
			LOGW("msg_mms_add_media failed, filepath=%s", audio->filepath);
		}
	}

	if (NULL != msg->text)
	{
		ret = _messages_save_textfile(msg->text, &filepath);
		if (MESSAGES_ERROR_NONE == ret)
		{
			media = msg_mms_add_media(page, MMS_SMIL_MEDIA_TEXT, "Text", (char *)filepath);
			if (NULL != media)
			{
				media->sMedia.sText.nColor = 0x000000;
				media->sMedia.sText.nSize = MMS_SMIL_FONT_SIZE_NORMAL;
				media->sMedia.sText.bBold = false;
			}
			else
			{
				LOGW("msg_mms_add_media failed, filepath=%s", audio->filepath);
			}
		}

		if (NULL != filepath)
		{
			free(filepath);
		}
	}

	// Add Attachment
	for (i=0; i < g_slist_length(msg->attachment_list); i++)
	{
		attach = g_slist_nth_data(msg->attachment_list, i);
		if (image != attach && audio != attach)
		{
			msg_mms_add_attachment(mms_data, (char *)attach->filepath);
		}
	}


	ret = msg_mms_set_message_body(msg->msg_h, mms_data);
	if (MSG_SUCCESS != ret)
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_add_page failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		msg_mms_destroy_message(mms_data);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	msg_mms_destroy_message(mms_data);

	return MESSAGES_ERROR_NONE;
}

int _messages_load_mms_data(messages_message_s *msg, MSG_HANDLE_T handle)
{
	int i,j;
	int ret;
	int msg_id;
	msg_message_t new_msg_h;
	MSG_SENDINGOPT_S sendOpt = {0, };
	MMS_MESSAGE_DATA_S *mms_data;
	MMS_PAGE_S *mms_page;
	MMS_MEDIA_S *mms_media;
	MMS_ATTACH_S *mms_attach;
	messages_attachment_s *attach;

	CHECK_NULL(msg);

	// Get MessageId
	msg_id = msg_get_message_id(msg->msg_h);
	if (MSG_ERR_NULL_POINTER == msg_id)
	{
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_get_message_id failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;	
	}

	// Load MMS_MESSAGE_DATA_S
	new_msg_h = msg_new_message();
	ret = msg_get_message(handle, msg_id, new_msg_h, &sendOpt);
	if (MSG_SUCCESS != ret)
	{
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_get_message failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;			
	}

	mms_data = msg_mms_create_message();
	if (NULL == mms_data)
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_create_message failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
	   	msg_release_message(&new_msg_h);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	ret = msg_mms_get_message_body(new_msg_h, mms_data);
	if (MSG_SUCCESS != ret)
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : msg_mms_get_message_body failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
	   	msg_mms_destroy_message(mms_data);
	   	msg_release_message(&new_msg_h);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	msg_release_message(&new_msg_h);

	// Load Media, Text
	for (i=0; i < mms_data->pageCnt; i++)
	{
		mms_page = msg_mms_get_page(mms_data, i);
		if (NULL == mms_page)
		{
			continue;
		}
		for (j=0; j < mms_page->mediaCnt; j++)
		{
			mms_media = msg_mms_get_media(mms_page, j);
			if (NULL == mms_media)
			{
				continue;
			}

			attach = (messages_attachment_s *)calloc(1, sizeof(messages_attachment_s));
			if (NULL == attach)
			{
				LOGW("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'attach'."
					, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
				break;
			}

			if (MMS_SMIL_MEDIA_TEXT == mms_media->mediatype)
			{
				_messages_load_textfile(mms_media->szFilePath, &msg->text);
			}
			else
			{
				strncpy(attach->filepath, mms_media->szFilePath, MSG_FILEPATH_LEN_MAX);
				switch (mms_media->mediatype)
				{
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

	// Load Attachments
	for (i=0; i < mms_data->attachCnt; i++)
	{
		mms_attach = msg_mms_get_attachment(mms_data, i);
		if (NULL == mms_attach)
		{
			continue;
		}

		attach = (messages_attachment_s *)calloc(1, sizeof(messages_attachment_s));
		if (NULL == attach)
		{
			LOGW("[%s] OUT_OF_MEMORY(0x%08x) fail to create a 'attach'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			break;
		}

		strncpy(attach->filepath, mms_attach->szFilePath, MSG_FILEPATH_LEN_MAX);
		attach->media_type = _messages_get_media_type_from_filepath(attach->filepath);
	}

	msg_mms_destroy_message(mms_data);

	return MESSAGES_ERROR_NONE;
}

int _messages_save_textfile(const char *text, char **filepath)
{
	FILE* file = NULL;

	CHECK_NULL(text);

	*filepath = (char *)malloc(sizeof(char)*MSG_FILEPATH_LEN_MAX+1);
	if (NULL == *filepath)
	{
		LOGE("[%s:%d] OUT_OF_MEMORY(0x%08x) fail to create a '*filepath'."
			, __FUNCTION__, __LINE__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	snprintf(*filepath, MSG_FILEPATH_LEN_MAX+1, "/tmp/.capi_messages_text_%d.txt", getpid());

	file = fopen(*filepath, "w");
	if (file != NULL)
	{
		 fputs(text, file);
		 fclose(file);
	}
	else
	{
	   	LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : opening file for text of message failed.", 
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
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

	if (NULL == file)
	{
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : opening file for text of message failed.", 
			__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	fstat(fileno(file), &st);

	if (NULL == *text)
	{
		*text = (char*)calloc(1, st.st_size + 1);
		pos = *text;
	}
	else
	{
		len = strlen(*text);
		(*text)[len] = '\n';
		*text = (char*)realloc(*text, len + st.st_size + 1);
		pos = *text + len;
	}

	while(0 < (nread = fread(buf, 1, 1024, file)))
	{
		memcpy(pos, buf, nread);
		pos += nread;
	}
	pos[0] = '\0';

	return 0;
}

int _messages_get_media_type_from_filepath(const char *filepath)
{
	int len;
	int ret;
	char *file_ext;

	if (NULL == filepath)
	{
		return MESSAGES_MEDIA_UNKNOWN;
	}
	// check the length of filepath
	len = strlen(filepath);
	if (len < 5) 
	{
		return MESSAGES_MEDIA_UNKNOWN;
	}

	// check extension of file
	file_ext = (char *)&filepath[len - 4];

	if(strncmp(file_ext,".jpg",4) !=0 && strncmp(file_ext,".gif",4) !=0 
				&& strncmp(file_ext,".bmp",4) !=0 && strncmp(file_ext,".png",4) !=0)
	{
		ret = MESSAGES_MEDIA_IMAGE;
	}
	else if(strncmp(file_ext,".mp4",4) !=0 && strncmp(file_ext,".3gp",4) !=0)
	{
		ret = MESSAGES_MEDIA_VIDEO;
	}
	else if(strncmp(file_ext,".mid",4) !=0 && strncmp(file_ext,".aac",4) !=0 && strncmp(file_ext,".amr",4) !=0)
	{
		ret = MESSAGES_MEDIA_AUDIO;
	}
	else
	{
		ret = MESSAGES_MEDIA_UNKNOWN;
	}

	return ret;
}

MSG_FOLDER_ID_T _messages_convert_mbox_to_fw(messages_message_box_e mbox)
{
	MSG_FOLDER_ID_T folderId;
	switch(mbox)
	{
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

MSG_MESSAGE_TYPE_T _messages_convert_msgtype_to_fw(messages_message_type_e type)
{
	MSG_MESSAGE_TYPE_T msgType;
	switch (type)
	{
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


int _messages_error_converter(int err, const char *func, int line)
{
	switch(err)
	{
		case MSG_ERR_NULL_POINTER:
			LOGE("[%s:%d] NULL_POINTER(0x%08x) : Error from internal Messaging F/W ret: %d.", 
				func, line, MESSAGES_ERROR_INVALID_PARAMETER, err);
			return MESSAGES_ERROR_INVALID_PARAMETER;

		case MSG_ERR_INVALID_PARAMETER:
			LOGE("[%s:%d] INVALID_PARAMETER(0x%08x) : Error from internal Messaging F/W ret: %d.",
				func, line, MESSAGES_ERROR_INVALID_PARAMETER, err);
			return MESSAGES_ERROR_INVALID_PARAMETER;

		case -EINVAL:
			LOGE("[%s:%d] EINVAL(0x%08x) : Error from internal Messaging F/W ret: %d.", 
				func, line, MESSAGES_ERROR_INVALID_PARAMETER, err);
			return MESSAGES_ERROR_INVALID_PARAMETER;

		case MSG_ERR_SERVER_NOT_READY:
			LOGE("[%s:%d] SERVER_NOT_READY(0x%08x) : Error from internal Messaging F/W ret: %d.", 
				func, line, MESSAGES_ERROR_SERVER_NOT_READY, err);
			return MESSAGES_ERROR_SERVER_NOT_READY;

		case MSG_ERR_COMMUNICATION_ERROR:
			LOGE("[%s:%d] OMMUNICATION_ERROR(0x%08x) : Error from internal Messaging F/W ret: %d.", 
				func, line, MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED, err);
			return MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED;

		case MSG_ERR_TRANSPORT_ERROR:
			LOGE("[%s:%d] TRANSPORT_ERROR(0x%08x) : Error from internal Messaging F/W ret: %d.", 
				func, line, MESSAGES_ERROR_SENDING_FAILED, err);
			return MESSAGES_ERROR_SENDING_FAILED;

		case MSG_SUCCESS:
			return MESSAGES_ERROR_NONE;

		default:
			LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : Error from internal Messaging F/W ret: %d.", 
				func, line, MESSAGES_ERROR_OPERATION_FAILED, err);
			return MESSAGES_ERROR_OPERATION_FAILED;

	}
}