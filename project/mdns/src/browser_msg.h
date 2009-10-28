/**
 * @file browser_msg.h
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */

#ifndef BROWSER_MSG_H_
#define BROWSER_MSG_H_

	#include "macros.h"
	#include "queue.h"
	#include "browser.h"

	/**
	 * Browser Message Class
	 */
	class BMsg {

	public:
		typedef enum {
			BMSG_INVALID=0,
			BMSG_DBUS_CONN_ERROR,
			BMSG_DBUS_ADDFILTER_ERROR,
			BMSG_DBUS_ADDMATCH_ERROR,
			BMSG_DBUS_DISCONNECTED
		} BMsgType;

	public:
		BMsgType type;

	public:
		BMsg() {
			type=BMSG_INVALID;
		}

		BMsg(BMsgType _type) {
			type=_type;
		}

	};


	// Prototypes
	// ^^^^^^^^^^
	void browser_push_simple_msg(queue *q, BMsg::BMsgType type);
	void browser_push_simple_msg(browserParams *bp, BMsg::BMsgType type);


#endif /* BROWSER_MSG_H_ */
