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

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _messages_service_s {
	msg_handle_t service_h;
	void*        incoming_cb;
	void*        incoming_cb_user_data;
	bool         incoming_cb_enabled;
	GSList*      sent_cb_list;
	GSList*      push_incoming_cb_list;
		
	bool         cbs_enabled_cb;
	
	void*        cbs_incoming_cb;
	void*        cbs_incoming_cb_user_data;
	bool         cbs_incoming_cb_save;
	
	void*        etws_incoming_cb;
	void*        etws_incoming_cb_user_data;
	
	messages_cb_settings_h cbs_settings_h;
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



// CBS

#define MESSAGES_ETWS_WARNING_INFO_LEN 50

typedef struct _messages_cb_message_s {
	msg_struct_t cb_msg_h;
} messages_cb_message_s;

typedef struct _messages_cb_settings_s {
	msg_struct_t _struct;
	GSList*      channel_list;
} messages_cb_settings_s;

typedef struct _messages_cb_channel_s {
	int          from_id;
	int          to_id;
	char         name[CB_CHANNEL_NAME_MAX + 1];
	bool         activated;
} messages_cb_channel_s;



#define ERROR_CONVERT(err) _messages_error_converter(err, __FUNCTION__, __LINE__);
#define CHECK_NULL(p) \
	if (NULL == p) { \
		LOGE("[%s] INVALID_PARAMETER(0x%08x) %s is null.", \
			__FUNCTION__, MESSAGES_ERROR_INVALID_PARAMETER, #p); \
		return MESSAGES_ERROR_INVALID_PARAMETER; \
	}
	
#define IS_SMS(x) ((x & MESSAGES_TYPE_SMS) == MESSAGES_TYPE_SMS)
#define IS_MMS(x) ((x & MESSAGES_TYPE_MMS) == MESSAGES_TYPE_MMS)

	
int _messages_convert_mbox_to_fw(messages_message_box_e mbox);
int _messages_convert_msgtype_to_fw(messages_message_type_e type);
int _messages_convert_recipient_to_fw(messages_recipient_type_e type);
int _messages_error_converter(int err, const char *func, int line);

int _messages_cb_clear_settings(messages_cb_settings_s *settings);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MESSAGING_PRIVATE_H__ */
