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

/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_MODULE
 * @{
 */

/**
 * @brief The messaging service handle.
 * @since_tizen 2.3
 */
typedef struct messages_service_s *messages_service_h;


/**
 * @brief The message handle.
 * @since_tizen 2.3
 */
typedef struct messages_message_s *messages_message_h;

/**
 * @brief Enumeration for the message box type.
 * @since_tizen 2.3
 */
typedef enum {
	MESSAGES_MBOX_ALL = 0,      /**< All message box type */
	MESSAGES_MBOX_INBOX = 1,    /**< Inbox type */
	MESSAGES_MBOX_OUTBOX = 2,   /**< Outbox type*/
	MESSAGES_MBOX_SENTBOX = 3,  /**< Sentbox type */
	MESSAGES_MBOX_DRAFT = 4,    /**< Draft type */
} messages_message_box_e;


/**
 * @brief Enumeration for the message type.
 * @since_tizen 2.3
 */
typedef enum {
	MESSAGES_TYPE_UNKNOWN = 0,                                        /**< Unknown type */
	MESSAGES_TYPE_SMS = 1,                                            /**< SMS type */
	MESSAGES_TYPE_MMS = 2,                                            /**< MMS type */
	MESSAGES_TYPE_SMS_CB   = MESSAGES_TYPE_SMS | 1 << 4,              /**< CB(Cell Broadcast) Type */
	MESSAGES_TYPE_SMS_PUSH = MESSAGES_TYPE_SMS | 10 << 4,             /**< WAP Push Type */
	MESSAGES_TYPE_SMS_ETWS_PRIMARY = MESSAGES_TYPE_SMS | 21 << 4,     /**< ETWS(Earthquake and Tsunami Warning System) Primary Type */
	MESSAGES_TYPE_SMS_ETWS_SECONDARY = MESSAGES_TYPE_SMS | 22 << 4,   /**< ETWS(Earthquake and Tsunami Warning System) Secondary Type */
} messages_message_type_e;


/**
 * @brief Enumeration for the attachment type for MMS messaging.
 * @since_tizen 2.3
 */
typedef enum {
	MESSAGES_MEDIA_UNKNOWN = 0,  /**< Unknown */
	MESSAGES_MEDIA_IMAGE = 1,    /**< The image */
	MESSAGES_MEDIA_AUDIO = 2,    /**< The audio */
	MESSAGES_MEDIA_VIDEO = 3,    /**< The video */
} messages_media_type_e;

/**
 * @brief Enumeration for the recipient type of a message.
 * @since_tizen 2.3
 */
typedef enum {
	MESSAGES_RECIPIENT_UNKNOWN = 0, /**< Unknown */
	MESSAGES_RECIPIENT_TO = 1,      /**< The 'To' recipient  */
	MESSAGES_RECIPIENT_CC = 2,      /**< The 'Cc' (carbon copy) recipient */
	MESSAGES_RECIPIENT_BCC = 3,     /**< The 'Bcc' (blind carbon copy) recipient */
} messages_recipient_type_e;

/**
 * @brief Enumeration for the result of sending a message.
 * @since_tizen 2.3
 */
typedef enum {
	MESSAGES_SENDING_FAILED = -1,   /**< Message sending failed */
	MESSAGES_SENDING_SUCCEEDED = 0, /**< Message sending succeeded */
} messages_sending_result_e;

/**
 * @brief Enumeration for the SIM index of a message.
 * @since_tizen 2.3
 */
typedef enum {
	MESSAGES_SIM_INDEX_UNKNOWN = 0,	/**< Unknown SIM Index*/
	MESSAGES_SIM_INDEX_1 = 1,		/**< SIM Index 1 */
	MESSAGES_SIM_INDEX_2 = 2,		/**< SIM Index 2 */
} messages_sim_index_e;


/**
 * @brief Called when the process of sending a message to all recipients finishes.
 * @since_tizen 2.3
 *
 * @param[in] result     The result of message sending
 * @param[in] user_data  The user data passed from the callback registration function
 *
 * @pre messages_send_message() will invoke this callback function.
 *
 * @see messages_send_message()
 */
typedef void (* messages_sent_cb)(messages_sending_result_e result, void *user_data);


/**
 * @brief Called when an incoming message is received.
 * @since_tizen 2.3
 *
 * @param[in] incoming_msg  The incoming message
 * @param[in] user_data     The user data passed from the callback registration function
 *
 * @pre You register this callback using messages_set_message_incoming_cb() for getting an incoming message.
 *
 * @see messages_set_message_incoming_cb()
 * @see messages_unset_message_incoming_cb()
 */
typedef void (* messages_incoming_cb)(messages_message_h incoming_msg, void *user_data);


/**
 * @brief Called when a message is retrieved from a search request.
 * @since_tizen 2.3
 *
 * @remarks You should not call messages_destroy_message() with @a msg.
 *
 * @param[in] msg          The message handle \n
 *                         It can be @c NULL if nothing is found.
 * @param[in] user_data    The user data passed from the foreach function
 * @param[in] index        The index of a message from the messages that have been retrieved as a search result
 * @param[in] result_count The count of the messages that have been retrieved as a result applying @a limit and @a offset \n
 *                         If the search has a @a limit, then this value is always equal or less than the limit.
 * @param[in] total_count  The count of the messages that have been retrieved as a result without applying @a limit and @a offset \n
 *                         The value can be used to calculate the total number of page views for the searched messages. \n
 *                          For example, if the count of message search is @c 50 and the limit is @c 20, then using this value, you can notice the total page is @c 3.
 *
 * @return @c true to continue with the next iteration of the loop,
 *         otherwise return @c false to break out of the loop
 *
 * @pre messages_foreach_message() will invoke this callback function.
 *
 * @see messages_foreach_message()
 */
typedef bool (* messages_search_cb)(messages_message_h msg, int index, int result_count, int total_count, void *user_data);


 /**
 * @addtogroup CAPI_MESSAGING_MESSAGES_PUSH_MODULE
 * @{
 */

/**
 * @brief Called when an incoming push message is received.
 * @since_tizen 2.3
 *
 * @param[in] header      The header of the push message
 * @param[in] body        The body of the push message \n
 *                        It is not a null-terminated string.
 * @param[in] body_length The length of @a body
 * @param[in] user_data   The user data to be passed to the callback function
 *
 * @pre You register this callback using messages_push_set_incoming_cb() for getting an incoming message.
 *
 * @see messages_push_set_incoming_cb()
 * @see messages_push_unset_incoming_cb()
 */
typedef void (* messages_push_incoming_cb)(const char *header, const char *body, int body_length, void *user_data);

/**
 * @}
 */


/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MESSAGING_TYPES_H__ */
