#include <stdio.h>
#include <stdlib.h>

#include <messages.h>


bool _search_cb(messages_message_h msg, int index, int result_count, int total_count, void *user_data)
{
	char *text;

	messages_get_text(msg, &text);

	printf(">>%s\n", text);

	return true;
}

int main(int argc, char *argv[])
{
	int ret;

	messages_service_h svc;

	// open service
	ret = messages_open_service(&svc);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_open_service() = %d", ret);
		return 1;
	}

	ret = messages_foreach_message_from_db(svc, 
			MESSAGES_MBOX_ALL, MESSAGES_TYPE_SMS, 
			"test", NULL, 
			0, 0, _search_cb, NULL);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_foreach_message_from_db() = %d", ret);
		return 1;
	}


	// destroy
	messages_close_service(svc);	


	return 0;
}
