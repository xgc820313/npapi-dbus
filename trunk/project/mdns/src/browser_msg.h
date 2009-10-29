/**
 * @file browser_msg.h
 *
 *  Created on: 2009-10-28
 *      Author: jldupont
 */

#ifndef BROWSER_MSG_H_
#define BROWSER_MSG_H_

	#include <iostream>
	#include <sstream>
	#include <string>

	#include "macros.h"
	#include "queue.h"
	#include "browser.h"

	using namespace std;

	/**
	 * Browser Message Class
	 */
	class BMsg {

	public:
		typedef enum {
			BMSG_INVALID=0,
			BMSG_JSON=1,
			BMSG_MALLOC_ERROR=2,
			BMSG_DBUS_CONN_ERROR=3,
			BMSG_DBUS_ADDFILTER_ERROR=4,
			BMSG_DBUS_ADDMATCH_ERROR=5,
			BMSG_DBUS_SERVICE_BROWSER_ERROR=6,
			BMSG_DBUS_DISCONNECTED=7
		} BMsgType;

	public:
		BMsgType type;
		ostringstream *json_string;

	public:
		BMsg() {
			type=BMSG_INVALID;
			json_string=NULL;
		}

		BMsg(BMsgType _type) {
			type=_type;
			json_string=NULL;
		}
		BMsg(BMsgType _type, ostringstream *_resp) {
			type=_type;
			json_string=_resp;
		}
		~BMsg() {
			if (NULL!=json_string)
				delete json_string;
		}

	};


	// Prototypes
	// ^^^^^^^^^^
	void browser_push_simple_msg(queue *q, BMsg::BMsgType type);
	void browser_push_simple_msg(browserParams *bp, BMsg::BMsgType type);
	void browser_push_msg(browserParams *bp, BMsg *msg);


#endif /* BROWSER_MSG_H_ */
