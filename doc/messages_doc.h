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


#ifndef __TIZEN_MESSAGES_DOC_H__
#define __TIZEN_MESSAGES_DOC_H__

/**
 * @defgroup CAPI_MESSAGING_MESSAGES_MODULE Messages
 * @brief The @ref CAPI_MESSAGING_MESSAGES_MODULE API provides functions to create, set properties and send a SMS/MMS message.
 * @ingroup CAPI_MESSAGING_FRAMEWORK
 *
 * @addtogroup CAPI_MESSAGING_MESSAGES_MODULE
 *
 * @section CAPI_MESSAGING_MESSAGES_MODULE_HEADER Required Header
 *   \#include <messages.h>
 *
 * @section CAPI_MESSAGING_MESSAGES_MODULE_OVERVIEW Overview
 *
 * The Messages API provides the following functionalities:
 *
 * - Creating a message
 * - Setting the text of the message
 * - Setting the message recipient(s)
 * - Sending a message
 * - Searching for messages
 * - Registering/Unregistering a callback function to check whether a message is sent successfully or not
 * - Registering/Unregistering a callback function to receive notifications when an incoming message is received
 *
 * @section CAPI_MESSAGING_MESSAGES_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/network.telephony\n
 *  - http://tizen.org/feature/network.telephony.mms\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 * <BR>
 */

/**
 * @defgroup CAPI_MESSAGING_MESSAGES_MMS_MODULE MMS
 * @brief The MMS API provides functions to manage attachments and subject of the MMS message.
 * @ingroup CAPI_MESSAGING_MESSAGES_MODULE
 *
 * @addtogroup CAPI_MESSAGING_MESSAGES_MMS_MODULE
 *
 * @section CAPI_MESSAGING_MESSAGES_MMS_MODULE_HEADER Required Header
 *   \#include <messages.h>
 *
 * @section CAPI_MESSAGING_MESSAGES_MMS_MODULE_OVERVIEW Overview
 *
 * The MMS API provides the following functionalities:
 *
 * - Setting/Getting the subject
 * - Attaching the media files(image, video, audio)
 * - Retrieving the attachments
 * - Deleting all attachments
 *
 * @section CAPI_MESSAGING_MESSAGES_MMS_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/network.telephony\n
 *  - http://tizen.org/feature/network.telephony.mms\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */

/**
 * @defgroup CAPI_MESSAGING_MESSAGES_PUSH_MODULE WAP Push
 * @brief The WAP PUSH API provides functions to add/remove incoming callback for WAP Push messages and register current application to push server.
 * @ingroup CAPI_MESSAGING_MESSAGES_MODULE
 *
 * @addtogroup CAPI_MESSAGING_MESSAGES_PUSH_MODULE
 *
 * @section CAPI_MESSAGING_MESSAGES_PUSH_MODULE_HEADER Required Header
 *   \#include <messages.h>
 *
 * @section CAPI_MESSAGING_MESSAGES_PUSH_MODULE_OVERVIEW Overview
 *
 * The WAP PUSH API provides the following functionalities:
 *
 * - Adding/Removing incoming callback for WAP Push messages
 * - Managing the registration
 *
 * @section CAPI_MESSAGING_MESSAGES_PUSH_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/network.telephony\n
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 *
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 *
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 *
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */


#endif /* __TIZEN_MESSAGES_DOC_H__ */
