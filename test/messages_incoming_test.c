#include <stdio.h>
#include <stdlib.h>
#include <dlog.h>

#include <glib.h>

#include <messages.h>

#define ERROR_CHECK(ret) \
	if (MESSAGES_ERROR_NONE != ret) { \
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "%d: error, ret=%d \n", __LINE__, ret); \
		return(0); \
	}

static GMainLoop *mainloop;

static void sig_quit(int signo)
{
        if(mainloop)
        {
                g_main_loop_quit(mainloop);
        }
}

void incoming_cb(messages_message_h msg, void *user_data)
{
	int ret;
	messages_message_type_e msgType;
	char *text;
	int i, nAddr;
	char *addr = NULL;
	time_t time;
	struct tm ptm;

	dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Incoming Message\n");

	messages_get_address_count(msg, &nAddr);
	for (i=0; i < nAddr; i++) {
		ret = messages_get_address(msg, i, &addr, NULL);
		if (MESSAGES_ERROR_NONE == ret) {
			dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Address[%d]: %s\n", i, addr);
			free(addr);
		}
	}

	ret = messages_get_text(msg, &text);
	if (MESSAGES_ERROR_NONE == ret) {
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Text: %s\n", text);
		free(text);
	} else {
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "%d: error, ret=%d\n", __LINE__, ret);
	}

	
	messages_get_message_type(msg, &msgType);
	switch(msgType) {
	case MESSAGES_TYPE_SMS:
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Type: SMS\n");
		break;
	case MESSAGES_TYPE_MMS:
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Type: MMS\n");
		break;
	default:
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Type: Unknown\n");
		break;
	}
	
	messages_get_time(msg, &time);
	char buf[50];
	if (ctime_r(&time, buf))
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Time: %d, %s", (int)time, ctime_r(&time, buf));

	gmtime_r(&time, &ptm);
	dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "gmtime test: %d.%d.%d %d:%d:%d \n",
			ptm.tm_year, ptm.tm_mon, ptm.tm_mday,
			ptm.tm_hour, ptm.tm_min, ptm.tm_sec
	);
}

int main(int argc, char *argv[])
{
	int ret;
	messages_service_h svc;


        signal(SIGINT, sig_quit);
        signal(SIGTERM, sig_quit);
        signal(SIGQUIT, sig_quit);
	mainloop = g_main_loop_new(NULL, FALSE);

	ret = messages_open_service(&svc);
	ERROR_CHECK(ret);


	messages_set_message_incoming_cb(svc, incoming_cb, NULL);

        g_main_loop_run(mainloop);
        g_main_loop_unref(mainloop);

	messages_close_service(svc);


	return 0;
}

