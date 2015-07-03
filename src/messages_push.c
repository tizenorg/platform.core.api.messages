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

#include <dlog.h>
#include <msg.h>
#include <msg_transport.h>
#include <msg_storage.h>

#include <app.h>

#include <messages.h>
#include <messages_types.h>
#include <messages_private.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_MESSAGING"


void _messages_push_incoming_mediator_cb(msg_handle_t handle, const char *push_header, const char *push_body, int push_body_len, void *user_param)
{
	messages_push_incoming_callback_s *_cb = (messages_push_incoming_callback_s *)user_param;
	
	if (_cb && _cb->callback)
	{
		((messages_push_incoming_cb)_cb->callback)(push_header, push_body, push_body_len, _cb->user_data);
	}
}



int messages_push_add_incoming_cb(messages_service_h service, const char *app_id, 
					  messages_push_incoming_cb callback, void *user_data)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	messages_push_incoming_callback_s *_cb;
	
	messages_service_s *_svc = (messages_service_s *)service;
	
	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(app_id);
	CHECK_NULL(callback);
	
	_cb = (messages_push_incoming_callback_s *)malloc(sizeof(messages_push_incoming_callback_s));
	if (NULL == _cb)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '_cb'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;	
	}

	_cb->app_id = strdup(app_id);
	_cb->callback = callback;
	_cb->user_data = user_data;
	
	ret = msg_reg_push_message_callback(_svc->service_h, _messages_push_incoming_mediator_cb, app_id, (void*)_cb);
	if (MSG_SUCCESS != ret)
	{
		if (_cb->app_id) {
			free(_cb->app_id);
		}
		free(_cb);
		return ERROR_CONVERT(ret);
	}
	
	_svc->push_incoming_cb_list = g_slist_append(_svc->push_incoming_cb_list, _cb);
	
	return MESSAGES_ERROR_NONE;
}

int messages_push_remove_incoming_cb(messages_service_h service, const char *app_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int i;
	messages_push_incoming_callback_s *_cb;

	messages_service_s *_svc = (messages_service_s *)service;
	
	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(app_id);
	
	for (i=0; i < g_slist_length(_svc->push_incoming_cb_list); i++)
	{
		_cb = (messages_push_incoming_callback_s *)g_slist_nth_data(_svc->push_incoming_cb_list, i);
		if (NULL != _cb && NULL != _cb->app_id)
		{
			if (!strcmp(_cb->app_id, app_id)) {				
				_cb->callback = NULL;
			}
		}	
	}
	
	return MESSAGES_ERROR_NONE;
}


int messages_push_register(messages_service_h service, const char *content_type, const char *app_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	
	msg_struct_t push_info;
	char *pkg_name;
	
	messages_service_s *_svc = (messages_service_s *)service;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(content_type);
	CHECK_NULL(app_id);
	
	ret = app_get_id(&pkg_name);
	if (APP_ERROR_NONE != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : app_get_id failed.",
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;			
	}
	
	push_info = msg_create_struct(MSG_STRUCT_PUSH_CONFIG_INFO);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_CONTENT_TYPE_STR, (char *)content_type, MAX_WAPPUSH_CONTENT_TYPE_LEN);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_APPLICATON_ID_STR, (char *)app_id, MAX_WAPPUSH_ID_LEN);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_PACKAGE_NAME_STR, pkg_name, MSG_FILEPATH_LEN_MAX);
	free(pkg_name);
	
	ret = msg_add_push_event(_svc->service_h, push_info);	
	msg_release_struct(&push_info);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}
	
	return MESSAGES_ERROR_NONE;
}

int messages_push_deregister(messages_service_h service, const char *content_type, const char *app_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	
	msg_struct_t push_info;
	char *pkg_name;
	
	messages_service_s *_svc = (messages_service_s *)service;
	
	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(content_type);
	CHECK_NULL(app_id);
	
	ret = app_get_id(&pkg_name);
	if (APP_ERROR_NONE != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : app_get_id failed.",
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;			
	}
	
	push_info = msg_create_struct(MSG_STRUCT_PUSH_CONFIG_INFO);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_CONTENT_TYPE_STR, (char *)content_type, MAX_WAPPUSH_CONTENT_TYPE_LEN);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_APPLICATON_ID_STR, (char *)app_id, MAX_WAPPUSH_ID_LEN);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_PACKAGE_NAME_STR, pkg_name, MSG_FILEPATH_LEN_MAX);
	free(pkg_name);
	
	ret = msg_delete_push_event(_svc->service_h, push_info);
	msg_release_struct(&push_info);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}
	
	return MESSAGES_ERROR_NONE;
}

int messages_push_reregister(messages_service_h service, const char *content_type, const char *app_id,
							 const char *dst_content_type, const char *dst_app_id)
{
	CHECK_MESSAGES_SUPPORTED(MESSAGES_TELEPHONY_SMS_FEATURE);

	int ret;
	
	msg_struct_t push_info;
	msg_struct_t dst_push_info;
	char *pkg_name;
	
	messages_service_s *_svc = (messages_service_s *)service;
	
	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(content_type);
	CHECK_NULL(app_id);
	CHECK_NULL(dst_content_type);
	CHECK_NULL(dst_app_id);
	
	ret = app_get_id(&pkg_name);
	if (APP_ERROR_NONE != ret) {
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : app_get_id failed.",
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;			
	}
	
	push_info = msg_create_struct(MSG_STRUCT_PUSH_CONFIG_INFO);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_CONTENT_TYPE_STR, (char *)content_type, MAX_WAPPUSH_CONTENT_TYPE_LEN);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_APPLICATON_ID_STR, (char *)app_id, MAX_WAPPUSH_ID_LEN);
	msg_set_str_value(push_info, MSG_PUSH_CONFIG_PACKAGE_NAME_STR, pkg_name, MSG_FILEPATH_LEN_MAX);
	
	dst_push_info = msg_create_struct(MSG_STRUCT_PUSH_CONFIG_INFO);
	msg_set_str_value(dst_push_info, MSG_PUSH_CONFIG_CONTENT_TYPE_STR, (char *)dst_content_type, MAX_WAPPUSH_CONTENT_TYPE_LEN);
	msg_set_str_value(dst_push_info, MSG_PUSH_CONFIG_APPLICATON_ID_STR, (char *)dst_app_id, MAX_WAPPUSH_ID_LEN);
	msg_set_str_value(dst_push_info, MSG_PUSH_CONFIG_PACKAGE_NAME_STR, pkg_name, MSG_FILEPATH_LEN_MAX);
	free(pkg_name);
	
	ret = msg_update_push_event(_svc->service_h, push_info, dst_push_info);
	msg_release_struct(&push_info);
	msg_release_struct(&dst_push_info);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}
	
	return MESSAGES_ERROR_NONE;
}
