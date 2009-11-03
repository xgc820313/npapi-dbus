/**
 * @file browser_msg.cc
 *
 * @date 2009-10-28
 * @author jldupont
 */
#include <assert.h>

#include "macros.h"
#include "browser.h"
#include "browser_msg.h"

const char *BMsg::strTypes[] = {
		"invalid",
		"json",
		"malloc_error",
		"dbus_connection_error",
		"dbus_filter_error",
		"dbus_match_rule_error",
		"dbus_service_browser_error",
		"dbus_disconnected",
		"all_for_now",
		"cache_exhausted",
		"exited",
		"init_error",
		// From Client
		"reconnect",
		"exit",
		"exit_and_clean",
		// Management
		"flush_marker"
	};



/**
 * Allows pushing a simple message down the
 * communication queue
 */
void
browser_push_simple_msg(queue *q, BMsg::BMsgType type) {

	BMsg *m = new BMsg(type);

	queue_put(q, (void *) m);
}//

void
browser_push_simple_msg(browserParams *bp, BMsg::BMsgType type) {

	BMsg *m = new BMsg(type);
	queue *q = bp->cc->out;

	queue_put(q, (void *) m);
}//

void
browser_push_msg(browserParams *bp, BMsg *msg) {

	queue *q = bp->cc->out;

	queue_put(q, (void *) msg);
}

void
browser_push_msg(queue *q, BMsg *msg) {

	queue_put(q, (void *) msg);
}//


