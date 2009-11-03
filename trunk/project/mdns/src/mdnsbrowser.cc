/**
 * @file mdnsbrowser.cc
 *
 *  Created on: 2009-11-02
 *      Author: jldupont
 */
#include "mdnsbrowser.h"

MDNSBrowser::MDNSBrowser() {
	initState=FALSE;
}//

MDNSBrowser::~MDNSBrowser() {
}

bool
MDNSBrowser::init(void) {

	// if this malloc fails then there are
	// much bigger problems looming
	cc=(CommChannel *) malloc(sizeof(CommChannel));
	if (NULL==cc)
		return FALSE;

	BrowserReturnCode result=browser_init(cc);

	if (BROWSER_OK!=result) {
		free(cc);
	}

	initState=(BROWSER_OK==result);

	return initState;
}//

/**
 * The worker thread will take care of all cleaning
 *
 * From hereon, no more access is allowed e.g.
 *  no more messages to/from the worker thread.
 */
void
MDNSBrowser::release(void) {

	if (NULL==cc) {
		DBGLOG(LOG_ERR, "MDNSBrowser: communication channel no up!");
		return;
	}

	pushMsg( BMsg::BMSG_EXIT_CLEAN );

}//

void MDNSBrowser::pushMsg(BMsg::BMsgType type) {

	if (NULL==cc) {
		DBGLOG(LOG_ERR, "MDNSBrowser: communication channel down.");
		return;
	}
	browser_push_simple_msg(cc->in, type);

}//

void
MDNSBrowser::pushMsg(BMsg *msg) {

	if (NULL==cc) {
		DBGLOG(LOG_ERR, "MDNSBrowser: communication channel down.");
		return;
	}
	browser_push_msg(cc->in, msg);

}//

BMsg *
MDNSBrowser::popMsg(void) {

	if (!initState) {
		BMsg *msg=new BMsg(BMsg::BMSG_INIT_ERROR);
		return msg;
	}

	if (NULL==cc) {
		DBGLOG(LOG_ERR, "MDNSBrowser: communication channel down.");
		return NULL;
	}

	return (BMsg*) queue_get_nb(cc->out);
}//
