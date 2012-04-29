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


#include <messages_types.h>
#include <messages_error.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup TIZEN_MESSAGING_MODULE
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
 * @brief Gets the type of the message.
 *
 * @param[in] msg The message handle
 * @param[out] type The message type
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	messages_create_message()
 */
int messages_get_message_type(messages_message_h msg, messages_message_type_e *type);


/**
 * @brief Adds an recipient's address(phone number) to the message.
 * @details The maximum number of recipients per a message is 10.
 *
 * @param[in] msg The message handle
 * @param[in] address The recipient's address to receive a message \n
 * 		      The maximum length of @a address is 254.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	messages_get_address()
 * @see	messages_remove_all_addresses()
 */
int messages_add_address(messages_message_h msg, const char *address);


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
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY Out of memory
 *
 * @see	messages_add_address()
 * @see	messages_remove_all_addresses()
 */
int messages_get_address(messages_message_h msg, int index, char **address);


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
 * @brief Sends the message to all recipients.
 *
 * @remarks In order to check whether sending a message succeeds, 
 *      you should register messages_sent_cb() using messages_set_message_sent_cb().
 *
 * @param[in] service The message service handle
 * @param[in] msg The message handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_SENDING_FAILED Sending a message failed
 * @retval #MESSAGES_ERROR_OPERATION_FAILED Messaging operation failed
 *
 * @see messages_sent_cb()
 */
int messages_send_message(messages_service_h service, messages_message_h msg);


/**
 * @brief Searches for messages.
 *
 * @param[in] service The message service handle
 * @param[in] mbox The message box type
 * @param[in] type The message type \n
 * 		   If @a type is #MESSAGES_TYPE_UNKNOWN, all sms and mms messages are searched.
 * @param[in] keyword The keyword search in text and subject
 * @param[in] address The recepient address
 * @param[in] offset The start position (base 0)
 * @param[in] limit The maximum amount of messages to get (In case of 0, this method passes to thecallback all searched messages.)
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
int messages_foreach_message_from_db(messages_service_h service,
							 messages_message_box_e mbox,
							 messages_message_type_e type,
							 const char *keyword, const char *address,
							 int offset, int limit,
							 messages_search_cb callback, void *user_data);

/**
 * @brief Registers a callback to be invoked when a message is sent.
 *
 * @details You will be notified when sending a message finishes and check whether it succeeds using messages_sent_cb().
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
 * @post It will invoke messages_sent_cb().
 *
 * @see messages_unset_message_sent_cb()
 * @see messages_sent_cb()
 */
int messages_set_message_sent_cb(messages_service_h service, messages_sent_cb callback, void *user_data);


/**
 * @brief Unregisters the callback function.
 *
 * @param[in] service The message service handle
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #MESSAGES_ERROR_NONE Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @post It will invoke messages_sent_cb().
 *
 * @see messages_set_message_sent_cb()
 * @see messages_sent_cb()
 */
int messages_unset_message_sent_cb(messages_service_h service);


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
 * @addtogroup TIZEN_MESSAGING_MMS_MODULE
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
 * @}
 */



#ifdef __cplusplus
}
#endif


#endif /* __TIZEN_MESSAGING_H__ */
