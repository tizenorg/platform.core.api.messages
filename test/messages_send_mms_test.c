#include <stdio.h>
#include <stdlib.h>

#include <messages.h>

#define TEST_NUMBER "000000000"

void _sent_cb(messages_sending_result_e result, void *user_data)
{
}

int main(int argc, char *argv[])
{
	int ret;

	messages_service_h svc;
	messages_message_h msg;

	// open service
	ret = messages_open_service(&svc);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_open_service() = %d", ret);
		return 1;
	}

	// create message
	ret = messages_create_message(MESSAGES_TYPE_MMS, &msg);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_create_message() = %d", ret);
		return 1;
	}

	messages_add_address(msg, TEST_NUMBER, MESSAGES_RECIPIENT_TO);
	messages_set_text(msg, "This is a multi-media message!");

	messages_mms_set_subject(msg, "TEST!");
//	messages_mms_add_attachment(msg, MESSAGES_MEDIA_VIDEO, "/opt/etc/msg-service/V091120_104905.3gp");
//	messages_mms_add_attachment(msg, MESSAGES_MEDIA_IMAGE, "/opt/etc/msg-service/P091120_104633.jpg");
	
	printf("Before Sending\n");
	
	// send message
	ret = messages_send_message(svc, msg, true, _sent_cb, NULL);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_send_message() = %d", ret);
		return 1;
	}

	// destroy
	messages_destroy_message(msg);
	messages_close_service(svc);	


	return 0;
}
