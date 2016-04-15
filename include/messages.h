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
 * @brief Opens a handle for the messaging service.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @remarks You must release @a service using messages_close_service().
 *
 * @param[out] service The message service handle
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE                             Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER                Invalid parameter
 * @retval #MESSAGES_ERROR_SERVER_NOT_READY                 Server is not read
 * @retval #MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED Communication with server failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED                The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED                    Not supported
 *
 * @see	messages_close_service()
 */
int messages_open_service(messages_service_h *service);


/**
 * @brief Closes a handle for the messaging service.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @param[in] service The message service handle
 *
 * @return @c 0 on success,
 *        otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE                             Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER                Invalid parameter
 * @retval #MESSAGES_ERROR_COMMUNICATION_WITH_SERVER_FAILED Communication with server failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED                The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED                    Not supported
 *
 * @see	messages_open_service()
 */
int messages_close_service(messages_service_h service);


/**
 * @brief Creates a message handle.
 *
 * @since_tizen 2.3
 *
 * @remarks You must release @a msg using messages_destroy_message().
 *
 * @param[in] type  The message type (#MESSAGES_TYPE_SMS or #MESSAGES_TYPE_MMS) \n
 *                  If @a type is #MESSAGES_TYPE_UNKNOWN, #MESSAGES_ERROR_INVALID_PARAMETER occurs.
 * @param[out] msg  The message handle that is newly created if successful
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @see	messages_destroy_message()
 */
int messages_create_message(messages_message_type_e type, messages_message_h *msg);


/**
 * @brief Destroys a message handle and releases all its resources.
 *
 * @since_tizen 2.3
 *
 * @param[in] msg The message handle to destroy
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_create_message()
 */
int messages_destroy_message(messages_message_h msg);


/**
 * @brief Adds the message to message database.
 *
 * @since_tizen 3.0
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.write
 *
 * @param[in] service         The message service handle
 * @param[in] msg             The message handle
 * @param[out] msg_id         The message ID of added message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED  Messaging operation failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @see messages_create_message()
 *
 */
int messages_add_message(messages_service_h service, messages_message_h msg, int *msg_id);

/**
 * @brief Gets the message ID of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg     The message handle
 * @param[out] msg_id  The message ID
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED   Messaging operation failed
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see messages_search_message_by_id()
 */
int messages_get_message_id(messages_message_h msg, int *msg_id);


/**
 * @brief Sets the SIM ID of the sending message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg     The message handle
 * @param[in]  sim_id  The SIM id to send message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED   Messaging operation failed
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see messages_get_sim_id()
 */
int messages_set_sim_id(messages_message_h msg, int sim_id);


/**
 * @brief Gets the SIM ID of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg     The message handle
 * @param[out] sim_id  The SIM id of message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED   Messaging operation failed
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see messages_set_sim_id()
 */
int messages_get_sim_id(messages_message_h msg, int *sim_id);

/**
 * @brief Sets the message box type of the message.
 *
 * @since_tizen 3.0
 *
 * @param[in]  msg   The message handle
 * @param[in]  mbox  The message box type
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 */
int messages_set_mbox_type(messages_message_h msg, messages_message_box_e mbox);

/**
 * @brief Gets the message box type of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg   The message handle
 * @param[out] mbox  The message box type
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 */
int messages_get_mbox_type(messages_message_h msg, messages_message_box_e *mbox);

/**
 * @brief Gets the destination port of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg   The message handle
 * @param[out] port  The destination port of the message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see messages_add_sms_listening_port()
 */
int messages_get_message_port(messages_message_h msg, int *port);

/**
 * @brief Gets the type of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg  The message handle
 * @param[out] type The message type
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 */
int messages_get_message_type(messages_message_h msg, messages_message_type_e *type);


/**
 * @brief Adds an recipient's address(phone number) to the message.
 * @details The maximum number of recipients per a message is 10.
 *
 * @since_tizen 2.3
 *
 * @param[in] msg      The message handle
 * @param[in] address  The recipient's address to receive a message \n
 *                     The maximum length of @a address is @c 254.
 * @param[in] type     The recipient's type of the @a address
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_get_address()
 * @see	messages_remove_all_addresses()
 */
int messages_add_address(messages_message_h msg, const char *address, messages_recipient_type_e type);


/**
 * @brief Gets the total number of recipients in the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg    The message handle
 * @param[out] count  The total number of recipients
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_add_address()
 * @see	messages_remove_all_addresses()
 */
int messages_get_address_count(messages_message_h msg, int *count);


/**
 * @brief Gets a recipient's address with specified index.
 *
 * @since_tizen 2.3
 *
 * @remarks You must release @a address using free().
 *
 * @param[in]  msg      The message handle
 * @param[in]  index    The zero-based index of an address to receive a message.
 * @param[out] address  The recipient's address with specified index
 * @param[out] type     The recipient's type of the @a address
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_add_address()
 * @see	messages_remove_all_addresses()
 */
int messages_get_address(messages_message_h msg, int index, char **address, messages_recipient_type_e *type);


