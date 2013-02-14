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

#ifndef __TIZEN_MESSAGING_ERROR_H__
#define __TIZEN_MESSAGING_ERROR_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C"
{
#endif


/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_MODULE
 * @{
 */


/**
 * @brief Enumerations of error code for Messaging.
 */
typedef enum {
	MESSAGES_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
	MESSAGES_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
	MESSAGES_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	MESSAGES_ERROR_SERVER_NOT_READY = TIZEN_ERROR_MESSAGING_CLASS|0x501, /**< Server is not read */
	MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED = TIZEN_ERROR_MESSAGING_CLASS|0x502, /**< Communication with server failed */
	MESSAGES_ERROR_OUT_OF_RANGE = TIZEN_ERROR_MESSAGING_CLASS|0x503, /**< Index out of range */
	MESSAGES_ERROR_SENDING_FAILED = TIZEN_ERROR_MESSAGING_CLASS|0x504, /**< Sending a message failed */
	MESSAGES_ERROR_OPERATION_FAILED = TIZEN_ERROR_MESSAGING_CLASS|0x505, /**< Messaging operation failed */
	MESSAGES_ERROR_NO_SIM_CARD = TIZEN_ERROR_MESSAGING_CLASS|0x506, /**< No SIM Card */
	MESSAGES_ERROR_NO_DATA = TIZEN_ERROR_MESSAGING_CLASS|0x507, /**< No data available */
} messages_error_e;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MESSAGING_ERROR_H__ */
