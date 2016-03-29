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

#ifndef __TIZEN_MESSAGING_PRIVATE_H__
#define __TIZEN_MESSAGING_PRIVATE_H__

#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_MESSAGING"
#define DBG_MODE (1)

#ifdef __cplusplus
extern "C"
{
#endif

#include <system_info.h>

typedef struct _messages_service_s {
	msg_handle_t service_h;
	void*        incoming_cb;
	void*        incoming_cb_user_data;
	bool         incoming_cb_enabled;
	GSList*      sent_cb_list;
	GSList*      push_incoming_cb_list;
} messages_service_s;

typedef struct _messages_message_s {
	msg_struct_t msg_h;
	char*        text;
	GSList*      attachment_list;
} messages_message_s;

typedef struct _messages_attachment_s {
	int          media_type;
	char         filepath[MSG_FILEPATH_LEN_MAX +1];
} messages_attachment_s;

typedef struct _messages_sent_callback_s {
	int          req_id;
	void*        callback;
	void*        user_data;
} messages_sent_callback_s;

typedef struct _messages_push_incoming_callback_s {
	char*        app_id;
	void*        callback;
	void*        user_data;
} messages_push_incoming_callback_s;

#define MESSAGES_PRIVILEGE_READ			"http://tizen.org/privilege/message.read"
#define MESSAGES_PRIVILEGE_WRITE		"http://tizen.org/privilege/message.write"

#define MESSAGES_TELEPHONY_SMS_FEATURE       "http://tizen.org/feature/network.telephony.sms"
#define MESSAGES_TELEPHONY_MMS_FEATURE   "http://tizen.org/feature/network.telephony.mms"

#define ERROR_CONVERT(err) _messages_error_converter(err, __FUNCTION__, __LINE__);
#define CHECK_NULL(p) \
	if (NULL == p) { \
		LOGE("[%s] INVALID_PARAMETER(0x%08x) %s is null.", \
			__FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER, #p); \
		return MESSAGES_ERROR_INVALID_PARAMETER; \
	}

#define IS_SMS(x) ((x & MESSAGES_TYPE_SMS) == MESSAGES_TYPE_SMS)
#define IS_MMS(x) ((x & MESSAGES_TYPE_MMS) == MESSAGES_TYPE_MMS)

#define CHECK_MESSAGES_SUPPORTED(feature_name) \
	do { \
		bool is_supported = false; \
		if (!system_info_get_platform_bool(feature_name, &is_supported)) { \
			if (is_supported == false) { \
				LOGE("[%s] feature is disabled", feature_name); \
				return MESSAGES_ERROR_NOT_SUPPORTED; \
			} \
		} else { \
			LOGE("Error - Feature getting from System Info"); \
			return MESSAGES_ERROR_OPERATION_FAILED; \
		} \
	} while (0)

int _messages_convert_mbox_to_fw(messages_message_box_e mbox);
int _messages_convert_msgtype_to_fw(messages_message_type_e type);
int _messages_convert_recipient_to_fw(messages_recipient_type_e type);
int _messages_error_converter(int err, const char *func, int line);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MESSAGING_PRIVATE_H__ */