/**
 * @brief Removes all recipients in the message.
 *
 * @since_tizen 2.3
 *
 * @param[in] msg The message handle
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_add_address()
 * @see	messages_get_address()
 */
int messages_remove_all_addresses(messages_message_h msg);


/**
 * @brief Sets the text of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in] msg  The message handle
 * @param[in] text The text of the message \n
 *                 The maximum length of @a text is @c 1530.
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_get_text()
 */
int messages_set_text(messages_message_h msg, const char *text);


/**
 * @brief Gets the text of the message.
 *
 * @since_tizen 2.3
 *
 * @remarks You must release @a text using free().
 *
 * @param[in]  msg   The message handle
 * @param[out] text  The text of the message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY      Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 *
 * @see	messages_set_text()
 */
int messages_get_text(messages_message_h msg, char **text);

/**
 * @brief Sets the time of the message.
 *
 * @since_tizen 3.0
 *
 * @param[in]  msg   The message handle
 * @param[in]  time  The time of the message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 */
int messages_set_time(messages_message_h msg, time_t time);

/**
 * @brief Gets the time of the message.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg   The message handle
 * @param[out] time  The time of the message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE               Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED      Not supported
 */
int messages_get_time(messages_message_h msg, time_t *time);

/**
 * @brief Sends the message to all recipients.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.write
 *
 * @param[in] service         The message service handle
 * @param[in] msg             The message handle
 * @param[in] save_to_sentbox Set to @c true to save the message in the sentbox,
 *                            otherwise set to @c false to not save the message in the sentbox
 * @param[in] callback        The callback function
 * @param[in] user_data       The user data to be passed to the callback function
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_SENDING_FAILED    Sending a message failed
 * @retval #MESSAGES_ERROR_OPERATION_FAILED  Messaging operation failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @see messages_sent_cb()
 */
int messages_send_message(messages_service_h service, messages_message_h msg, bool save_to_sentbox, messages_sent_cb callback, void *user_data);

/**
 * @brief Gets the message count in the specific message box.
 * @since_tizen 2.3
 *
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @param[in]  service The message service handle
 * @param[in]  mbox    The message box type
 * @param[in]  type    The message type \n
 *                     If @a type is #MESSAGES_TYPE_UNKNOWN, all SMS and MMS messages are counted.
 * @param[out] count   The number of messages
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED  Messaging operation failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 */
int messages_get_message_count(messages_service_h service,
							messages_message_box_e mbox, messages_message_type_e type,
							int *count);



/**
 * @brief Searches for messages.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @remarks You must release @a message_array using messages_free_message_array().
 *
 * @param[in] service         The message service handle
 * @param[in] mbox            The message box type
 * @param[in] type            The message type \n
 *                            If @a type is #MESSAGES_TYPE_UNKNOWN, all SMS and MMS messages are searched.
 * @param[in] keyword         The keyword search in the text and subject
 * @param[in] address         The recipient address
 * @param[in] offset          The start position (base @c 0)
 * @param[in] limit           The maximum amount of messages to get \n
 *                    In case of @c 0, this method passes to the callback all searched messages.
 * @param[out] message_array  The array of the message handle
 * @param[out] length         The number of messages of the @a message_array
 * @param[out] total          The count of the messages that have been retrieved as a result without applying @a limit and @a offset\ n
 *                            The value can be used to calculate the total number of page views for the searched messages\ n
 *                            For example, if the count of message search is @c 50 and the limit is @c 20, then using this value, you can notice the total page is @c 3.
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_OPERATION_FAILED  Messaging operation failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
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
 * @brief Searches a message with the given message ID.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @remarks You must release @a msg using messages_destroy_message().
 *
 * @param[in]  service The message service handle
 * @param[in]  msg_id  The message ID
 * @param[out] msg     The message handle that is newly created if successful
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @see messages_get_message_id()
 */
int messages_search_message_by_id(messages_service_h service, int msg_id, messages_message_h *msg);


/**
 * @brief Frees the message array.
 *
 * @since_tizen 2.3
 *
 * @param[in] message_array The array of the message handle
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @see messages_search_message()
 */
int messages_free_message_array(messages_message_h *message_array);


/**
 * @brief Retrieves the searched messages by invoking the given callback function iteratively.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @param[in] service   The message service handle
 * @param[in] mbox      The message box type
 * @param[in] type      The message type \n
 *                      If @a type is #MESSAGES_TYPE_UNKNOWN, all SMS and MMS messages are searched.
 * @param[in] keyword   The keyword search in the text and subject
 * @param[in] address   The recipient address
 * @param[in] offset    The start position (base 0)
 * @param[in] limit     The maximum amount of messages to get \n
 *              In case of @c 0, this method passes to the callback all searched messages.
 * @param[in] callback  The callback function to get a message
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_OPERATION_FAILED  Messaging operation failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @post It invokes messages_search_cb().
 *
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
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @param[in] service   The message service handle
 * @param[in] callback  The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OPERATION_FAILED  Messaging operation failed
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
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
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.write
 *
 * @param[in] service The message service handle
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @see messages_set_message_incoming_cb()
 * @see messages_incoming_cb()
 */
int messages_unset_message_incoming_cb(messages_service_h service);

