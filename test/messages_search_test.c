#include <stdio.h>
#include <stdlib.h>

#include <messages.h>


bool _search_cb(messages_message_h msg, int index, int result_count, int total_count, void *user_data)
{
	messages_message_type_e msgType;
	messages_message_box_e mbox;
	int ret;
	char *text;
	int i, nAddr;
	char *addr = NULL;
	time_t time;

	messages_get_message_type(msg, &msgType);

	printf("=[%d/%d]==================================\n", index + 1, result_count);
	messages_get_address_count(msg, &nAddr);
	for (i=0; i < nAddr; i++) {
		ret = messages_get_address(msg, i, &addr, NULL);
		if (MESSAGES_ERROR_NONE == ret) {
			printf("Address[%d]: %s\n", i, addr);
			free(addr);
		}
	}

	ret = messages_get_text(msg, &text);
	if (MESSAGES_ERROR_NONE == ret) {
		printf("Text: %s\n", text);
		free(text);
	}

	if (MESSAGES_TYPE_SMS == msgType) {
		printf("Type: SMS\n");
	} else if (MESSAGES_TYPE_MMS == msgType) {
		printf("Type: MMS\n");
		ret = messages_mms_get_subject(msg, &text);
		if (MESSAGES_ERROR_NONE == ret) {
			printf("Subject: %s\n", text);
			free(text);
		}
	} else {
		printf("Type: Unknown");
	}


	ret = messages_get_time(msg, &time);
	printf("Time: %d, %s", (int)time, ctime(&time));
	
	ret = messages_get_mbox_type(msg, &mbox);
	printf("MBOX: %d\n", mbox);


	return true;
}

int main(int argc, char *argv[])
{
	int ret;
	int count;

	messages_service_h svc;

	// open service
	ret = messages_open_service(&svc);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_open_service() = %d", ret);
		return 1;
	}

	ret = messages_foreach_message(svc, 
			MESSAGES_MBOX_ALL, MESSAGES_TYPE_UNKNOWN, 
			NULL, NULL, 
			0, 0, _search_cb, NULL);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_foreach_message_from_db() = %d", ret);
		return 1;
	}

	printf("===============================================\n");
	ret = messages_get_message_count(svc, MESSAGES_MBOX_INBOX, MESSAGES_TYPE_SMS, &count);
	printf("INBOX: %d messages\n", count);
	ret = messages_get_message_count(svc, MESSAGES_MBOX_SENTBOX, MESSAGES_TYPE_SMS, &count);
	printf("SENTBOX: %d messages\n", count);
	ret = messages_get_message_count(svc, MESSAGES_MBOX_OUTBOX, MESSAGES_TYPE_SMS, &count);
	printf("OUTBOX: %d sms messages\n", count);
	ret = messages_get_message_count(svc, MESSAGES_MBOX_OUTBOX, MESSAGES_TYPE_MMS, &count);
	printf("OUTBOX: %d mms messages\n", count);



	// destroy
	messages_close_service(svc);	


	return 0;
}
