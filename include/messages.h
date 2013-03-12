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

#ifndef __TIZEN_MESSAGING_H__
#define __TIZEN_MESSAGING_H__


#include <time.h>
#include <messages_types.h>
#include <messages_error.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * @file messages.h
 * @brief This file contains the messages API.
 */


/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_MODULE
 * @{
 */


/**
 * @brief Opens a handle for messaging service.
 *
 * @remark @a service must be released with messages_close_service() by you.
 *
 * @param[out] service The message service handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_SERVER_NOT_READY Server is not read
 * @retval #MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED Communication with server failed
 *
 * @see	messages_close_service()
 */
int messages_open_service(messages_service_h *service);


/**
 * @brief Closes a handle for messaging service.
 *
 * @param[in] service The message service handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED Communication with server failed
 *
 * @see	messages_open_service()
 */
int messages_close_service(messages_service_h service);


/**
 * @brief Creates a message handle.
 *
 * @remark @a msg must be released with messages_destroy_message() by you.
 *
 * @param[in] type A message type (MESSAGES_TYPE_SMS or MESSAGES_TYPE_MMS) \n
 * 				   If @a type is #MESSAGES_TYPE_UNKNOWN, #MESSAGES_ERROR_INVALID_PARAMETER occurs.
 * @param[out] msg A message handle to be newly created if successful
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see	messages_destroy_message()
 */
int messages_create_message(messages_message_type_e type, messages_message_h *msg);


/**
 * @brief Destroys a message handle and release all its resources.
 *
 * @param[in] msg A message handle to destroy
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	messages_create_message()
 */
int messages_destroy_message(messages_message_h msg);

/**
 * @brief Gets the message id of the message.
 *
 * @param[in] msg The message handle
 * @param[out] msg_id The message id
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 *
 * @see messages_search_message_by_id()
 */
int messages_get_message_id(messages_message_h msg, int *msg_id);

/**
 * @brief Gets the message box type of the message.
 *
 * @param[in] msg The message handle
 * @param[out] mbox The message box type
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_get_mbox_type(messages_message_h msg, messages_message_box_e *mbox);

/**
 * @brief Gets the destination port of the message.
 *
 * @param[in] msg The message handle
 * @param[out] port The destination port of the message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see messages_add_sms_listening_port()
 */
int messages_get_message_port(messages_message_h msg, int *port);

/**
 * @brief Gets the type of the message.
 *
 * @param[in] msg The message handle
 * @param[out] type The message type
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_get_message_type(messages_message_h msg, messages_message_type_e *type);


/**
 * @brief Adds an recipient's address(phone number) to the message.
 * @details The maximum number of recipients per a message is 10.
 *
 * @param[in] msg The message handle
 * @param[in] address The recipient's address to receive a message \n
 * 		      The maximum length of @a address is 254.
 * @param[in] type The recipient's type of the @a address
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	messages_get_address()
 * @see	messages_remove_all_addresses()
 */
int messages_add_address(messages_message_h msg, const char *address, messages_recipient_type_e type);


/**
 * @brief Gets the total number of recipients in the message.
 *
 * @param[in] msg The message handle
 * @param[out] count The total number of recipients
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	messages_add_address()
 * @see	messages_remove_all_addresses()
 */
int messages_get_address_count(messages_message_h msg, int *count);


/**
 * @brief Gets a recipient's address with specified index.
 *
 * @remarks @a address must be released with @c free() by you.
 *
 * @param[in] msg The message handle
 * @param[in] index The zero-based index of address to receive a message.
 * @param[out] address The recipient's address with specified index
 * @param[out] type The recipient's type of the @a address.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see	messages_add_address()
 * @see	messages_remove_all_addresses()
 */
int messages_get_address(messages_message_h msg, int index, char **address, messages_recipient_type_e *type);


/**
 * @brief Removes all recipients in the message.
 *
 * @param[in] msg The message handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	messages_add_address()
 * @see	messages_get_address()
 */
int messages_remove_all_addresses(messages_message_h msg);


/**
 * @brief Sets the text of the message.
 *
 * @param[in] msg The message handle
 * @param[in] text The text of the message \n
 * 		   The maximum length of @a text is 1530.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see	messages_get_text()
 */
int messages_set_text(messages_message_h msg, const char *text);


/**
 * @brief Gets the text of the message.
 *
 * @remarks @a text must be released with @c free() by you.
 *
 * @param[in] msg The message handle
 * @param[out] text The text of the message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see	messages_set_text()
 */
int messages_get_text(messages_message_h msg, char **text);


/**
 * @brief Gets the time of the message.
 *
 * @param[in] msg The message handle
 * @param[out] time The time of the message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_get_time(messages_message_h msg, time_t *time);

/**
 * @brief Sends the message to all recipients.
 *
 * @remarks In order to check whether sending a message succeeds, 
 *      you should register messages_sent_cb() using messages_set_message_sent_cb().
 *
 * @param[in] service The message service handle
 * @param[in] msg The message handle
 * @param[in] save_to_sentbox Set to true to save the message in the sentbox, else false
 * @param[in] callback The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_SENDING_FAILED Sending a message failed
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 *
 * @see messages_sent_cb()
 */
int messages_send_message(messages_service_h service, messages_message_h msg, bool save_to_sentbox, messages_sent_cb callback, void *user_data);

/**
 * @brief Gets the message count in the specific message box
 *
 * @param[in] service The message service handle
 * @param[in] mbox The message box type
 * @param[in] type The message type \n
 *			If @a type is #MESSAGES_TYPE_UNKNOWN, all sms and mms messages are counted.
 * @param[out] count The number of messages
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 */
int messages_get_message_count(messages_service_h service, 
							messages_message_box_e mbox, messages_message_type_e type,
							int *count);



/**
 * @brief Searches for messages.
 *
 * @details @a message_array must be released with messages_free_message_array() by you.
 *
 * @param[in] service The message service handle
 * @param[in] mbox The message box type
 * @param[in] type The message type \n
 * 		   If @a type is #MESSAGES_TYPE_UNKNOWN, all sms and mms messages are searched.
 * @param[in] keyword The keyword search in text and subject
 * @param[in] address The recipient address
 * @param[in] offset The start position (base 0)
 * @param[in] limit The maximum amount of messages to get (In case of 0, this method passes to the callback all searched messages.)
 * @param[out] message_array The array of the message handle
 * @param[out] length The number of messages of the message_array
 * @param[out] total The count of the messages that have been retrieved as a result without applying @a limit and @a offset. \n
 *                     The value can be used to calculate the total number of page views for the searched messages.\n
 *                     For example, if the count of message search is 50 and the limit is 20, then using this value, you can notice the total page is 3.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 *
 * @see messages_free_message_array()
 */
int messages_search_message(messages_service_h service,
							messages_message_box_e mbox,
							messages_message_type_e type,
							const char *keyword, const char *address,
							int offset, int limit,
							messages_message_h **message_array, int *length, int *total);
							
/**
 * @brief Searches a message with the given message id.
 *
 * @remark @a msg must be released with messages_destroy_message() by you.
 *
 * @param[in] service The message service handle
 * @param[in] msg_id The message id
 * @param[out] msg A message handle to be newly created if successful
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see messages_get_message_id()
 */
int messages_search_message_by_id(messages_service_h service, int msg_id, messages_message_h *msg);


/**
 * @brief Frees message array.
 *
 * @param[in] message_array The array of the message handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see messages_search_message()
 */
int messages_free_message_array(messages_message_h *message_array);


/**
 * @brief Retrieves the searched messages by invoking the given callback function iteratively.
 *
 * @param[in] service The message service handle
 * @param[in] mbox The message box type
 * @param[in] type The message type \n
 * 		   If @a type is #MESSAGES_TYPE_UNKNOWN, all sms and mms messages are searched.
 * @param[in] keyword The keyword search in text and subject
 * @param[in] address The recipient address
 * @param[in] offset The start position (base 0)
 * @param[in] limit The maximum amount of messages to get (In case of 0, this method passes to the callback all searched messages.)
 * @param[in] callback The callback function to get a message
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 *
 * @post It invokes messages_search_cb().
 * @see messages_search_cb()
 */
int messages_foreach_message(messages_service_h service,
							 messages_message_box_e mbox,
							 messages_message_type_e type,
							 const char *keyword, const char *address,
							 int offset, int limit,
							 messages_search_cb callback, void *user_data);

/**
 * @brief Registers a callback to be invoked when an incoming message is received.
 *
 * @param[in] service The message service handle
 * @param[in] callback The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 *
 * @post It will invoke messages_incoming_cb().
 *
 * @see messages_unset_message_incoming_cb()
 * @see messages_incoming_cb()
 */
int messages_set_message_incoming_cb(messages_service_h service, messages_incoming_cb callback, void *user_data);


/**
 * @brief Unregisters the callback function.
 *
 * @param[in] service The message service handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see messages_set_message_incoming_cb()
 * @see messages_incoming_cb()
 */
int messages_unset_message_incoming_cb(messages_service_h service);

/**
 * @brief Adds an additional listening port for the incoming SMS messages.
 *
 * @param[in] service The message service handle
 * @param[in] port The listening port for the SMS messages
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see messages_set_message_incoming_cb()
 * @see messages_get_message_port()
 */
int messages_add_sms_listening_port(messages_service_h service, int port);

/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_MMS_MODULE
 * @{
 */
/**
 * @brief Sets the subject of the message.
 *
 * @param[in] msg The message handle
 * @param[in] subject The subject of the message \n
 * 		      The maximum length of @a subject is 120.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_get_subject()
 */
int messages_mms_set_subject(messages_message_h msg, const char *subject);


/**
 * @brief Gets the subject of the message.
 *
 * @remarks @a subject must be released with @c free() by you.
 *
 * @param[in] msg The message handle
 * @param[out] subject The subject of the message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_set_subject()
 */
int messages_mms_get_subject(messages_message_h msg, char **subject);


/**
 * @brief Adds the attachment to the MMS message.
 *
 * @param[in] msg The message handle
 * @param[in] type The attachment type
 * @param[in] path The file path to attach \n
 * 	           The maximum length of @a path is 1024.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_get_attachment()
 * @see	messages_mms_get_attachment_count()
 * @see messages_mms_remove_all_attachments()
 */
int messages_mms_add_attachment(messages_message_h msg, messages_media_type_e type, const char *path);


/**
 * @brief Gets the file path of the attachment with the specified index.
 *
 * @remark @a path must be released with @c free() by you.
 *
 * @param[in] msg The message handle
 * @param[in] index The zero-based index of attachment
 * @param[out] type The attachment type
 * @param[out] path The file path to attach
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_add_attachment()
 * @see	messages_mms_get_attachment_count()
 * @see messages_mms_remove_all_attachments()
 */
int messages_mms_get_attachment(messages_message_h msg, int index, messages_media_type_e *type, char **path);


/**
 * @brief Gets the attachment with the specified index.
 *
 * @param[in] msg The message handle
 * @param[out] count The total number of attachments
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_add_attachment()
 * @see	messages_mms_get_attachment()
 * @see messages_mms_remove_all_attachments()
 */
int messages_mms_get_attachment_count(messages_message_h msg, int *count);


/**
 * @brief Removes all attachments to the MMS message.
 *
 * @param[in] msg The message handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_add_attachment()
 * @see	messages_mms_get_attachment()
 * @see messages_mms_get_attachment_count()
 */
int messages_mms_remove_all_attachments(messages_message_h msg);
/**
 * @}
 */

/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_PUSH_MODULE
 * @{
 */

/**
 * @brief Registers a callback to be invoked when an WAP Push message is received.
 *
 * @param[in] service The message service handle
 * @param[in] app_id The "X-WAP-Application-ID" to indicate a destination WAP Push application id.
 * @param[in] callback The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_push_add_incoming_cb(messages_service_h service, const char *app_id, 
					  messages_push_incoming_cb callback, void *user_data);


/**
 * @brief Unregisters the WAP push incoming callback function.
 *
 * @param[in] service The message service handle
 * @param[in] app_id The "X-WAP-Application-ID" to indicate a destination WAP Push application id
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_push_remove_incoming_cb(messages_service_h service, const char *app_id);


/**
 * @brief Registers an application to the mapping table of the WAP Push service.
 *
 * @param[in] service The message service handle
 * @param[in] content_type The MIME content type of the content
 * @param[in] app_id The "X-WAP-Application-ID" to indicate a destination WAP Push application id
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_push_register(messages_service_h service, const char *content_type, const char *app_id);


/**
 * @brief Deregisters an application from the mapping table of the WAP Push service.
 *
 * @param[in] service The message service handle
 * @param[in] content_type The MIME content type of the content
 * @param[in] app_id The "X-WAP-Application-ID" to indicate a destination WAP Push application id
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_push_deregister(messages_service_h service, const char *content_type, const char *app_id);


/**
 * @brief Re-registers an application to the mapping table of the WAP Push service.
 *
 * @param[in] service The message service handle
 * @param[in] content_type An element of the composite key for searching registered entry.\n
 *                         The MIME content type of the content.
 * @param[in] app_id The The composite key for searching mapping information.\n
 *                   "X-WAP-Application-ID" to indicate a destination WAP Push application id.
 * @param[in] dst_content_type The MIME content type of the content.
 * @param[in] dst_app_id The "X-WAP-Application-ID" to indicate a destination WAP Push application id.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_push_reregister(messages_service_h service, const char *content_type, const char *app_id,
							 const char *dst_content_type, const char *dst_app_id);

/**
 * @}
 */


/**
 * @addtogroup CAPI_MESSAGING_MESSAGES_CB_MODULE
 * @{
 */

/**
 * @brief Gets the message id of the CB message.
 *
 * @param[in] msg The CB message handle
 * @param[out] msg_id The message id of the CB message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_get_message_id(messages_cb_message_h msg, int *msg_id);

/**
 * @brief Gets the serial number of the CB message.
 *
 * @param[in] msg The CB message handle
 * @param[out] serial The serial number of the CB message (16-bit integer)
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_get_serial_number(messages_cb_message_h msg, int *serial);

/**
 * @brief Gets the DCS(Data Coding Scheme) value of the CB message.
 *
 * @param[in] msg The CB message handle
 * @param[out] dcs The DCS value of the CB message (8-bit integer)
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_get_dcs(messages_cb_message_h msg, int *dcs);

/**
 * @brief Gets the message type of the CB message.
 *
 * @param[in] msg The CB message handle
 * @param[out] type The message type of the CB message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_get_message_type(messages_cb_message_h msg, messages_message_type_e *type);


/**
 * @brief Gets the language type of the CB message.
 *
 * @remarks @a type must be released with @c free() by you.
 *
 * @param[in] msg The CB message handle
 * @param[out] text The language type of the CB message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 */
int messages_cb_get_language_type(messages_cb_message_h msg, char **type);

/**
 * @brief Gets the text contents of the CB message.
 *
 * @remarks @a text must be released with @c free() by you.
 *
 * @param[in] msg The CB message handle
 * @param[out] text The the text of the CB message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED if the type of @a msg is MESSAGES_CB_TYPE_ETWS_PRIMARY
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 */
int messages_cb_get_text(messages_cb_message_h msg, char **text);


/**
 * @brief Gets the received time of the CB message.
 *
 * @param[in] msg The CB message handle
 * @param[out] time The recieved time of the CB message
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_get_time(messages_cb_message_h msg, time_t *time);


/**
 * @brief Registers a callback to be invoked when an CB message is received.
 *
 * @param[in] service The message service handle
 * @param[in] save Set to true to save the message, else false.
 * @param[in] callback The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_set_incoming_cb(messages_service_h service, bool save,
								messages_cb_incoming_cb callback, void *user_data);


/**
 * @brief Unregisters the callback function for cb incoming message.
 *
 * @param[in] service The message service handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_unset_incoming_cb(messages_service_h service);

/**
 * @brief Registers a callback to be invoked when an ETWS primary notification message is received.
 *
 * @param[in] service The message service handle
 * @param[in] callback The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_set_etws_primary_noti_cb(messages_service_h service,
										 messages_cb_etws_primary_noti_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function for ETWS primary notification message is received.
 *
 * @param[in] service The message service handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_unset_etws_primary_noti_cb(messages_service_h service);


/**
 * @brief Loads the settings for CB messaging.
 *
 * @remark The @a settings will be free automatically when the @a service is destroyed.
 *
 * @param[in] service The message service handle
 * @param[out] settings The settings handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 */
int messages_cb_load_settings(messages_service_h service, messages_cb_settings_h *settings);

/**
 * @brief Saves the settings for CB messaging.
 *
 * @param[in] service The message service handle
 * @param[in] settings The settings handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_save_settings(messages_service_h service, messages_cb_settings_h settings);

/**
 * @brief Enables or disables the CB messaging.
 *
 * @param[in] settings The settings handle
 * @param[in] enabled If true, the CB messaging is enabled. otherwise the CB messaging is disabled.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_settings_set_enabled(messages_cb_settings_h settings, bool enabled);

/**
 * @brief Determines whether the CB messaging is enabled.
 *
 * @param[in] settings The settings handle
 * @param[out] enabled true if the CB messaging is enabled, false otherwise.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_settings_is_enabled(messages_cb_settings_h settings, bool *enabled);

/**
 * @brief Adds a channel to the CB messaging settings.
 *
 * @param[in] settings The settings handle
 * @param[in] channel The channel handle to add
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_RANGE Maximum cb channels exceeded
 */
int messages_cb_settings_add_channel(messages_cb_settings_h settings, messages_cb_channel_h channel);

/**
 * @brief Removes a channel from the CB messaging settings.
 *
 * @param[in] settings The settings handle
 * @param[in] channel The channel handle to remove
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_settings_remove_channel(messages_cb_settings_h settings, messages_cb_channel_h channel);

/**
 * @brief Gets a channel from the CB messaging settings.
 *
 * @param[in] settings The settings handle
 * @param[in] index The zero-based index of channel to receive 
 * @param[out] channel The channel with specified index
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 */
int messages_cb_settings_get_channel(messages_cb_settings_h settings, int index, messages_cb_channel_h *channel);

/**
 * @brief Gets the total number of channels in the settings.
 *
 * @param[in] settings The settings handle
 * @param[out] count The total number of channels
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_settings_get_channel_count(messages_cb_settings_h settings, int *count);

/**
 * @brief Creates a channel handle.
 *
 * @remark @a channel must be released with messages_cb_destroy_channel() by you.
 *
 * @param[out] channel A message handle to be newly created if successful
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 */
int messages_cb_create_channel(messages_cb_channel_h *channel);

/**
 * @brief Destroys a channel handle and release all its resource.
 *
 * @param[in] channel A channel handle to destroy
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_destroy_channel(messages_cb_channel_h channel);

/**
 * @brief Sets the range of channel id values for the CB messaging.
 *
 * @param[in] channel The channel handle
 * @param[in] from_id The start of the range of channel id values.
 * @param[in] to_id The end of the range of channel id values.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_channel_set_id(messages_cb_channel_h channel, int from_id, int to_id);

/**
 * @brief Gets the range of channel id values for the CB messaging.
 *
 * @param[in] channel The channel handle
 * @param[out] from_id The start of the range of channel id values.
 * @param[out] to_id The end of the range of channel id values.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_channel_get_id(messages_cb_channel_h channel, int *from_id, int *to_id);

/**
 * @brief Sets the name of the channel.
 *
 * @param[in] channel The channel handle
 * @param[in] name The name of the channel
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_RANGE Maximum length of name exceeded
 */
int messages_cb_channel_set_name(messages_cb_channel_h channel, const char *name);

/**
 * @brief Gets the name of the channel.
 *
 * @param[in] channel The channel handle
 * @param[out] name The name of the channel
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_channel_get_name(messages_cb_channel_h channel, char **name);

/**
 * @brief Activates or deactivates a specific channel.
 *
 * @param[in] channel The channel handle
 * @param[in] activated If true, the channel is activated. otherwise, the channel is deactivated.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_channel_set_activated(messages_cb_channel_h channel, bool activated);

/**
 * @brief Determines whether the channel is activated.
 *
 * @param[in] channel The channel handle
 * @param[out] activated true If the channel is activated, false otherwise.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 */
int messages_cb_channel_is_activated(messages_cb_channel_h channel, bool *activated);
 
 
 
/**
 * @}
 */



#ifdef __cplusplus
}
#endif


#endif /* __TIZEN_MESSAGING_H__ */
