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
	queue *q = bp->q;

	queue_put(q, (void *) m);
}//

void
browser_push_msg(browserParams *bp, BMsg *msg) {

	queue *q = bp->q;

	queue_put(q, (void *) msg);
}



// =================





/**
 * Simple message push i.e. just a type
 */
void
BrowserMessaging::spush(BMsg::BMsgType type) {

	BMsg *m = new BMsg(type);

	queue_put(cc.out, msg);
}//

/**
 * Complete Message push
 */
void
BrowserMessaging::push(BMsg::BMsgType type, BMsg& msg) {

	queue_put(cc.out, msg);

}//

