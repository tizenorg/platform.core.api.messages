#include <stdio.h>
#include <stdlib.h>

#include <messages.h>

int main(int argc, char *argv[])
{
	int ret;

	messages_service_h svc;

	messages_cb_settings_h settings;

	

	// open service
	ret = messages_open_service(&svc);
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_open_service() = %d", ret);
		return 1;
	}

	ret = messages_cb_load_settings(svc, &settings);	
	if (MESSAGES_ERROR_NONE != ret) {
		printf("error: messages_cb_load_settings() = %d", ret);
		return 1;
	}
	
	


	// close service
	messages_close_service(svc);	


	return 0;
}