/**
 * @brief Adds an additional listening port for the incoming SMS messages.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @param[in] service The message service handle
 * @param[in] port    The listening port for the SMS messages
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
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
 * @since_tizen 2.3
 *
 * @param[in] msg     The message handle
 * @param[in] subject The subject of the message \n
 *                    The maximum length of @a subject is @c 120.
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_get_subject()
 */
int messages_mms_set_subject(messages_message_h msg, const char *subject);


/**
 * @brief Gets the subject of the message.
 *
 * @since_tizen 2.3
 *
 * @remarks You must release @a subject using free().
 *
 * @param[in]  msg     The message handle
 * @param[out] subject The subject of the message
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_set_subject()
 */
int messages_mms_get_subject(messages_message_h msg, char **subject);


/**
 * @brief Adds the attachment to the MMS message.
 *
 * @since_tizen 2.3
 *
 * @param[in] msg  The message handle
 * @param[in] type The attachment type
 * @param[in] path The file path to attach \n
 *                 The maximum length of @a path is @c 1024.
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_get_attachment()
 * @see	messages_mms_get_attachment_count()
 * @see	messages_mms_remove_all_attachments()
 */
int messages_mms_add_attachment(messages_message_h msg, messages_media_type_e type, const char *path);


/**
 * @brief Gets the file path of the attachment with the specified index.
 *
 * @since_tizen 2.3
 *
 * @remarks You must release @a path using free().
 *
 * @param[in]  msg   The message handle
 * @param[in]  index The zero-based index of the attachment
 * @param[out] type  The attachment type
 * @param[out] path  The file path to attach
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_OUT_OF_MEMORY     Out of memory
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_add_attachment()
 * @see	messages_mms_get_attachment_count()
 * @see	messages_mms_remove_all_attachments()
 */
int messages_mms_get_attachment(messages_message_h msg, int index, messages_media_type_e *type, char **path);


/**
 * @brief Gets the attachment with the specified index.
 *
 * @since_tizen 2.3
 *
 * @param[in]  msg   The message handle
 * @param[out] count The total number of attachments
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_add_attachment()
 * @see	messages_mms_get_attachment()
 * @see	messages_mms_remove_all_attachments()
 */
int messages_mms_get_attachment_count(messages_message_h msg, int *count);


/**
 * @brief Removes all attachments to the MMS message.
 *
 * @since_tizen 2.3
 *
 * @param[in] msg The message handle
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 * @pre @a msg is created as a #MESSAGES_TYPE_MMS.
 *
 * @see	messages_mms_add_attachment()
 * @see	messages_mms_get_attachment()
 * @see	messages_mms_get_attachment_count()
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
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.read
 *
 * @param[in] service   The message service handle
 * @param[in] app_id    The "X-WAP-Application-ID" to indicate a destination WAP Push application ID
 * @param[in] callback  The callback function
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 */
int messages_push_add_incoming_cb(messages_service_h service, const char *app_id,
					  messages_push_incoming_cb callback, void *user_data);


/**
 * @brief Unregisters the WAP push incoming callback function.
 *
 * @since_tizen 2.3
 *
 * @param[in] service The message service handle
 * @param[in] app_id  The "X-WAP-Application-ID" to indicate a destination WAP Push application ID
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 */
int messages_push_remove_incoming_cb(messages_service_h service, const char *app_id);


/**
 * @brief Registers an application to the mapping table of the WAP Push service.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.write
 *
 * @param[in] service      The message service handle
 * @param[in] content_type The MIME content type of the content
 * @param[in] app_id       The "X-WAP-Application-ID" to indicate a destination WAP Push application ID
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 */
int messages_push_register(messages_service_h service, const char *content_type, const char *app_id);


/**
 * @brief De-registers an application from the mapping table of the WAP Push service.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.write
 *
 * @param[in] service      The message service handle
 * @param[in] content_type The MIME content type of the content
 * @param[in] app_id       The "X-WAP-Application-ID" to indicate a destination WAP Push application ID
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 */
int messages_push_deregister(messages_service_h service, const char *content_type, const char *app_id);


/**
 * @brief Re-registers an application to the mapping table of the WAP Push service.
 *
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/message.write
 *
 * @param[in] service          The message service handle
 * @param[in] content_type     An element of the composite key for searching registered entry \n
 *                             The MIME content type of the content.
 * @param[in] app_id           The composite key for searching mapping information \n
 *                             The "X-WAP-Application-ID" to indicate a destination WAP Push application ID.
 * @param[in] dst_content_type The MIME content type of the content
 * @param[in] dst_app_id       The "X-WAP-Application-ID" to indicate a destination WAP Push application ID
 *
 * @return @c 0 on success,
 *       otherwise a negative error value
 *
 * @retval #MESSAGES_ERROR_NONE              Successful
 * @retval #MESSAGES_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #MESSAGES_ERROR_PERMISSION_DENIED The application does not have the privilege to call this method
 * @retval #MESSAGES_ERROR_NOT_SUPPORTED     Not supported
 *
 */
int messages_push_reregister(messages_service_h service, const char *content_type, const char *app_id,
							 const char *dst_content_type, const char *dst_app_id);

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
