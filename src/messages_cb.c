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

#include <messages.h>
#include <messages_types.h>
#include <messages_private.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CAPI_MESSAGING"



void _messages_cb_incoming_mediator_cb(msg_handle_t handle, msg_struct_t msg, void *user_param)
{
	int cbType;
	int etwsWarningType;
	char etwsWarningInfo[MESSAGES_ETWS_WARNING_INFO_LEN];

	messages_cb_message_s *_msg = NULL;
	messages_service_s *_svc = (messages_service_s*)user_param;

	if (NULL == _svc) {
		LOGW("[%s:%d] _svc is NULL.", __FUNCTION__, __LINE__);
		return;
	}

	if (NULL == msg) {
		LOGW("[%s:%d] msg is NULL.", __FUNCTION__, __LINE__);
		return;
	}

	if (_svc->etws_incoming_cb || _svc->cbs_incoming_cb)
	{

		_msg = (messages_cb_message_s *)calloc(1, sizeof(messages_message_s));
		if (NULL == _msg)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_msg'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			return;
		}

		_msg->cb_msg_h = msg;

		msg_get_int_value(msg, MSG_CB_MSG_TYPE_INT, &cbType);

		if (MSG_TYPE_SMS_ETWS_PRIMARY == cbType)
		{
			if (_svc->etws_incoming_cb)
			{
				msg_get_int_value(msg, MSG_CB_MSG_ETWS_WARNING_TYPE_INT, &etwsWarningType);
				msg_get_str_value(msg, MSG_CB_MSG_ETWS_WARNING_SECU_INFO_STR, etwsWarningInfo, MESSAGES_ETWS_WARNING_INFO_LEN);

				((messages_cb_etws_primary_noti_cb)_svc->etws_incoming_cb)((messages_cb_message_h)_msg, etwsWarningType, etwsWarningInfo, _svc->etws_incoming_cb_user_data);
			}
		}
		else {
			if (_svc->cbs_incoming_cb)
			{
				((messages_cb_incoming_cb)_svc->cbs_incoming_cb)((messages_cb_message_h)_msg, _svc->cbs_incoming_cb_user_data);
			}
		}

		free(_msg);
	}
}


int messages_cb_get_message_id(messages_cb_message_h msg, int *msg_id)
{
	int ret;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(msg_id);

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_MSG_ID_INT, msg_id);

	return ERROR_CONVERT(ret);
}

int messages_cb_get_serial_number(messages_cb_message_h msg, int *serial)
{
	int ret;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(serial);

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_SERIAL_NUM_INT, serial);

	return ERROR_CONVERT(ret);
}

int messages_cb_get_dcs(messages_cb_message_h msg, int *dcs)
{
	int ret;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(dcs);

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_DCS_INT, dcs);

	return ERROR_CONVERT(ret);
}

int messages_cb_get_message_type(messages_cb_message_h msg, messages_message_type_e *type)
{
	int ret;
	int msgType;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(type);

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_TYPE_INT, &msgType);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	switch (msgType)
	{
	case MSG_TYPE_SMS_ETWS_PRIMARY:
		*type = MESSAGES_TYPE_SMS_ETWS_PRIMARY;
		break;
	case MSG_TYPE_SMS_ETWS_SECONDARY:
		*type = MESSAGES_TYPE_SMS_ETWS_SECONDARY;
		break;
	default:
		*type = MESSAGES_TYPE_SMS_CB;
		break;
	}

	return MESSAGES_ERROR_NONE;
}

