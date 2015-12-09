#include <stdio.h>
#include <stdlib.h>
#include <dlog.h>

#include <glib.h>

#include <messages.h>

#define TEST_NUMBER "+491726597478"

static GMainLoop *mainloop;

static void sig_quit(int signo)
{
	if (mainloop)
		g_main_loop_quit(mainloop);
}

void _sent_cb(messages_sending_result_e result, void *user_data)
{
	dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "sending result=%d\n", (int)result);
}

int main(int argc, char *argv[])
{
	int ret;

	signal(SIGINT, sig_quit);
	signal(SIGTERM, sig_quit);
	signal(SIGQUIT, sig_quit);
	mainloop = g_main_loop_new(NULL, FALSE);

	messages_service_h svc;
	messages_message_h msg;

	/* open service */
	ret = messages_open_service(&svc);
	if (MESSAGES_ERROR_NONE != ret) {
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "error: messages_open_service() = %d", ret);
		return 1;
	}
	/* create message */
	ret = messages_create_message(MESSAGES_TYPE_MMS, &msg);
	if (MESSAGES_ERROR_NONE != ret) {
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "error: messages_create_message() = %d", ret);
		return 1;
	}

	messages_add_address(msg, TEST_NUMBER, MESSAGES_RECIPIENT_TO);
	messages_set_text(msg, "This is a multi-media message!");

	messages_mms_set_subject(msg, "TEST!");
/*
 * messages_mms_add_attachment(msg, MESSAGES_MEDIA_AUDIO, "/tmp/test.amr");
 *  messages_mms_add_attachment(msg, MESSAGES_MEDIA_IMAGE, "/opt/etc/msg-service/P091120_104633.jpg");
 */
	messages_mms_add_attachment(msg, MESSAGES_MEDIA_IMAGE, "/opt/media/Images/image2.jpg");

	dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "Before Sending\n");

	/* send message */
	ret = messages_send_message(svc, msg, true, _sent_cb, NULL);
	if (MESSAGES_ERROR_NONE != ret) {
		dlog_print(DLOG_DEBUG, "MESSAGE_TEST", "error: messages_send_message() = %d", ret);
		return 1;
	}

	g_main_loop_run(mainloop);
	g_main_loop_unref(mainloop);

	/* destroy */
	messages_destroy_message(msg);
	messages_close_service(svc);

	return 0;
}
