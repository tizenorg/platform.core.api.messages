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

#ifndef __TIZEN_MESSAGING_TYPES_H__
#define __TIZEN_MESSAGING_TYPES_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_MODULE
 * @{
 */

/**
 * @brief The messaging service handle.
 */
typedef struct messages_service_s *messages_service_h;


/**
 * @brief The message handle.
 */
typedef struct messages_message_s *messages_message_h;

/**
 * @brief The message box type.
 */
typedef enum {
	MESSAGES_MBOX_ALL = 0, /**< All message box type */
	MESSAGES_MBOX_INBOX = 1, /**< Inbox type */
	MESSAGES_MBOX_OUTBOX = 2, /**< Outbox type*/
	MESSAGES_MBOX_SENTBOX = 3, /**< Sentbox type */
	MESSAGES_MBOX_DRAFT = 4, /**< Draft type */
} messages_message_box_e;


/**
 * @brief The message type.
 */
typedef enum {
	MESSAGES_TYPE_UNKNOWN = 0, /**< Unknown type */
	MESSAGES_TYPE_SMS = 1, /**< SMS type */
	MESSAGES_TYPE_MMS = 2, /**< MMS type */
} messages_message_type_e;


/**
 * @brief The attachment type for MMS messaging.
 */
typedef enum {
	MESSAGES_MEDIA_UNKNOWN = 0, /**< Unknown */
	MESSAGES_MEDIA_IMAGE = 1, /**< The image */
	MESSAGES_MEDIA_AUDIO = 2, /**< The audio */
	MESSAGES_MEDIA_VIDEO = 3, /**< The video */
} messages_media_type_e;

/**
 * @brief The recipient type of a message.
 */
typedef enum {
	MESSAGES_RECIPIENT_UNKNOWN = 0,
	MESSAGES_RECIPIENT_TO = 1,
	MESSAGES_RECIPIENT_CC = 2,
	MESSAGES_RECIPIENT_BCC = 3,
} messages_recipient_type_e;

/**
 * @brief The result of sending a message.
 */
typedef enum {
	MESSAGES_SENDING_FAILED = -1, /**< Message sending is failed */
	MESSAGES_SENDING_SUCCEEDED = 0, /**< Message sending is succeeded */
} messages_sending_result_e;


/**
 * @brief Called when the process of sending a message to all recipients finishes. 
 *
 * @param[in] result The result of message sending.
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre messages_send_message() will invoke this callback if you register this callback using messages_set_message_sent_cb(). 
 *
 * @see messages_set_message_sent_cb()
 * @see messages_unset_message_sent_cb()
 * @see messages_send_message()
 */
typedef void (* messages_sent_cb)(messages_sending_result_e result, void *user_data);


/**
 * @brief Called when an incoming message is received.
 *
 * @param[in] incoming_msg An incoming message
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre You register this callback using messages_set_message_incoming_cb() for getting an incoming message.
 *
 * @see messages_set_message_incoming_cb()
 * @see messages_unset_message_incoming_cb()
 */
typedef void (* messages_incoming_cb)(messages_message_h incoming_msg, void *user_data);


/**
 * @brief Called when a message is retrieved from a search request.
 *
 * @remark You should not call messages_destroy_message() with @a msg.
 *
 * @param[in] msg The message handle (It can be NULL if nothing is found)
 * @param[in] user_data The user data passed from the foreach function 
 * @param[in] index The index of a message from the messages that have been retrieved as a search result
 * @param[in] result_count The count of the messages that have been retrieved as a result applying @a limit and @a offset.\n
 *                     If the search has a @a limit, then this value is always equal or less than the limit.
 * @param[in] total_count The count of the messages that have been retrieved as a result without applying @a limit and @a offset. \n
 *                     The value can be used to calculate the total number of page views for the searched meessages.\n
 *                     For example, if the count of message search is 50 and the limit is 20, then using this value, you can notice the total page is 3.
 *
 * @return @c true to continue with the next iteration of the loop or return @c false to break out of the loop.
 *
 * @pre messages_foreach_message() will invoke this callback function.
 *
 * @see messages_foreach_message()
 */
typedef bool (* messages_search_cb)(messages_message_h msg, int index, int result_count, int total_count, void *user_data);


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MESSAGING_TYPES_H__ */