int messages_cb_get_language_type(messages_cb_message_h msg, char **type)
{
	int ret;
	int len;
	char *_type;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(type);

	len = 15;

	_type = (char*)calloc(len + 1, sizeof(char)); // Maximum 15 bytes
	if (NULL == _type)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_type'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}
	ret = msg_get_str_value(_msg->cb_msg_h, MSG_CB_MSG_LANGUAGE_TYPE_STR, _type, len);
	if (MSG_SUCCESS != ret)
	{
		free(_type);
		return ERROR_CONVERT(ret);
	}

	*type = _type;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_get_text(messages_cb_message_h msg, char **text)
{
	int ret;

	int txtLen;
	int msgType;
	char *_text;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(text);

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_TYPE_INT, &msgType);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	if (MSG_TYPE_SMS_ETWS_PRIMARY == msgType)
	{
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : The type of msg is ETWS_PRIMARY. This msg doesn't have 'text'.",
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_CB_TEXT_LEN_INT, &txtLen);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	_text = (char *)calloc(txtLen + 1, sizeof(char));
	if (NULL == _text)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_text'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	ret = msg_get_str_value(_msg->cb_msg_h, MSG_CB_MSG_CB_TEXT_STR, _text, txtLen + 1);
	if (MSG_SUCCESS != ret)
	{
		free(_text);
		return ERROR_CONVERT(ret);
	}

	*text = _text;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_get_time(messages_cb_message_h msg, time_t *time)
{
	int ret;
	int _time;

	messages_cb_message_s *_msg = (messages_cb_message_s *)msg;

	CHECK_NULL(_msg);
	CHECK_NULL(_msg->cb_msg_h);
	CHECK_NULL(time);

	ret = msg_get_int_value(_msg->cb_msg_h, MSG_CB_MSG_RECV_TIME_INT, &_time);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	*time = (time_t)_time;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_set_incoming_cb(messages_service_h service, bool save,
								messages_cb_incoming_cb callback, void *user_data)
{
	int ret;

	messages_service_s *_svc = (messages_service_s *)service;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(callback);

	if (!_svc->cbs_enabled_cb)
	{
		ret = msg_reg_cb_message_callback(_svc->service_h, _messages_cb_incoming_mediator_cb, save, (void*)_svc);
		if (MSG_SUCCESS != ret)
		{
			return ERROR_CONVERT(ret);
		}
		_svc->cbs_enabled_cb = true;
	}

	_svc->cbs_incoming_cb = (void*)callback;
	_svc->cbs_incoming_cb_user_data = (void*)user_data;
	_svc->cbs_incoming_cb_save = save;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_unset_incoming_cb(messages_service_h service)
{
	messages_service_s *_svc = (messages_service_s *)service;

	CHECK_NULL(_svc);

	_svc->cbs_incoming_cb = NULL;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_set_etws_primary_noti_cb(messages_service_h service,
										 messages_cb_etws_primary_noti_cb callback, void *user_data)
{
	int ret;

	messages_service_s *_svc = (messages_service_s *)service;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(callback);

	if (!_svc->cbs_enabled_cb)
	{
		ret = msg_reg_cb_message_callback(_svc->service_h, _messages_cb_incoming_mediator_cb, _svc->cbs_incoming_cb_save, (void*)_svc);
		if (MSG_SUCCESS != ret)
		{
			return ERROR_CONVERT(ret);
		}
		_svc->cbs_enabled_cb = true;
	}

	_svc->etws_incoming_cb = (void*)callback;
	_svc->etws_incoming_cb_user_data = (void*)user_data;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_unset_etws_primary_noti_cb(messages_service_h service)
{
	messages_service_s *_svc = (messages_service_s *)service;

	CHECK_NULL(_svc);

	_svc->etws_incoming_cb = NULL;

	return MESSAGES_ERROR_NONE;
}

int _messages_cb_clear_settings(messages_cb_settings_s *settings)
{
	CHECK_NULL(settings);
	CHECK_NULL(settings->_struct);

	msg_release_struct(&settings->_struct);

	if (settings->channel_list)
	{
		g_slist_foreach(settings->channel_list, (GFunc)g_free, NULL);
		g_slist_free(settings->channel_list);
		settings->channel_list = NULL;
	}

	return MESSAGES_ERROR_NONE;
}

int messages_cb_load_settings(messages_service_h service, messages_cb_settings_h *settings)
{
	int i;
	int ret;

	messages_service_s *_svc = (messages_service_s *)service;
	messages_cb_settings_s * _settings;

	msg_struct_list_s *channels = NULL;

	messages_cb_channel_s *_ch;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(settings);

	// Allocate Settings
	_settings = (messages_cb_settings_s *)calloc(1, sizeof(messages_cb_settings_s));
	if (NULL == _settings)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create a '_settings'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	// Load Settings
	_settings->_struct = msg_create_struct(MSG_STRUCT_SETTING_CB_OPT);
	ret = msg_get_cb_opt(_svc->service_h, _settings->_struct);
	if (MSG_SUCCESS != ret)
	{
		msg_release_struct(&_settings->_struct);
		free(_settings);
		return ERROR_CONVERT(ret);
	}

	// Load Channel List
	msg_get_list_handle(_settings->_struct, MSG_CB_CHANNEL_LIST_STRUCT, (void **)&channels);
	for (i=0; i < channels->nCount; i++)
	{
		_ch = (messages_cb_channel_s *)calloc(1, sizeof(messages_cb_channel_s));
		if (NULL == _ch)
		{
			LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_ch'."
				, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
			_messages_cb_clear_settings(_settings);
			free(_settings);
			return MESSAGES_ERROR_OUT_OF_MEMORY;
		}

		msg_get_bool_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_ACTIVATE_BOOL, &_ch->activated);
		msg_get_int_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_ID_FROM_INT, &_ch->from_id);
		msg_get_int_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_ID_TO_INT, &_ch->to_id);
		msg_get_str_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_NAME_STR, _ch->name, CB_CHANNEL_NAME_MAX);

		_settings->channel_list = g_slist_append(_settings->channel_list, (messages_cb_channel_h)_ch);
	}

	if (NULL != _svc->cbs_settings_h)
	{
		_messages_cb_clear_settings((messages_cb_settings_s*)_svc->cbs_settings_h);
		free(_svc->cbs_settings_h);
	}
	_svc->cbs_settings_h = (messages_cb_settings_h)_settings;

	*settings = (messages_cb_settings_h)_settings;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_save_settings(messages_service_h service, messages_cb_settings_h settings)
{
	int i;
	int count;
	int ret;

	messages_service_s *_svc = (messages_service_s *)service;
	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	messages_cb_channel_s *_ch;
	msg_struct_list_s *channels = NULL;

	CHECK_NULL(_svc);
	CHECK_NULL(_svc->service_h);
	CHECK_NULL(_settings);
	CHECK_NULL(_settings->_struct);

	msg_get_list_handle(_settings->_struct, MSG_CB_CHANNEL_LIST_STRUCT, (void **)&channels);

	count = g_slist_length(_settings->channel_list);
	if (CB_CHANNEL_MAX < count)
	{
		count = CB_CHANNEL_MAX;
	}
	channels->nCount = count;

	for (i=0; i < channels->nCount; i++)
	{
		_ch = (messages_cb_channel_s *)g_slist_nth_data(_settings->channel_list, i);

		msg_set_bool_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_ACTIVATE_BOOL, _ch->activated);
		msg_set_int_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_ID_FROM_INT, _ch->from_id);
		msg_set_int_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_ID_TO_INT, _ch->to_id);
		msg_set_str_value(channels->msg_struct_info[i], MSG_CB_CHANNEL_NAME_STR, _ch->name, CB_CHANNEL_NAME_MAX);
	}

	ret = msg_set_cb_opt(_svc->service_h, _settings->_struct);

	return ERROR_CONVERT(ret);
}


