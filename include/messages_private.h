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
	MSG_HANDLE_T service_h;
	void*        sent_cb;
	void*        sent_cb_user_data;
	bool         sent_cb_enabled;
	void*        incoming_cb;
	void*        incoming_cb_user_data;
	bool         incoming_cb_enabled;
} messages_service_s;

typedef struct _messages_message_s {
	msg_message_t msg_h;	
	char *text;
	GSList *attachment_list;
} messages_message_s;

typedef struct _messages_attachment_s {
    int media_type;
    char filepath[MSG_FILEPATH_LEN_MAX];
} messages_attachment_s;


#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MESSAGING_PRIVATE_H__ */