int messages_cb_settings_set_enabled(messages_cb_settings_h settings, bool enabled)
{
	int ret;
	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	CHECK_NULL(_settings);
	CHECK_NULL(_settings->_struct);

	ret = msg_set_bool_value(_settings->_struct, MSG_CB_RECEIVE_BOOL, enabled);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	return MESSAGES_ERROR_NONE;
}

int messages_cb_settings_is_enabled(messages_cb_settings_h settings, bool *enabled)
{
	int ret;
	bool _enabled;
	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	CHECK_NULL(_settings);
	CHECK_NULL(_settings->_struct);
	CHECK_NULL(enabled);

	ret = msg_get_bool_value(_settings->_struct, MSG_CB_RECEIVE_BOOL, &_enabled);
	if (MSG_SUCCESS != ret)
	{
		return ERROR_CONVERT(ret);
	}

	*enabled = _enabled;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_settings_add_channel(messages_cb_settings_h settings, messages_cb_channel_h channel)
{
	int count;

	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	CHECK_NULL(_settings);
	CHECK_NULL(channel);

	count = g_slist_length(_settings->channel_list);
	if (CB_CHANNEL_MAX <= count)
	{
		LOGE("[%s] OUT_OF_RANGE(0x%08x) channel_list is full."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_RANGE;
	}

	_settings->channel_list = g_slist_append(_settings->channel_list, channel);

	return MESSAGES_ERROR_NONE;
}

int messages_cb_settings_remove_channel(messages_cb_settings_h settings, messages_cb_channel_h channel)
{
	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	CHECK_NULL(_settings);
	CHECK_NULL(channel);

	_settings->channel_list = g_slist_remove_all(_settings->channel_list, channel);

	return MESSAGES_ERROR_NONE;
}

int messages_cb_settings_get_channel(messages_cb_settings_h settings, int index, messages_cb_channel_h *channel)
{
	messages_cb_channel_h ch;
	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	CHECK_NULL(_settings);
	CHECK_NULL(channel);

	ch = (messages_cb_channel_h)g_slist_nth_data(_settings->channel_list, index);
	if (NULL == ch)
	{
		LOGE("[%s:%d] OPERATION_FAILED(0x%08x) : No channel data in the channel list.",
	   		__FUNCTION__, __LINE__, MESSAGES_ERROR_OPERATION_FAILED);
		return MESSAGES_ERROR_OPERATION_FAILED;
	}

	*channel = ch;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_settings_get_channel_count(messages_cb_settings_h settings, int *count)
{
	messages_cb_settings_s *_settings = (messages_cb_settings_s *)settings;

	CHECK_NULL(_settings);
	CHECK_NULL(count);

	*count = g_slist_length(_settings->channel_list);

	return MESSAGES_ERROR_NONE;
}

int messages_cb_create_channel(messages_cb_channel_h *channel)
{
	messages_cb_channel_s *_channel;

	CHECK_NULL(channel);

	_channel = (messages_cb_channel_s *)calloc(1, sizeof(messages_cb_channel_s));
	if (NULL == _channel)
	{
		LOGE("[%s] OUT_OF_MEMORY(0x%08x) fail to create '_channel'."
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY);
		return MESSAGES_ERROR_OUT_OF_MEMORY;
	}

	*channel = (messages_cb_channel_h)_channel;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_destroy_channel(messages_cb_channel_h channel)
{
	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);

	free(channel);

	return MESSAGES_ERROR_NONE;
}

int messages_cb_channel_set_id(messages_cb_channel_h channel, int from_id, int to_id)
{
	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);

	_channel->from_id = from_id;
	_channel->to_id = to_id;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_channel_get_id(messages_cb_channel_h channel, int *from_id, int *to_id)
{
	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);
	CHECK_NULL(from_id);
	CHECK_NULL(to_id);

	*from_id = _channel->from_id;
	*to_id = _channel->to_id;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_channel_set_name(messages_cb_channel_h channel, const char *name)
{
	int size;

	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);
	CHECK_NULL(name);

	size = strlen(name);

	if (CB_CHANNEL_NAME_MAX < size)
	{
		LOGE("[%s] OUT_OF_RANGE(0x%08x) channel name > %d"
			, __FUNCTION__, MESSAGES_ERROR_OUT_OF_MEMORY, CB_CHANNEL_NAME_MAX);
		return MESSAGES_ERROR_OUT_OF_RANGE;
	}

	strncpy(_channel->name, name, CB_CHANNEL_NAME_MAX);

	return MESSAGES_ERROR_NONE;
}

int messages_cb_channel_get_name(messages_cb_channel_h channel, char **name)
{
	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);
	CHECK_NULL(name);

	*name = strdup(_channel->name);

	return MESSAGES_ERROR_NONE;
}

int messages_cb_channel_set_activated(messages_cb_channel_h channel, bool activated)
{
	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);

	_channel->activated = activated;

	return MESSAGES_ERROR_NONE;
}

int messages_cb_channel_is_activated(messages_cb_channel_h channel, bool *activated)
{
	messages_cb_channel_s *_channel = (messages_cb_channel_s *)channel;

	CHECK_NULL(_channel);
	CHECK_NULL(activated);

	*activated = _channel->activated;

	return MESSAGES_ERROR_NONE;
}





